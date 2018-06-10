/**
 *   \file bitpack.hh
 *   \brief Header for bit-packing functions
 */

#ifndef LZW_SRC_BITPACK_H_
#define LZW_SRC_BITPACK_H_

#include <cstdint>
#include <string>
#include <vector>

/// \brief Bat-packs the input dynamically
[[nodiscard]] std::vector<unsigned char>
pack(const std::vector<std::uint16_t> &);

/// \brief Packs std::uint16_t of n bits into unsigned char
[[nodiscard]] std::vector<unsigned char>
pack_n(const std::vector<std::uint16_t>::const_iterator,
       const std::vector<std::uint16_t>::const_iterator, const int);

/// \brief Specialization of \ref pack_n for 16bits
[[nodiscard]] std::vector<unsigned char>
pack_16(const std::vector<std::uint16_t>::const_iterator,
        const std::vector<std::uint16_t>::const_iterator);

[[nodiscard]] std::vector<std::uint16_t>
unpack(std::basic_string<unsigned char> &&);

[[nodiscard]] std::vector<std::uint16_t>
unpack_n(const std::basic_string<unsigned char>::const_iterator,
         const std::basic_string<unsigned char>::const_iterator, const int t_n);

[[nodiscard]] std::vector<std::uint16_t>
unpack_16(const std::basic_string<unsigned char>::const_iterator,
          const std::basic_string<unsigned char>::const_iterator);

#endif /* LZW_SRC_BITPACK_H_ */
