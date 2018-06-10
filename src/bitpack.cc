#include "bitpack.hh"
#include "common.hh"
#include <cmath>
#include <algorithm>

using std::uint16_t;
using std::vector;
using uchar = unsigned char;
using vuint16 = vector<uint16_t>;
using vuchar = vector<uchar>;
using ustring = std::basic_string<unsigned char>;

///////////////////////////////////////////////////////////////////////////////
//                                  packing                                  //
///////////////////////////////////////////////////////////////////////////////

[[nodiscard]] vuchar pack(const vuint16 &t_input) {
  return pack_n(t_input.begin(), t_input.end(), 9);
}

/**
 *  Packs \p t_input into unsigned char, assuming  the max value of t_input
 * only takes \p t_n bits
 *
 *  \param t_input_begin pointer to the beginning of the vector of values to
 * be packed \param t_input_end pointer to the end of the input vector
 * \param t_n maximum size of an input value in bits \return Returns a
 * vector of unsigned char containing the packed values from t_input
 */
[[nodiscard]] vuchar
pack_n(const vuint16::const_iterator t_input_begin,
       const vuint16::const_iterator t_input_end, const int t_n) {
  if (t_n == 16) {
    return pack_16(t_input_begin, t_input_end);
  }
  vuchar ret{};

  // max value with current number of bits + 1
  const int max_value = ipow(2, t_n) - 1;

  uchar current_char = 0;
  int step = t_n / 8;
  int left_shift = 0;
  int middle_shift = 0;
  int right_shift = 0;

  for (auto it = t_input_begin; it != t_input_end; ++it) {
    left_shift += step;
    if (left_shift >= t_n) {
      left_shift = (left_shift - t_n) + step;
    }
    ret.push_back((current_char | (*it >> left_shift)) & 0xFFu);
    current_char = 0;

    bool zero_right_shift = (right_shift == 0);
    right_shift -= step;
    if (right_shift < 0) {
      middle_shift = std::abs(right_shift);
      right_shift = 8 - std::abs(right_shift);
      if (!zero_right_shift) {
        ret.push_back((*it >> middle_shift) & 0xFF);
      }
    }
    if (right_shift == 0) {
      ret.push_back(*it & 0xff);
      current_char = 0;
    } else {
      current_char = (*it << right_shift) & 0xFF;
    }

    // at the end so we can detect `max_value` while parsing for bit-unpacking
    if (*t_input_end >= max_value) {
      if (current_char != 0) {
        ret.push_back(current_char);
      }
      const auto next_vec = pack_n(it, t_input_end, t_n + 1);
      ret.insert(ret.end(), next_vec.begin(), next_vec.end());
      return ret;
    }
  }
  if (current_char != 0) {
    ret.push_back(current_char);
  }
  return ret;
}

[[nodiscard]] vuchar pack_16(const vuint16::const_iterator t_input_begin,
                             const vuint16::const_iterator t_input_end) {
  vuchar ret{};
  std::for_each(t_input_begin, t_input_end, [&](const auto value) {
    ret.push_back((value >> 8) & 0xFF);
    ret.push_back(value & 0xFF);
  });
  return ret;
}

///////////////////////////////////////////////////////////////////////////////
//                                 unpacking //
///////////////////////////////////////////////////////////////////////////////

uint16_t mask_n(int t_nb_bits) {
  if(t_nb_bits == 0) {
    return 0;
  }
  uint16_t mask = mask_n(t_nb_bits - 1);
  mask <<= 1;
  mask |= 0x1;
  return mask;
}

[[nodiscard]] vuint16 unpack(ustring &&t_input) {
  return unpack_n(t_input.begin(), t_input.end(), 9);
}

[[nodiscard]] vuint16 unpack_n(const ustring::const_iterator t_begin,
                               const ustring::const_iterator t_end,
                               const int t_n) {
  int step = t_n / 8;
  int left_shift = 0;
  int right_shift = 0;
  vuint16 ret{};
  const uint16_t mask = mask_n(t_n);
  const int max_value = ipow(2, t_n);
  for (auto it = t_begin; it < t_end;) {
    uint16_t current_char = 0;
    // left bits
    left_shift =
        ((left_shift += step) >= t_n) ? (left_shift - t_n) + step : left_shift;
    current_char = static_cast<uint16_t>(*it << left_shift);
    // right bits
    bool zero_rs = right_shift;
    right_shift -= step;
    if (right_shift < 0) {
      // optional middle bits before right bits
      if (zero_rs) {
        current_char |= *(++it) << std::abs(right_shift);
      }
      right_shift = 8 - std::abs(right_shift);
    }
    current_char |= *(++it) >> right_shift;
    // char made!
    ret.push_back(current_char &= mask);
    if (current_char >= max_value) {
      const auto next_vec = unpack_n(it, t_end, t_n + 1);
      ret.insert(ret.end(), next_vec.begin(), next_vec.end());
      return ret;
    }
  }
  return ret;
}

[[nodiscard]] vuint16 unpack_16(const ustring::const_iterator t_begin,
                                const ustring::const_iterator t_end) {
  vuint16 ret{};
  for (auto it = t_begin; it < t_end; ++it) {
    ret.push_back(static_cast<uint16_t>((*it << 8) | *(++it)));
  }
  return ret;
}
