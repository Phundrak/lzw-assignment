#include "bitpack.hh"
#include "common.hh"
#include <algorithm>
#include <cmath>

using std::uint16_t;
using std::vector;
using uchar = unsigned char;
using vuint16 = vector<uint16_t>;
using vuchar = vector<uchar>;
using ustring = std::basic_string<unsigned char>;

int max(const int t_n) {
  return ipow(2, t_n) - 1;
}

///////////////////////////////////////////////////////////////////////////////
//                                  packing                                  //
///////////////////////////////////////////////////////////////////////////////

vuchar pack(const vuint16 &t_input) {
  return pack_n(t_input.begin(), t_input.end(), 9);
}

/**
 *  Packs \p t_input into unsigned char, assuming  the max value of t_input
 * only takes \p t_n bits
 *
 *  \param t_input_begin pointer to the beginning of the vector of values to be packed
 *  \param t_input_end pointer to the end of the input vector
 *  \param t_n maximum size of an input value in bits
 *  \return Returns a vector of unsigned char containing the packed values from t_input
 */
vuchar pack_n(const vuint16::const_iterator t_input_begin,
              const vuint16::const_iterator t_input_end, const int t_n) {
  if (t_n == 16) {
    return pack_16(t_input_begin, t_input_end);
  }
  const int max_value = max(t_n); // max value held within t_n bits

#ifdef Debug
  std::printf("%d bits! %ld chars remaining\n", t_n,
              std::distance(t_input_begin, t_input_end));
  std::printf("max: %d\n", max_value);
#endif

  int step = t_n / 8;
  int left_shift = 0;
  int middle_shift = 0;
  int right_shift = 0;
  uchar current_char = 0;
  bool char_touched = false;
  vuchar ret{};

  for (auto it = t_input_begin; it != t_input_end; ++it) {
    if ((left_shift += step) >= t_n) {
      left_shift = (left_shift - t_n) + step;
    }
    ret.push_back((current_char | (*it >> left_shift)) & 0xFFu);
    // current_char = 0;

    bool zero_right_shift = (right_shift == 0);
    right_shift -= step;
    if (right_shift < 0) {
      middle_shift = std::abs(right_shift);
      right_shift = 8 - std::abs(right_shift);
      if (!zero_right_shift) {
        current_char = (*it >> middle_shift) & 0xFFu;
        ret.push_back(current_char);
      }
    }
    if (right_shift == 0) {
      current_char = *it & 0xffu;
      ret.push_back(current_char);
      current_char = 0;
      char_touched = false;
    } else {
      current_char = (*it << right_shift) & 0xFFu;
      char_touched = true;
    }

    // il faut écrire la valeur pour la décompression
    if (*it >= max_value) {
      if (char_touched) {
        ret.push_back(current_char);
      }
      const auto next_vec = pack_n(it, t_input_end, t_n + 1);
      ret.insert(ret.end(), next_vec.begin(), next_vec.end());
      return ret;
    }
  }
  if (char_touched) {
    ret.push_back(current_char);
  }
  return ret;
}

vuchar pack_16(const vuint16::const_iterator t_input_begin,
               const vuint16::const_iterator t_input_end) {
#ifdef Debug
  std::printf("16 bits! %ld chars remaining\n",
              std::distance(t_input_begin, t_input_end));
#endif
  vuchar ret{};
  std::for_each(t_input_begin, t_input_end, [&](const auto value) {
    ret.push_back((value >> 8) & 0xFFu);
    ret.push_back(value & 0xFFu);
  });
  return ret;
}

///////////////////////////////////////////////////////////////////////////////
//                                 unpacking                                 //
///////////////////////////////////////////////////////////////////////////////

constexpr uint16_t mask_n(int t_nb_bits) {
  if (t_nb_bits == 0) {
    return 0;
  }
  uint16_t mask = mask_n(t_nb_bits - 1);
  mask = static_cast<uint16_t>(mask << 1);
  mask |= 0x1;
  return mask;
}

constexpr uint16_t masks[17] = {
    mask_n(0),  mask_n(1),  mask_n(2),  mask_n(3),  mask_n(4),  mask_n(5),
    mask_n(6),  mask_n(7),  mask_n(8),  mask_n(9),  mask_n(10), mask_n(11),
    mask_n(12), mask_n(13), mask_n(14), mask_n(15), mask_n(16)};

vuint16 unpack(ustring &&t_input) {
  return unpack_n(t_input.begin(), t_input.end(), 9);
}

vuint16 unpack_n(const ustring::const_iterator t_begin,
                 const ustring::const_iterator t_end, const int t_n) {
#ifdef Debug
  std::printf("Chunk! %d bits, %ld compressed chars\n", t_n,
              std::distance(t_begin, t_end));
#endif
  if (t_n == 16) {
    return unpack_16(t_begin, t_end);
  }
  int step = t_n / 8;
  int left_shift = 0;
  int right_shift = 0;
  vuint16 ret{};
  const int max_value = max(t_n);
  for (auto it = t_begin; it < t_end - 1; /* nope */) {
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
        current_char |= *++it << (-right_shift);
      }
      right_shift = 8 + right_shift;
    }
    current_char |= *(++it) >> right_shift;
    // char made!
    ret.push_back(current_char &= masks[t_n]);
    if (current_char >= max_value) {
      const auto next_vec = unpack_n(it + 1, t_end, t_n + 1);
      ret.insert(ret.end(), next_vec.begin(), next_vec.end());
      return ret;
    }
    if (right_shift == 0) {
      ++it;
    }
  }
  return ret;
}

vuint16 unpack_16(const ustring::const_iterator t_begin,
                  const ustring::const_iterator t_end) {
  vuint16 ret{};
  for (auto it = t_begin; it < t_end; ++it) {
    ret.push_back(static_cast<uint16_t>((*it << 8) | *(++it)));
  }
  return ret;
}
