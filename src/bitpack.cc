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

int max(const int t_n) { return ipow(2, t_n) - 1; }

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

///////////////////////////////////////////////////////////////////////////////
//                                  packing                                  //
///////////////////////////////////////////////////////////////////////////////

vuchar pack(const vuint16 &t_input) {
  vuchar vec{};
  return pack_n(t_input.begin(), t_input.end(), vec, 9);
}

vuchar pack_n(const vuint16::const_iterator t_input_begin,
              const vuint16::const_iterator t_input_end, vuchar &t_res,
              int t_n) {
  if (t_n == 16) {
    return pack_16(t_input_begin, t_input_end, t_res);
  }
  const int max_value = max(t_n); // max value held within t_n bits

  int step = t_n % 8;
  int left_shift = 0;
  int right_shift = 0;
  uchar current_char = 0;
  bool char_touched = false;

  // pour chaque élément
  for (auto it = t_input_begin; it != t_input_end; ++it) {
    // si on a atteint ou dépassé la valeur maximale, on change de nombre de
    // bits
    if (*it >= max_value) {
      // écriture du masque pour notifier à la décompression du changement de
      // bits
      if ((left_shift += step) >= t_n) {
        left_shift = (left_shift - t_n) + step;
      }
      const auto mask = masks[t_n] >> left_shift;
      t_res.push_back(static_cast<uchar>(current_char | mask));
      bool zero_rs = (right_shift == 0);

      right_shift -= step;
      if (right_shift < 0 && !zero_rs) {
        // si right_shift est inférieur à zéro
        // si right_shift était différent de zéro, alors extra octet
        current_char = static_cast<uchar>(masks[t_n] >> (-right_shift) & 0xFFU);
        t_res.push_back(current_char);
      }
      t_res.push_back(static_cast<uchar>(masks[t_n]));
      return pack_n(it, t_input_end, t_res, t_n + 1);
    }

    // écriture normale
    if ((left_shift += step) >= t_n) {
      left_shift = (left_shift - t_n) + step;
    }
    t_res.push_back(
        static_cast<uchar>(current_char | (*it >> left_shift & 0xFFU)));

    bool zero_rs = (right_shift == 0);
    right_shift -= step;
    if (right_shift < 0) {
      if (!zero_rs) {
        current_char = static_cast<uchar>(*it >> (-right_shift) & 0xFFU);
        t_res.push_back(current_char);
      }
      right_shift = 8 + right_shift;
    }
    if (right_shift == 0) {
      current_char = static_cast<uchar>(*it & 0xFFU);
      t_res.push_back(current_char);
      current_char = 0;
      char_touched = false;
    } else {
      current_char = static_cast<uchar>(*it << right_shift & 0xFFU);
      char_touched = true;
    }
  }
  if (char_touched) {
    t_res.push_back(current_char);
  }
  return t_res;
}

vuchar pack_16(const vuint16::const_iterator t_input_begin,
               const vuint16::const_iterator t_input_end, vuchar &t_res) {
  std::for_each(t_input_begin, t_input_end, [&t_res](const auto value) {
    t_res.push_back(static_cast<uchar>(value >> 8 & 0xFFU));
    t_res.push_back(static_cast<uchar>(value & 0xFFU));
  });
  return t_res;
}

///////////////////////////////////////////////////////////////////////////////
//                                 unpacking                                 //
///////////////////////////////////////////////////////////////////////////////

vuint16 unpack(ustring &&t_input) {
  vuint16 vec{};
  return unpack_n(t_input.begin(), t_input.end(), vec, 9);
}

vuint16 unpack_n(const ustring::const_iterator t_begin,
                 const ustring::const_iterator t_end, vuint16 &t_res, int t_n) {
  if (t_n == 16) {
    return unpack_16(t_begin, t_end, t_res);
  }
  int step = t_n % 8;
  int left_shift = 0;
  int right_shift = 0;
  const int max_value = max(t_n);
  for (auto it = t_begin; it < t_end - 1; /* nope */) {
    uint16_t current_char = 0;
    // left bits
    if ((left_shift += step) >= t_n) {
      left_shift = (left_shift - t_n) + step;
    }
    current_char = static_cast<uint16_t>(*it << left_shift) & masks[t_n];
    // right bits
    bool zero_rs = (right_shift == 0);
    right_shift -= step;
    if (right_shift < 0) {
      // if previous right shift was negative and not zero
      if (!zero_rs) {
        current_char |= *++it << (-right_shift) & masks[16 + right_shift];
      }
      right_shift = 8 + right_shift;
    }
    current_char |= *++it >> right_shift & masks[8 - right_shift];
    // char made!
    if (current_char >= max_value) { // if it is the mask
      return unpack_n(it + 1, t_end, t_res, t_n + 1);
    }
    current_char &= masks[t_n];
    t_res.push_back(current_char);
    if (right_shift == 0) {
      ++it;
    }
  }
  return t_res;
}

vuint16 unpack_16(const ustring::const_iterator t_begin,
                  const ustring::const_iterator t_end, vuint16 &t_res) {
  for (auto it = t_begin; it < t_end; ++it) {
    t_res.push_back(static_cast<uint16_t>(*it << 8 | *++it));
  }
  return t_res;
}
