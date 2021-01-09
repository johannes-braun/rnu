#include <rnu/math/math.hpp>
#include <iostream>
#include <chrono>
#include <functional>
#include <rnu/math/operators.hpp>

/// element_wise
/// * Takes N vectors
/// * The vector sizes are equal
/// * Takes functor with N parameters
/// * The value type of the i'th vector is convertible to the i'th parameter of the functor
/// * The function never returns by-ref
/// * If the functor has a return type, the function itself returns a vector with the common size and the return type of the function with const, volatile and references removed.
/// * Otherwise the function returns void
//
//
//namespace rnu
//{
//    template<typename... VectorTypes> 
//    concept vector_types_compatible = (vector_type<std::decay_t<VectorTypes>> && ...) && dimensionally_equal<VectorTypes...>;
//
//    namespace detail
//    {
//        template<typename FunctorType, typename... VectorTypes, size_t... Component>
//        void element_wise_void(FunctorType&& functor, VectorTypes&&... vectors, std::index_sequence<Component...>)
//        {
//            size_t c = 0;
//            ((c = Component, (functor(vectors[c]...))), ...);
//        }
//        template<typename ResultType, typename FunctorType, typename... VectorTypes, size_t... Component>
//        auto element_wise_vtype(FunctorType&& functor, VectorTypes&&... vectors, std::index_sequence<Component...>)
//        {
//            using result_t = vec<ResultType, head_t<VectorTypes...>::component_count>;
//            size_t c = 0;
//            return result_t{ (c = Component, (functor(vectors[c]...)))... };
//        }
//    }
//
//    template<typename FunctorType, typename... VectorTypes>
//    auto element_wise(FunctorType&& functor, VectorTypes&&... vectors) requires requires(reference_type<VectorTypes>... values, FunctorType&& fun) { fun(values...); }
//    {
//        using return_value_type = std::invoke_result_t<FunctorType, reference_type<VectorTypes>...>;
//        if (std::is_same_v<return_value_type, void>)
//            detail::element_wise_void<FunctorType, VectorTypes...>(std::forward<FunctorType>(functor), std::forward<VectorTypes>(vectors)..., std::make_index_sequence<head_t<VectorTypes...>::component_count>());
//        else
//            return detail::element_wise_vtype<return_value_type, FunctorType, VectorTypes...>(std::forward<FunctorType>(functor), std::forward<VectorTypes>(vectors)..., std::make_index_sequence<head_t<VectorTypes...>::component_count>());
//    }
//}

int main(int argc, char** argv)
{

    rnu::vec3 a{ 2, 4, 9 };
    rnu::vec3 b{ 1, 2, 3 };
    auto nv = rnu::element_wise([](float x, float y) { return x + y; }, a, b);

    return 0;
    /*using XX = rnu::reference_type<const rnu::vec3>;

    rnu::vec3 vector;

    rnu::mat2 mat;
    rnu::mat2 mat2;
    rnu::mat2i mat32;
    mat32 << 1;
    
    rnu::mat2 matrix(1, 2, 20.f, 234);

    mat = rnu::element_wise([](float f, float b) {
        return f + b;
        }, mat, mat2);
    auto x = rnu::element_wise([](float& f, float& fa) { return f += fa; }, vector, vector);

    rnu::quat quatasd;
    rnu::quatd d = rnu::element_wise([](float& r, float q) { 
        r += q;
        return double(r); }, quatasd, quatasd);

    (vector + vector * vector - vector / vector) + 10;
    vector + 5;
    5 + vector;
    vector += 210;

    auto vav = vector == vector;
    {
        std::vector<rnu::mat2> vectors(100000);
        std::generate(vectors.begin(), vectors.end(), [] { return rnu::mat2{ float(rand()), float(rand()), float(rand()), float(rand()) }; });
        auto start = std::chrono::steady_clock::now();
        for (size_t i = 0; i < 100000; ++i)
        {
            float faf[4];
            faf[0] = vectors[i].at(0, 0) + vectors[i].at(0, 0);
            faf[1] = vectors[i].at(0, 1) + vectors[i].at(0, 1);
            faf[2] = vectors[i].at(1, 0) + vectors[i].at(1, 0);
            faf[3] = vectors[i].at(1, 1) + vectors[i].at(1, 1);
            vectors[i].at(0, 0) = faf[0];
            vectors[i].at(0, 1) = faf[1];
            vectors[i].at(1, 0) = faf[2];
            vectors[i].at(1, 1) = faf[3];
        }
        std::cout << "Time: " << (std::chrono::steady_clock::now() - start).count() << " nanos\n";
    }

    {
        std::vector<rnu::mat2> vectors(100000);
        std::generate(vectors.begin(), vectors.end(), [] { return rnu::mat2{ float(rand()), float(rand()), float(rand()), float(rand()) }; });
        auto start = std::chrono::steady_clock::now();
        for (size_t i = 0; i < 100000; ++i)
        {
            vectors[i] = rnu::element_wise([](float a) { return a + a; }, vectors[i]);
        }
        std::cout << "Time: " << (std::chrono::steady_clock::now() - start).count() << " nanos\n";
    }*/

    //rnu::vec4i vack;
    //vack % 9;
    //--vack;

    /*rnu::camera<float> cam;
    cam.axis(0, 0, 0, 0, 0, 0, 0);
    cam.matrix(true);
    cam.mouse(0, 0, 0);
    cam.projection(0, 0, 0, 0, 0);

    rnu::mat4 o;
    auto val = any_of(o != o);*/

    //   rnu::quat q = static_cast<rnu::quat>(rnu::quatd());
    //   q *= q;

    //   rnu::mat<float, 3, 3> matrix{
    //       2.f, 3.f, 4.f,
    //       1.f, 4.f, 4.f,
    //       9.f, 1.f, 3.f
    //   };
    //   rnu::vec3 a, b;
    //   auto x = matrix * (inverse(q) * ((a * 5) + (b * 2) + cross(a, b)));

    //   rnu::mat4i mm;
    //   mm *= (-!+~(mm));
    //   mm++;
    //   --mm;

       //rnu::mat<float, 3, 3> ttt{
       //	2, 0, 0,
       //	0, 2, 0,
       //	0, 0, 2
       //};
       //auto xax = det(inverse(ttt));

    //   rnu::vec3 aaaa = rnu::mat<float, 1, 3>();
    //   rnu::mat<float, 1, 3> aosp = aaaa;

    return 0;
}