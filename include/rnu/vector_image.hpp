#pragma once

#include <rnu/math/math.hpp>
#include <charconv>
#include <cassert>
#include <cctype>
#include <stdexcept>
#include <vector>

namespace rnu
{
  enum class path_action_type : char
  {
    arc_rel = 'a',
    arc_abs = 'A',
    curve_to_rel = 'c',
    curve_to_abs = 'C',
    smooth_curve_to_rel = 's',
    smooth_curve_to_abs = 'S',
    quad_bezier_to_rel = 'q',
    quad_bezier_to_abs = 'Q',
    smooth_quad_bezier_to_rel = 't',
    smooth_quad_bezier_to_abs = 'T',
    move_to_rel = 'm',
    move_to_abs = 'M',
    line_to_rel = 'l',
    line_to_abs = 'L',
    horizontal_rel = 'h',
    horizontal_abs = 'H',
    vertical_rel = 'v',
    vertical_abs = 'V',
    close = 'z',
    close_alt = 'Z'
  };

  constexpr bool is_relative(path_action_type type)
  {
    const auto character = static_cast<char>(type);
    // < z, exclude path closing, its always absolute.
    return character < 'z' && character >= 'a';
  }

  struct line_data_t 
  {
    rnu::vec2d value;
  };
  struct move_data_t 
  {
    rnu::vec2d value;
  };
  struct smooth_quad_bezier_data_t 
  {
    rnu::vec2d value;
  };
  struct horizontal_data_t 
  {
    double value;
  };
  struct vertical_data_t
  {
    double value;
  };
  struct quad_bezier_data_t {
    double x1;
    double y1;
    double x;
    double y;
  };
  struct curve_to_data_t {
    double x1;
    double y1;
    double x2;
    double y2;
    double x;
    double y;
  };
  struct smooth_curve_to_data_t {
    double x2;
    double y2;
    double x;
    double y;
  };
  struct arc_data_t {
    double rx;
    double ry;
    double x_axis_rotation;
    double large_arc_flag;
    double sweep_flag;
    double x;
    double y;
  };
  struct close_data_t {
  };

  struct path_action_t
  {
    path_action_t() : values{ 0 } {
    }

    path_action_type type;
    union {
      double values[7]{};
      line_data_t line_data;
      move_data_t move_data;
      quad_bezier_data_t quad_bezier_data;
      smooth_quad_bezier_data_t smooth_quad_bezier_data;
      horizontal_data_t horizontal_data;
      vertical_data_t vertical_data;
      curve_to_data_t curve_data;
      smooth_curve_to_data_t smooth_curve_to_data;
      arc_data_t arc_data;
      close_data_t close_data;
    };

    rnu::vec2d start_point{ 0,0 };
  };

  template<typename Visitor>
  decltype(auto) visit(path_action_t const& action, Visitor&& visitor)
  {
    switch (action.type)
    {
    case path_action_type::arc_rel:
    case path_action_type::arc_abs: return visitor(action.arc_data);
    case path_action_type::curve_to_rel:
    case path_action_type::curve_to_abs: return visitor(action.curve_data);
    case path_action_type::smooth_curve_to_rel:
    case path_action_type::smooth_curve_to_abs: return visitor(action.smooth_curve_to_data);
    case path_action_type::quad_bezier_to_rel:
    case path_action_type::quad_bezier_to_abs: return visitor(action.quad_bezier_data);
    case path_action_type::smooth_quad_bezier_to_rel:
    case path_action_type::smooth_quad_bezier_to_abs: return visitor(action.smooth_quad_bezier_data);
    case path_action_type::move_to_rel:
    case path_action_type::move_to_abs: return visitor(action.move_data);
    case path_action_type::line_to_rel:
    case path_action_type::line_to_abs: return visitor(action.line_data);
    case path_action_type::horizontal_rel:
    case path_action_type::horizontal_abs: return visitor(action.horizontal_data);
    case path_action_type::vertical_rel:
    case path_action_type::vertical_abs: return visitor(action.vertical_data);
    case path_action_type::close:
    case path_action_type::close_alt: return visitor(action.close_data);
    }
    assert(false);
    throw std::runtime_error("Invalid state: path_action_type cannot be determined.");
  }

  inline void move_cursor(path_action_t const& action, rnu::vec2d& cursor)
  {
    switch (action.type)
    {
    case path_action_type::arc_rel: cursor += rnu::vec2d{ action.arc_data.x, action.arc_data.y }; break;
    case path_action_type::arc_abs: cursor = rnu::vec2d{ action.arc_data.x, action.arc_data.y }; break;
    case path_action_type::curve_to_rel: cursor += rnu::vec2d{ action.curve_data.x, action.curve_data.y }; break;
    case path_action_type::curve_to_abs: cursor = rnu::vec2d{ action.curve_data.x, action.curve_data.y }; break;
    case path_action_type::smooth_curve_to_rel: cursor += rnu::vec2d{ action.smooth_curve_to_data.x, action.smooth_curve_to_data.y }; break;
    case path_action_type::smooth_curve_to_abs: cursor = rnu::vec2d{ action.smooth_curve_to_data.x, action.smooth_curve_to_data.y }; break;
    case path_action_type::quad_bezier_to_rel: cursor += rnu::vec2d{ action.quad_bezier_data.x, action.quad_bezier_data.y }; break;
    case path_action_type::quad_bezier_to_abs: cursor = rnu::vec2d{ action.quad_bezier_data.x, action.quad_bezier_data.y }; break;
    case path_action_type::smooth_quad_bezier_to_rel: cursor += action.smooth_quad_bezier_data.value; break;
    case path_action_type::smooth_quad_bezier_to_abs: cursor = action.smooth_quad_bezier_data.value; break;
    case path_action_type::move_to_rel: cursor += action.move_data.value; break;
    case path_action_type::move_to_abs: cursor = action.move_data.value; break;
    case path_action_type::line_to_rel: cursor += action.line_data.value; break;
    case path_action_type::line_to_abs: cursor = action.line_data.value; break;
    case path_action_type::horizontal_rel: cursor.x += action.horizontal_data.value; break;
    case path_action_type::horizontal_abs: cursor.x = action.horizontal_data.value; break;
    case path_action_type::vertical_rel:cursor.y += action.vertical_data.value; break;
    case path_action_type::vertical_abs:cursor.y = action.vertical_data.value; break;
    case path_action_type::close:
    case path_action_type::close_alt:  break;
    }
  }

  struct parse_result_t
  {
    enum class result_code
    {
      success,
      err_not_enough_values
    } result = result_code::success;
    const char* error_token_position = nullptr;
  };

  class vector_image
  {
  public:
    [[nodiscard]] vector_image() = default;
    [[nodiscard]] vector_image(std::string_view path, int x, int y, int w, int h);
    void parse(std::string_view path, int x, int y, int w, int h);

    [[nodiscard]]
    constexpr parse_result_t result() const noexcept { return _result; }
    [[nodiscard]]
    constexpr std::vector<path_action_t> const& path() const noexcept { return _path; }
    [[nodiscard]]
    constexpr rnu::rect2f const& bounds() const noexcept { return _bounds; }

  private:
    rnu::rect2f _bounds;
    parse_result_t _result;
    std::vector<path_action_t> _path;
  };
}