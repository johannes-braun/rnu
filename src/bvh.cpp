#include <rnu/algorithm/bvh.hpp>
#include <numeric>
#include <algorithm>
#include <format>
#include <iostream>
#include <bitset>

namespace rnu {
struct sah_axis_info_t {
  std::array<aabb_t, bvh::binned_sah_bin_count> axis_bins{};
  std::array<aabb_t, bvh::binned_sah_bin_count> axis_bounds{};
  std::array<int, bvh::binned_sah_bin_count> axis_counts{};
  std::array<aabb_t, bvh::binned_sah_bin_count> accum_aabbs{};
  std::array<aabb_t, bvh::binned_sah_bin_count> accum_aabbsr{};
  std::array<aabb_t, bvh::binned_sah_bin_count> accum_cbounds{};
  std::array<int, bvh::binned_sah_bin_count> accum_counts{};
};

struct build_state_t {
  aabb_t full_aabb;
  std::vector<aabb_t> aabbs;
  std::vector<bvh::index_type> indices;

  std::vector<aabb_t> inserter_aabbs;
  std::vector<bvh::index_type> inserter_indices;

  std::vector<sah_axis_info_t> sah_axises;
};

const primitive_split_func dont_split = [](primitive_split_request_t req) -> primitive_split_result_t {
  return primitive_split_result_t{.lower = req.aabb};
};

const primitive_split_func split_aabbs = [](primitive_split_request_t req) -> primitive_split_result_t {
  auto [a, b] = req.aabb.split(req.axis, req.position);

  primitive_split_result_t result;
  result.lower = a;
  result.higher = b;

  return result;
};

[[nodiscard]] std::tuple<int, float, bool, int, int> bvh::compute_split_axis(
    bvh_node_t const& node, build_state_t& state) {
  aabb_t sub_aabb;
  auto const count = node.second_child - node.first_child + 1;
  for (auto index = node.first_child; index <= node.second_child; ++index) {
    const auto centroid = state.aabbs[index].centroid();
    sub_aabb.enclose(centroid);
  }

  auto const sub_aabb_size = sub_aabb.dimension();
  auto const step = sub_aabb_size / float(bvh::binned_sah_bin_count);

  auto& axises = state.sah_axises;

  for (auto index = node.first_child; index <= node.second_child; ++index) {
    const auto centroid = state.aabbs[index].centroid();
    auto relative = centroid - sub_aabb.min;
    auto integral = relative / step;
    rnu::vec3i const bin_id =
        rnu::clamp<rnu::vec3i>(rnu::vec3i(integral), rnu::vec3i(0), rnu::vec3i(bvh::binned_sah_bin_count - 1));
    for (auto i = 0; i < 3; ++i) {
      axises[i].axis_bins[bin_id[i]].enclose(state.aabbs[index]);
      axises[i].axis_bounds[bin_id[i]].enclose(state.aabbs[index].centroid());
      axises[i].axis_counts[bin_id[i]]++;
    }
  }

  const auto enclose_reduce = [](aabb_t current, const aabb_t& next) -> aabb_t {
    current.enclose(next);
    return current;
  };
  for (int axis = 0; axis < 3; ++axis) {
    std::partial_sum(
        begin(axises[axis].axis_bins), end(axises[axis].axis_bins), begin(axises[axis].accum_aabbs), enclose_reduce);
    std::partial_sum(rbegin(axises[axis].axis_bins), rend(axises[axis].axis_bins), rbegin(axises[axis].accum_aabbsr),
        enclose_reduce);
    std::partial_sum(rbegin(axises[axis].axis_bounds), rend(axises[axis].axis_bounds),
        rbegin(axises[axis].accum_cbounds), enclose_reduce);
    std::partial_sum(begin(axises[axis].axis_counts), end(axises[axis].axis_counts), begin(axises[axis].accum_counts));
  }

  constexpr auto cost_traverse = 7.f;
  constexpr auto cost_intersect = 2.f;

  const float total_surface_area = node.aabb().surface_area();
  const auto cost_area = [&](auto surface_area) { return surface_area / total_surface_area; };

  float best_cost = std::numeric_limits<float>::max();
  int best_axis = 0;
  int best_axis_partition = 0;
  for (int axis = 0; axis < 3; ++axis) {
    for (size_t index = 0; index < bvh::binned_sah_bin_count - 1; ++index) {
      const auto count_first = axises[axis].accum_counts[index];
      const auto count_second = axises[axis].accum_counts[bvh::binned_sah_bin_count - 1 - index];
      const auto area_first = axises[axis].accum_aabbs[index].surface_area();
      const auto area_second = axises[axis].accum_aabbsr[index].surface_area();

      auto const cost = cost_traverse +
                        cost_intersect * (count_first * cost_area(area_first) + count_second * cost_area(area_second));

      if (cost <= best_cost) {
        best_axis = axis;
        best_axis_partition = static_cast<int>(index);
        best_cost = cost;
      }
    }
  }

  float const total_count = static_cast<float>(axises[best_axis].accum_counts.back());
  auto const cost_without_split = cost_intersect * total_count;

  auto const nval = sub_aabb.min[best_axis] + step[best_axis] * (best_axis_partition + 1);
  return std::make_tuple(best_axis, nval, best_cost < cost_without_split, 0, best_axis_partition);
}

[[nodiscard]] std::vector<aabb_t> generate_triangle_bounds(std::span<detail::default_index_type const> indices,
    std::function<detail::default_point_type(detail::default_index_type)> const& get_point) {
  std::vector<aabb_t> aabbs(indices.size() / 3);
  for (size_t i = 0; i < indices.size(); i += 3) {
    const auto& p0 = get_point(indices[i + 0]);
    const auto& p1 = get_point(indices[i + 1]);
    const auto& p2 = get_point(indices[i + 2]);

    aabb_t& aabb = aabbs[i / 3];
    aabb.enclose(p0);
    aabb.enclose(p1);
    aabb.enclose(p2);
    aabb.pad(1e-5f);
    aabb.weighted_centroid = (p0 + p1 + p2) / 3.0f;
  }
  return aabbs;
}

std::vector<aabb_t> generate_triangle_bounds(
    std::span<detail::default_index_type const> indices, std::span<detail::default_point_type const> points) {
  std::vector<aabb_t> aabbs(indices.size() / 3);
  for (size_t i = 0; i < indices.size(); i += 3) {
    const auto& p0 = points[indices[i + 0]];
    const auto& p1 = points[indices[i + 1]];
    const auto& p2 = points[indices[i + 2]];

    aabb_t& aabb = aabbs[i / 3];
    aabb.enclose(p0);
    aabb.enclose(p1);
    aabb.enclose(p2);
    aabb.pad(1e-5f);
    aabb.weighted_centroid = (p0 + p1 + p2) / 3.0f;
  }
  return aabbs;
}

bvh::bvh(std::span<aabb_t const> aabbs, primitive_split_func split) : m_split_primitive(split ? split : dont_split) {
  build_state_t build_state;
  build_state.aabbs = {aabbs.begin(), aabbs.end()};

  for (size_t i = 0; i < aabbs.size(); ++i) build_state.full_aabb.enclose(aabbs[i]);

  create(build_state);
}

template <typename Stack> void check_shift_count(std::int32_t shift) {
  static constexpr auto max_size = sizeof(std::int32_t) * 8;

  if (shift >= max_size) {
    throw std::runtime_error("Stack size exceeded.");
  } else if (shift < 0) {
    throw std::runtime_error("Shifted past the beginning of the stack.");
  }
}

#ifdef RNU_BVH_HAS_GENERATOR
[[nodiscard]] std::experimental::generator<bvh::index_type> bvh::traverse(
    std::function<std::optional<float>(aabb_t const&)> should_traverse) const {
  bool hits_primitive = false;

  using bit_stack = std::uint32_t;
  using bit = std::uint32_t;

  index_type node_base_index = 0;
  bit_stack visited_stack = 0;
  bit_stack left_right_stack = 0;
  index_type node_index = 0;
  std::int32_t shift_count = 0;

  auto tmp = should_traverse(m_nodes[node_base_index].node.aabb());
  bool hits_any = tmp != std::nullopt;
  while (hits_any) {
    aligned_node_t current = m_nodes[node_index];
    while (!(current.node.is_leaf())) {
      aligned_node_t const& first_child = m_nodes[current.node.first_child + node_base_index];
      aligned_node_t const& second_child = m_nodes[current.node.second_child + node_base_index];

      auto t_left = should_traverse(first_child.node.aabb());
      auto t_right = should_traverse(second_child.node.aabb());

      bool hits_left = !!t_left;
      bool hits_right = !!t_right;
      if (!hits_left && !hits_right)
        break;
      bool use_right = !hits_left || (hits_right && *t_left >= *t_right);

      check_shift_count<bit_stack>(++shift_count);
      left_right_stack = bit(use_right) | (left_right_stack << bit(1));
      visited_stack = bit(hits_left && hits_right) | (visited_stack << bit(1));
      node_index = (use_right ? current.node.second_child : current.node.first_child) + node_base_index;
      current = m_nodes[node_index];
    }
    if ((current.node.is_leaf())) {
      for (index_type tri = current.node.first_child; tri <= current.node.second_child; ++tri) {
        co_yield m_reordered_indices[tri];
      }
    }
    while ((visited_stack & bit(1)) != 1) {
      if (visited_stack == 0)
        co_return;
      node_index = m_nodes[node_index].node.parent() + node_base_index;
      visited_stack >>= bit(1);
      left_right_stack >>= bit(1);
      check_shift_count<bit_stack>(--shift_count);
    }
    node_index =
        node_base_index + (((left_right_stack & 0x1) == 0x1)
                                  ? m_nodes[m_nodes[node_index].node.parent() + node_base_index].node.first_child
                                  : m_nodes[m_nodes[node_index].node.parent() + node_base_index].node.second_child);
    visited_stack ^= bit(1);
  }
  co_return;
}

std::experimental::generator<bvh::index_type> bvh::traverse(ray_t const& ray) const {
  return traverse([&ray](aabb_t const& aabb) { return ray.intersect(aabb); });
}
#endif

std::vector<aligned_node_t> const& bvh::nodes() const noexcept {
  return m_nodes;
}

std::vector<bvh::index_type> const& bvh::reordered_indices() const noexcept {
  return m_reordered_indices;
}

void bvh::create(build_state_t& initial_state) {
  m_reordered_indices.resize(initial_state.aabbs.size());
  std::iota(begin(m_reordered_indices), end(m_reordered_indices), index_type(0));
  m_nodes.reserve(2 * m_reordered_indices.size());

  initial_state.indices = m_reordered_indices;
  initial_state.aabbs.reserve(m_reordered_indices.size() * 3);
  initial_state.indices.reserve(m_reordered_indices.size() * 3);
  initial_state.inserter_aabbs.reserve(m_reordered_indices.size());
  initial_state.inserter_indices.reserve(m_reordered_indices.size());

  index_type current_node_index = 0;
  ptrdiff_t active_nodes = 1;

  m_nodes.emplace_back().node = bvh_node_t{.min_extents = initial_state.full_aabb.min,
      .type_and_parent = bvh_node_t::make_type_and_parent(true, 0),
      .max_extents = initial_state.full_aabb.max,
      .first_child = 0,
      .second_child = static_cast<index_type>(m_reordered_indices.size()) - 1};

  std::vector<size_t> depths;
  depths.reserve(2 * m_reordered_indices.size());
  depths.emplace_back(0);

  while (active_nodes--) {
    bvh_node_t& current_node = m_nodes[current_node_index].node;

    initial_state.sah_axises.clear();
    initial_state.sah_axises.resize(3);
    const auto split_nodes = split(current_node_index, current_node, initial_state);

    if (split_nodes.has_value()) {
      const auto& [first, second] = split_nodes.value();
      active_nodes += 2;
      current_node.first_child = static_cast<index_type>(m_nodes.size());
      current_node.second_child = static_cast<index_type>(m_nodes.size() + 1);
      current_node.make_leaf(false);

      m_nodes.emplace_back(aligned_node_t{.node = first}).node.set_parent(current_node_index);
      m_nodes.emplace_back(aligned_node_t{.node = second}).node.set_parent(current_node_index);
      depths.emplace_back(depths[current_node_index] + 1);
      depths.emplace_back(depths[current_node_index] + 1);
    }
    current_node_index++;
  }
  m_nodes.shrink_to_fit();

  size_t max_depth = *std::ranges::max_element(depths);
  // std::format_to(std::ostreambuf_iterator(std::cout), "Max depth was {}\n", max_depth);

  m_reordered_indices = initial_state.indices;
}
std::optional<std::pair<bvh_node_t, bvh_node_t>> bvh::split(
    index_type current_node_index, const bvh_node_t& current_node, build_state_t& build_state) {
  if (current_node.second_child - current_node.first_child < min_leaf_primitives)
    return std::nullopt;

  const auto [split_axis, split_plane, should_split, num_splits, split_partition] =
      compute_split_axis(current_node, build_state);

  if (!should_split)
    return std::nullopt;

  const auto split_test = [this, &build_state, axis = split_axis, plane = split_plane](
                              size_t index) { return build_state.aabbs[index].centroid()[axis] < plane; };

  auto first = current_node.first_child;
  auto last = current_node.second_child + 1;

  auto const num_elements = last - first;

  build_state.inserter_aabbs.clear();
  build_state.inserter_indices.clear();

  int local_num_splits = 0;
  for (auto i = first; i != last; ++i) {
    float const thres = 0.05f * build_state.full_aabb.dimension()[split_axis];
    primitive_split_result_t split;
    if (build_state.aabbs[i].dimension()[split_axis] > thres) {
      split = m_split_primitive(primitive_split_request_t{
          .index = i, .aabb = build_state.aabbs[i], .axis = split_axis, .position = split_plane});
    } else {
      split.lower = build_state.aabbs[i];
    }

    if ((split.higher && split.higher->dimension()[split_axis] < thres) ||
        (split.lower && split.lower->dimension()[split_axis] < thres)) {
      split.higher = std::nullopt;
      split.lower = build_state.aabbs[i];
    }

    if (split.lower && !split.higher) {
      build_state.aabbs[i] = split.lower.value();
    } else if (split.higher && !split.lower) {
      build_state.aabbs[i] = split.higher.value();
    } else if (split.higher && split.lower) {
      auto const index = build_state.indices[i];
      build_state.aabbs[i] = split.higher.value();

      build_state.inserter_aabbs.push_back(split.lower.value());
      build_state.inserter_indices.push_back(index);
      ++local_num_splits;
    }
  }
  if (!build_state.inserter_aabbs.empty()) {
    build_state.aabbs.insert(
        build_state.aabbs.begin() + first, begin(build_state.inserter_aabbs), end(build_state.inserter_aabbs));
    build_state.indices.insert(
        build_state.indices.begin() + first, begin(build_state.inserter_indices), end(build_state.inserter_indices));
  }

  auto const increment_if_larger = [](index_type ref, int by, bvh_node_t& node) {
    if (node.is_leaf()) {
      if (node.first_child >= ref)
        node.first_child += by;
      if (node.second_child >= ref)
        node.second_child += by;
    }
  };

  // Partition
  while (first != last && split_test(first)) first++;
  if (first != last) {
    for (auto i = first; i != last + local_num_splits; ++i) {
      if (split_test(i)) {
        std::swap(build_state.indices[i], build_state.indices[first]);
        std::swap(build_state.aabbs[i], build_state.aabbs[first]);
        ++first;
      }
    }
  }
  if (local_num_splits > 0)
    for (auto i = 0; i < m_nodes.size(); ++i) increment_if_larger(last, local_num_splits, m_nodes[i].node);

  if (local_num_splits != num_splits) {
    int i = 0;
  }

  const index_type split_index = first == current_node.first_child ? first + 1 : first;

  std::pair<bvh_node_t, bvh_node_t> split_result;
  split_result.first.first_child = current_node.first_child;
  split_result.first.second_child = split_index - 1;
  split_result.second.first_child = split_index;
  split_result.second.second_child = current_node.second_child + local_num_splits;

  aabb_t first_aabb;
  for (auto i = split_result.first.first_child; i <= split_result.first.second_child; ++i)
    first_aabb.enclose(build_state.aabbs[i]);
  split_result.first.set_aabb(first_aabb);
  aabb_t second_aabb;
  for (auto i = split_result.second.first_child; i <= split_result.second.second_child; ++i)
    second_aabb.enclose(build_state.aabbs[i]);
  split_result.second.set_aabb(second_aabb);

  return split_result;
}

std::optional<float> ray_t::intersect(
    const rnu::vec3 v1, const rnu::vec3 v2, const rnu::vec3 v3, rnu::vec2& barycentric) {
  constexpr float float_epsilon = 1e-23f;
  constexpr float border_epsilon = 1e-6f;
  rnu::vec3 e1 = v2 - v1;
  rnu::vec3 e2 = v3 - v1;
  rnu::vec3 P = cross(rnu::vec3(direction), e2);
  float det = dot(e1, P);
  if (det > -float_epsilon && det < float_epsilon)
    return std::nullopt;
  float inv_det = 1.0f / det;
  rnu::vec3 T = origin - v1;
  barycentric.x = dot(T, P) * inv_det;
  if (barycentric.x < -border_epsilon || barycentric.x > 1.0f + border_epsilon)
    return std::nullopt;
  rnu::vec3 Q = cross(T, e1);
  barycentric.y = dot(rnu::vec3(direction), Q) * inv_det;
  if (barycentric.y < -border_epsilon || barycentric.x + barycentric.y > 1.0f + border_epsilon)
    return std::nullopt;
  float tt = dot(e2, Q) * inv_det;
  if (tt > float_epsilon) {
    return tt;
  }
  return std::nullopt;
}

std::optional<float> ray_t::intersect(aabb_t const& aabb) const noexcept {
  rnu::vec3 inv_direction = 1.f / (direction);
  rnu::vec3 t135 = (aabb.min - origin) * inv_direction;
  rnu::vec3 t246 = (aabb.max - origin) * inv_direction;

  rnu::vec3 min_values = min(t135, t246);
  rnu::vec3 max_values = max(t135, t246);

  float tmin = rnu::max(rnu::max(min_values.x, min_values.y), min_values.z);
  float tmax = rnu::min(rnu::min(max_values.x, max_values.y), max_values.z);

  return (tmax >= 0 && tmin <= tmax && tmin <= length) ? std::optional(tmin < 0 ? tmax : tmin) : std::nullopt;
}

aabb_t bvh::aabb() const noexcept {
  return m_nodes[0].node.aabb();
}
} // namespace myrt