#include "bitpack.hh"
#include <algorithm>
#include <cmath>

using std::uint16_t;
using std::vector;
using uchar = unsigned char;
using vuint16 = vector<uint16_t>;
using vuchar = vector<uchar>;

constexpr int ipow(int base, int exp) {
  int result = 1;
  for (;;) {
    if (exp & 1) {
      result *= base;
    }
    exp >>= 1;
    if (exp == 0) {
      break;
    }
    base *= base;
  }
  return result;
}

/**
 *  Packs \p t_input into unsigned char, assuming  the max value of t_input only
 *  takes \p t_n bits
 *
 *  \param t_input_begin pointer to the beginning of the vector of values to be
 * packed \param t_input_end pointer to the end of the input vector \param t_n
 * maximum size of an input value in bits \return Returns a vector of unsigned
 * char containing the packed values from t_input
 */
vuchar pack_n(const vuint16::const_iterator t_input_begin,
              const vuint16::const_iterator t_input_end, int t_n) {
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
    if (*t_input_end >= max_value) {
      ret.push_back(current_char);
      const auto next_vec = pack_n(it, t_input_end, t_n + 1);
      ret.insert(ret.end(), next_vec.begin(), next_vec.end());
      return ret;
    }
    left_shift += step;
    if (left_shift >= t_n) {
      left_shift = (left_shift - t_n) + step;
    }
    ret.push_back((current_char | (*it >> left_shift)) & 0xFF);
    current_char = 0;

    bool zero_right_shift = (right_shift == 0);
    right_shift -= step;
    if (right_shift < 0) {
      middle_shift = std::abs(right_shift);
      right_shift = 8 + std::abs(right_shift);
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
  }
  if (current_char != 0) {
    ret.push_back(current_char);
  }
  return ret;
}

vuchar pack_16(const vuint16::const_iterator t_input_begin,
               const vuint16::const_iterator t_input_end) {
  vuchar ret{};
  std::for_each(t_input_begin, t_input_end, [&](const auto value) {
    ret.push_back((value >> 8) & 0xFF);
    ret.push_back(value & 0xFF);
  });
  return ret;
}

vuchar pack(const vuint16 t_input) {
  vuchar ret{};
  constexpr int max_value = ipow(2, 8);
  for (auto it = t_input.begin(); it != t_input.end(); ++it) {
    if (*it >= max_value) {
      const auto next_vec =
          pack_n(static_cast<vuint16::const_iterator>(it), t_input.end(), 9);
      ret.insert(ret.end(), next_vec.begin(), next_vec.end());
      return ret;
    }
    ret.push_back(static_cast<unsigned char>(*it));
  }
  return ret;
}
