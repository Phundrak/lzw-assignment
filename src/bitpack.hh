/**
 *   \file bitpack.hh
 *   \brief Header for bit-packing functions
 */

#ifndef LZW_SRC_BITPACK_H_
#define LZW_SRC_BITPACK_H_

#include <cstdint>
#include <vector>

/// \brief Packs std::uint16_t of n bits into unsigned char
std::vector<unsigned char> pack_n(const std::vector<std::uint16_t>::const_iterator,
                                  const std::vector<std::uint16_t>::const_iterator,
                                  int);

/// \brief Specialization of \ref pack_n for 16bits
std::vector<unsigned char> pack_16(const std::vector<std::uint16_t>::const_iterator,
                                   const std::vector<std::uint16_t>::const_iterator);

#endif /* LZW_SRC_BITPACK_H_ */
