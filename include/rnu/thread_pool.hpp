#pragma once

#include <thread>
#include <future>
#include <vector>
#include <functional>
#include <mutex>
#include <memory>
#include <deque>
#include <experimental/generator>

namespace rnu
{
  template<typename Func, typename... Args>
  concept callable = requires(Func func, Args... args) { func(args...); };

  template<typename Func, typename Data, typename Type>
  concept async_loader = requires(Func func, Data d) { { func(d) }->std::convertible_to<Type>; };

  template<typename T>
  concept is_atomic = std::is_trivially_copyable_v<T> && std::is_copy_constructible_v<T> &&
    std::is_move_constructible_v<T> && std::is_copy_assignable_v<T> && std::is_move_assignable_v<T>;

  template<typename T>
  concept copyable_or_movable = std::copyable<T> || std::movable<T>;

  template<typename T>
  concept iterable = requires(T t) { begin(t); end(t); };

  template<typename T>
  consteval auto iterable_type_get() {
    if constexpr (iterable<T>)
      return std::decay_t<decltype(*begin(std::declval<T>()))>{};
  }

  template<typename ContainerOfFutures> 
  void await_all(ContainerOfFutures&& c)
    requires requires(ContainerOfFutures c) { { begin(c)->wait() }; { end(c) }; }
  {
    for (auto const& awaitable : c)
      awaitable.wait();
  }

  template<typename R>
  bool is_ready(std::future<R> const& f)
  {
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
  }

  template<typename Arg, typename Result>
  struct ref_fun
  {
    using type = Result(Arg&);
  };
  
  template<typename Result>
  struct ref_fun<void, Result>
  {
    using type = Result();
  };

  template<typename Arg, typename Result>
  using ref_fun_t = typename ref_fun<Arg, Result>::type;

  template<typename ThreadData = void>
  class basic_thread_pool {
  public:
    template<typename Res>
    using job_async_t = std::function<ref_fun_t<ThreadData, Res>>;
    using job_fun_t = job_async_t<void>;

    [[nodiscard]] basic_thread_pool(unsigned concurrency = std::thread::hardware_concurrency()) requires(std::is_void_v<ThreadData>);
    [[nodiscard]] basic_thread_pool(std::function<ThreadData(unsigned id)> create_data = [] { return ThreadData{}; }, unsigned concurrency = std::thread::hardware_concurrency()) requires(!std::is_void_v<ThreadData>);
    ~basic_thread_pool();

    template<typename Res = void>
    [[nodiscard]] auto run_async(job_async_t<Res> func);

    void run_detached(job_fun_t job);

    [[nodiscard]] unsigned concurrency() const;

  private:
    void thread_loop(std::stop_token stop_token, ThreadData* data);

    std::vector<std::jthread> m_threads;
    std::deque<job_fun_t> m_jobs;
    std::mutex m_jobs_mutex;
    std::condition_variable m_wait_condition;
  };

  template<copyable_or_movable T>
  struct async_resource {
    struct empty_mutex {};
    struct empty_lock {};

    using iterable_type = decltype(iterable_type_get<T>());
    using value_type = std::conditional_t<is_atomic<T>, std::atomic<T>, T>;
    using mutex_type = std::conditional_t<is_atomic<T>, empty_mutex, std::mutex>;
    using lock_type = std::conditional_t<is_atomic<T>, empty_lock, std::unique_lock<std::mutex>>;

    [[nodiscard]] async_resource() = default;
    [[nodiscard]] async_resource(T&& value) requires std::movable<T> : m_value(std::move(value)) {}
    [[nodiscard]] async_resource(T const& value) requires std::copyable<T> : m_value(value) {}
    template<typename Pt, async_loader<Pt, T> Func>
    [[nodiscard]] async_resource(basic_thread_pool<Pt>& pool, Func&& loader) {
      load_resource(pool, std::forward<Func>(loader));
    }

    [[nodiscard]] std::experimental::generator<iterable_type const*> iterate() const requires iterable<T> {
      auto const lock = make_lock();
      for (iterable_type const& item : m_value)
        co_yield &item;
    }

    [[nodiscard]] std::experimental::generator<iterable_type*> iterate() requires iterable<T> {
      auto const lock = make_lock();
      for (iterable_type& item : m_value)
        co_yield &item;
    }

    template<callable<T const&> ApplyFun>
    void current(ApplyFun&& apply) const
    {
      auto const lock = make_lock();
      apply(static_cast<T>(m_value));
    }

    [[nodiscard]] T&& extract_current() requires std::movable<T> {
      auto const lock = make_lock();
      return std::move(m_value);
    }

