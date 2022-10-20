#pragma once

#include <utility>

namespace rnu::character_ranges
{
  using char16_range = std::pair<char16_t, char16_t>;

  constexpr auto make_char_range(char16_t first, char16_t last)
  {
    return char16_range(first, last);
  }

  static constexpr auto basic_latin = char16_range(0x0000, 0x007F);
  static constexpr auto c1_controls_and_latin_1_supplement = char16_range(0x0080, 0x00FF);
  static constexpr auto latin_extended_a = char16_range(0x0100, 0x017F);
  static constexpr auto latin_extended_b = char16_range(0x0180, 0x024F);
  static constexpr auto ipa_extensions = char16_range(0x0250, 0x02AF);
  static constexpr auto spacing_modifier_letters = char16_range(0x02B0, 0x02FF);
  static constexpr auto combining_diacritical_marks = char16_range(0x0300, 0x036F);
  static constexpr auto greek_and_coptic = char16_range(0x0370, 0x03FF);
  static constexpr auto cyrillic = char16_range(0x0400, 0x04FF);
  static constexpr auto cyrillic_supplement = char16_range(0x0500, 0x052F);
  static constexpr auto armenian = char16_range(0x0530, 0x058F);
  static constexpr auto hebrew = char16_range(0x0590, 0x05FF);
  static constexpr auto arabic = char16_range(0x0600, 0x06FF);
  static constexpr auto syriac = char16_range(0x0700, 0x074F);
  static constexpr auto thaana = char16_range(0x0780, 0x07BF);
  static constexpr auto devanagari = char16_range(0x0900, 0x097F);
  static constexpr auto bengali_and_assamese = char16_range(0x0980, 0x09FF);
  static constexpr auto gurmukhi = char16_range(0x0A00, 0x0A7F);
  static constexpr auto gujarati = char16_range(0x0A80, 0x0AFF);
  static constexpr auto oriya = char16_range(0x0B00, 0x0B7F);
  static constexpr auto tamil = char16_range(0x0B80, 0x0BFF);
  static constexpr auto telugu = char16_range(0x0C00, 0x0C7F);
  static constexpr auto kannada = char16_range(0x0C80, 0x0CFF);
  static constexpr auto malayalam = char16_range(0x0D00, 0x0DFF);
  static constexpr auto sinhala = char16_range(0x0D80, 0x0DFF);
  static constexpr auto thai = char16_range(0x0E00, 0x0E7F);
  static constexpr auto lao = char16_range(0x0E80, 0x0EFF);
  static constexpr auto tibetan = char16_range(0x0F00, 0x0FFF);
  static constexpr auto myanmar = char16_range(0x1000, 0x109F);
  static constexpr auto georgian = char16_range(0x10A0, 0x10FF);
  static constexpr auto hangul_jamo = char16_range(0x1100, 0x11FF);
  static constexpr auto ethiopic = char16_range(0x1200, 0x137F);
  static constexpr auto cherokee = char16_range(0x13A0, 0x13FF);
  static constexpr auto unified_canadian_aboriginal_syllabics = char16_range(0x1400, 0x167F);
  static constexpr auto ogham = char16_range(0x1680, 0x169F);
  static constexpr auto runic = char16_range(0x16A0, 0x16FF);
  static constexpr auto tagalog = char16_range(0x1700, 0x171F);
  static constexpr auto hanunoo = char16_range(0x1720, 0x173F);
  static constexpr auto buhid = char16_range(0x1740, 0x175F);
  static constexpr auto tagbanwa = char16_range(0x1760, 0x177F);
  static constexpr auto khmer = char16_range(0x1780, 0x17FF);
  static constexpr auto mongolian = char16_range(0x1800, 0x18AF);
  static constexpr auto limbu = char16_range(0x1900, 0x194F);
  static constexpr auto tai_le = char16_range(0x1950, 0x197F);
  static constexpr auto khmer_symbols = char16_range(0x19E0, 0x19FF);
  static constexpr auto phonetic_extensions = char16_range(0x1D00, 0x1D7F);
  static constexpr auto latin_extended_additional = char16_range(0x1E00, 0x1EFF);
  static constexpr auto greek_extended = char16_range(0x1F00, 0x1FFF);
  static constexpr auto general_punctuation = char16_range(0x2000, 0x206F);
  static constexpr auto superscripts_and_subscripts = char16_range(0x2070, 0x209F);
  static constexpr auto currency_symbols = char16_range(0x20A0, 0x20CF);
  static constexpr auto combining_diacritical_marks_for_symbols = char16_range(0x20D0, 0x20FF);
  static constexpr auto letterlike_symbols = char16_range(0x2100, 0x214F);
  static constexpr auto number_forms = char16_range(0x2150, 0x218F);
  static constexpr auto arrows = char16_range(0x2190, 0x21FF);
  static constexpr auto mathematical_operators = char16_range(0x2200, 0x22FF);
  static constexpr auto miscellaneous_technical = char16_range(0x2300, 0x23FF);
  static constexpr auto control_pictures = char16_range(0x2400, 0x243F);
  static constexpr auto optical_character_recognition = char16_range(0x2440, 0x245F);
  static constexpr auto enclosed_alphanumerics = char16_range(0x2460, 0x24FF);
  static constexpr auto box_drawing = char16_range(0x2500, 0x257F);
  static constexpr auto block_elements = char16_range(0x2580, 0x259F);
  static constexpr auto geometric_shapes = char16_range(0x25A0, 0x25FF);
  static constexpr auto miscellaneous_symbols = char16_range(0x2600, 0x26FF);
  static constexpr auto dingbats = char16_range(0x2700, 0x27BF);
  static constexpr auto miscellaneous_mathematical_symbols_a = char16_range(0x27C0, 0x27EF);
  static constexpr auto supplemental_arrows_a = char16_range(0x27F0, 0x27FF);
  static constexpr auto braille_patterns = char16_range(0x2800, 0x28FF);
  static constexpr auto supplemental_arrows_b = char16_range(0x2900, 0x297F);
  static constexpr auto miscellaneous_mathematical_symbols_b = char16_range(0x2980, 0x29FF);
  static constexpr auto supplemental_mathematical_operators = char16_range(0x2A00, 0x2AFF);
  static constexpr auto miscellaneous_symbols_and_arrows = char16_range(0x2B00, 0x2BFF);
  static constexpr auto cjk_radicals_supplement = char16_range(0x2E80, 0x2EFF);
  static constexpr auto kangxi_radicals = char16_range(0x2F00, 0x2FDF);
  static constexpr auto ideographic_description_characters = char16_range(0x2FF0, 0x2FFF);
  static constexpr auto cjk_symbols_and_punctuation = char16_range(0x3000, 0x303F);
  static constexpr auto hiragana = char16_range(0x3040, 0x309F);
  static constexpr auto katakana = char16_range(0x30A0, 0x30FF);
  static constexpr auto bopomofo = char16_range(0x3100, 0x312F);
  static constexpr auto hangul_compatibility_jamo = char16_range(0x3130, 0x318F);
  static constexpr auto kanbun_kunten = char16_range(0x3190, 0x319F);
  static constexpr auto bopomofo_extended = char16_range(0x31A0, 0x31BF);
  static constexpr auto katakana_phonetic_extensions = char16_range(0x31F0, 0x31FF);
  static constexpr auto enclosed_cjk_letters_and_months = char16_range(0x3200, 0x32FF);
  static constexpr auto cjk_compatibility = char16_range(0x3300, 0x33FF);
  static constexpr auto cjk_unified_ideographs_extension_a = char16_range(0x3400, 0x4DBF);
  static constexpr auto yijing_hexagram_symbols = char16_range(0x4DC0, 0x4DFF);
  static constexpr auto cjk_unified_ideographs = char16_range(0x4E00, 0x9FAF);
  static constexpr auto yi_syllables = char16_range(0xA000, 0xA48F);
  static constexpr auto yi_radicals = char16_range(0xA490, 0xA4CF);
  static constexpr auto hangul_syllables = char16_range(0xAC00, 0xD7AF);
  static constexpr auto high_surrogate_area = char16_range(0xD800, 0xDBFF);
  static constexpr auto low_surrogate_area = char16_range(0xDC00, 0xDFFF);
  static constexpr auto private_use_area = char16_range(0xE000, 0xF8FF);
  static constexpr auto cjk_compatibility_ideographs = char16_range(0xF900, 0xFAFF);
  static constexpr auto alphabetic_presentation_forms = char16_range(0xFB00, 0xFB4F);
  static constexpr auto arabic_presentation_forms_a = char16_range(0xFB50, 0xFDFF);
  static constexpr auto variation_selectors = char16_range(0xFE00, 0xFE0F);
  static constexpr auto combining_half_marks = char16_range(0xFE20, 0xFE2F);
  static constexpr auto cjk_compatibility_forms = char16_range(0xFE30, 0xFE4F);
  static constexpr auto small_form_variants = char16_range(0xFE50, 0xFE6F);
  static constexpr auto arabic_presentation_forms_b = char16_range(0xFE70, 0xFEFF);
  static constexpr auto halfwidth_and_fullwidth_forms = char16_range(0xFF00, 0xFFEF);
  static constexpr auto specials = char16_range(0xFFF0, 0xFFFF);
}