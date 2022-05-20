#include <rnu/ecs/component.hpp>

namespace rnu
{
  auto component_base::types() -> std::vector<std::tuple<component_creator_fun, component_deleter_fun, size_t>>& {
    static std::vector<std::tuple<component_creator_fun, component_deleter_fun, size_t>> t;
    return t;
  }
}