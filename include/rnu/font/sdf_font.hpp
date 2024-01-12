#pragma once

#include <rnu/font/font.hpp>
#include <rnu/math/rect.hpp>
#include <rnu/lines.hpp>
#include <string_view>
#include <optional>
#include <vector>
#include <span>
#include <utility>
#include <unordered_map>

namespace rnu
{
  using unicode_ranges = std::span<std::pair<char16_t, char16_t> const>;

  class sdf_font
  {
  public:
    friend class sdf_font;
    struct set_glyph_t
    {
      rnu::glyph_id glyph;
      rnu::rect2f bounds;
      rnu::rect2f uvs;
    };

    [[nodiscard]] sdf_font(
      int atlas_width, float base_size, float sdf_width, rnu::font font, unicode_ranges unicode_ranges);
    [[nodiscard]] std::vector<set_glyph_t> text_set(
      std::wstring_view str, int* num_lines = nullptr, float* x_max = nullptr, float* y_max = nullptr) const;
    void dump(std::vector<std::uint8_t>& image, int& w, int& h) const;

    template<typename Func>
    void foreach_glyph(Func&& fn)
    {
      for (auto const& [gly, info] : _infos)
      {
        fn(gly, info.packed_bounds, info.default_bounds);
      }
    }

    rnu::rect2f glyph_uv(glyph_id glyph) const;

    rnu::vec2i atlas_size() const;

    [[nodiscard]] float base_size() const;
    [[nodiscard]] float sdf_width() const;
    [[nodiscard]] float em_factor(float target_size) const;
    [[nodiscard]] float line_height() const;
    [[nodiscard]] rnu::font const& font() const;

  private:
    void load(int atlas_width, float base_size, float sdf_width, rnu::font font, unicode_ranges unicode_ranges);

    std::vector<goop::lines::line> const& load_glyph(rnu::glyph_id glyph) const;
    std::optional<rnu::glyph_id> ligature(
      rnu::font const& font, rnu::font_feature_info const& lig_feature, std::span<rnu::glyph_id const> glyphs) const;

    struct glyph_info
    {
      char16_t character = {};
      rnu::glyph_id id = {};
      float scale = {};
      rnu::rect2f default_bounds = {};
      rnu::rect2f packed_bounds = {};
    };

    mutable std::optional<rnu::font> _font;
    float _base_size = 0;
    float _sdf_width = 0;
    int _width = 0;
    int _height = 0;
    std::unordered_map<rnu::glyph_id, glyph_info> _infos;
  };
}