    template<typename Pt, async_loader<Pt, T> Func>
    bool load_resource(basic_thread_pool<Pt>& pool, Func&& loader)
    {
      if (!is_ready()) return false;
      m_current_process = pool.run_async([ld = std::forward<Func>(loader), this](auto&&... args){
        auto value = ld(std::forward<decltype(args)>(args)...);
        auto const lock = make_lock();
        if constexpr (std::movable<T>)
          m_value = std::move(value);
        else if constexpr (std::copyable<T>)
          m_value = value;
        });
      return true;
    }

    void wait() const {
      m_current_process.wait();
    }

    bool is_ready()
    {
      return !m_current_process.valid() ||
        m_current_process.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

  private:
    lock_type make_lock() const {
      if constexpr (is_atomic<T>)
        return empty_lock{};
      else
        return std::unique_lock<std::mutex>(m_value_mutex);
    }

    value_type m_value;
    mutable mutex_type m_value_mutex;
    std::future<void> m_current_process;
  };

  template<typename ThreadData>
  template<typename Res>
  auto basic_thread_pool<ThreadData>::run_async(job_async_t<Res> func)
  {
    using result_type = Res;
    auto promise = std::make_shared<std::promise<result_type>>();
    std::future<result_type> future = promise->get_future();

    std::unique_lock<std::mutex> lock(m_jobs_mutex);
    m_jobs.push_back([p = std::move(promise), f = std::move(func)](auto&&... args) mutable{
      try {
        if constexpr (std::same_as<result_type, void>)
        {
          f(std::forward<decltype(args)>(args)...);
          p->set_value();
        }
        else
        {
          p->set_value(f(std::forward<decltype(args)>(args)...));
        }
      }
      catch (...)
      {
        p->set_exception(std::current_exception());
      }
    });
    m_wait_condition.notify_one();
    return future;
  }

  template<typename ThreadData>
  [[nodiscard]] basic_thread_pool<ThreadData>::basic_thread_pool(std::function<ThreadData(unsigned id)> create_data, unsigned concurrency) requires(!std::is_void_v<ThreadData>)
  {
    for (unsigned i = 0; i < concurrency; ++i)
    {
      auto promise = std::make_shared<std::promise<void>>();
      auto creation_future = promise->get_future(); 
      m_threads.push_back(std::jthread([this, p = std::move(promise), i, &create_data](std::stop_token stop_token){
          auto data = create_data(i);
          p->set_value();
          thread_loop(stop_token, &data);
        }));

      // ! Important ! 
      // Ensures that "create_data" is still a valid reference in the thread function.
      // Also ensures that there may not be any race conditions between the creation calls.
      creation_future.wait();
    }
  }

  template<typename ThreadData>
  basic_thread_pool<ThreadData>::basic_thread_pool(unsigned concurrency) requires(std::is_void_v<ThreadData>)
  {
    for (unsigned i = 0; i < concurrency; ++i)
      m_threads.push_back(std::jthread([this] (std::stop_token stop_token) {
          thread_loop(stop_token, nullptr);
        }));
  }

  template<typename ThreadData>
  basic_thread_pool<ThreadData>::~basic_thread_pool()
  {
    for (auto& thread : m_threads) thread.request_stop();

    std::unique_lock<std::mutex> lock(m_jobs_mutex);
    m_wait_condition.notify_all();
  }

  template<typename ThreadData>
  void basic_thread_pool<ThreadData>::run_detached(job_fun_t job)
  {
    std::unique_lock<std::mutex> lock(m_jobs_mutex);
    m_jobs.push_back(std::move(job));
    m_wait_condition.notify_one();
  }

  template<typename ThreadData>
  unsigned basic_thread_pool<ThreadData>::concurrency() const
  {
    return static_cast<unsigned>(m_threads.size());
  }

  template<typename ThreadData>
  void basic_thread_pool<ThreadData>::thread_loop(std::stop_token stop_token, ThreadData* data)
  {
    while (!stop_token.stop_requested()) {

      std::unique_lock<std::mutex> lock(m_jobs_mutex);

      m_wait_condition.wait(lock, [&] { return stop_token.stop_requested() || !m_jobs.empty(); });

      if (!stop_token.stop_requested())
      {
        auto const fun = std::move(m_jobs.front());
        m_jobs.pop_front();
        lock.unlock();

        if constexpr (std::is_void_v<ThreadData>)
          fun();
        else
          fun(*data);

        lock.lock();
      }
    }
  }

  using thread_pool = basic_thread_pool<void>;
}