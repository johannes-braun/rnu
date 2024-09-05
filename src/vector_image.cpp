#include <rnu/vector_image.hpp>
#include <charconv>

namespace rnu
{
  namespace detail 
  {
    constexpr int count_for_type(path_action_type type)
    {
      switch (type)
      {
      case path_action_type::arc_rel:
      case path_action_type::arc_abs: return 7;
      case path_action_type::curve_to_rel:
      case path_action_type::curve_to_abs: return 6;
      case path_action_type::smooth_curve_to_rel:
      case path_action_type::smooth_curve_to_abs: return 4;
      case path_action_type::quad_bezier_to_rel:
      case path_action_type::quad_bezier_to_abs: return 4;
      case path_action_type::smooth_quad_bezier_to_rel:
      case path_action_type::smooth_quad_bezier_to_abs: return 2;
      case path_action_type::move_to_rel:
      case path_action_type::move_to_abs: return 2;
      case path_action_type::line_to_rel:
      case path_action_type::line_to_abs: return 2;
      case path_action_type::horizontal_rel:
      case path_action_type::horizontal_abs: return 1;
      case path_action_type::vertical_rel:
      case path_action_type::vertical_abs: return 1;
      case path_action_type::close:
      case path_action_type::close_alt: return 0;
      default: return 0;
      }
    }

    void parse_path_impl(std::string_view path, std::vector<path_action_t>& output, parse_result_t* result = nullptr)
    {
      output.clear();
      if (result)
        *result = parse_result_t{};
      char const* ptr = path.data();
      char const* end = &*std::prev(path.end()) + 1;

      while (end != ptr) {
        auto const character = *ptr;
        if (std::isalpha(character))
        {
          // Is token.
          path_action_t next_action;
          next_action.type = static_cast<path_action_type>(character);

          const int max_count = count_for_type(next_action.type);

          const char* token_position = ptr;
          ++ptr;
          int count = 0;
          while (!std::isalpha(*ptr) && ptr != end)
          {
            if (max_count == count)
            {
              output.push_back(next_action);
              token_position = ptr;
              count = 0;
            }

            char* result_ptr = const_cast<char*>(ptr);
            next_action.values[count++] = std::strtod(ptr, &result_ptr);
            ptr = result_ptr;

            while ((!std::isalnum(*ptr) && *ptr != '-') && ptr != end)
              ++ptr;
          }
          if (count != max_count)
          {
            if (result)
            {
              result->result = parse_result_t::result_code::err_not_enough_values;
              result->error_token_position = token_position;
            }
            return;
          }
          output.push_back(next_action);
        }
        while ((!std::isalnum(*ptr) && *ptr != '-') && ptr != end)
          ++ptr;
      }
    }
  }

  vector_image::vector_image(std::string_view path, int x, int y, int w, int h) {
    parse(path, x, y, w, h);
  }
  void vector_image::parse(std::string_view path, int x, int y, int w, int h) {
    _bounds = { {x, y}, {w, h} };
    detail::parse_path_impl(path, _path, &_result);
  }
}