#ifndef LZW_SRC_BITPACK_H_
#define LZW_SRC_BITPACK_H_

#include <cstdint>
#include <string>
#include <vector>

std::vector<unsigned char> pack(const std::vector<std::uint16_t> &);

std::vector<unsigned char> pack_n(std::vector<std::uint16_t>::const_iterator,
                                  std::vector<std::uint16_t>::const_iterator,
                                  std::vector<unsigned char> &, int);

std::vector<unsigned char> pack_16(std::vector<std::uint16_t>::const_iterator,
                                   std::vector<std::uint16_t>::const_iterator,
                                   std::vector<unsigned char> &);

std::vector<std::uint16_t> unpack(std::basic_string<unsigned char> &&);

std::vector<std::uint16_t>
unpack_n(std::basic_string<unsigned char>::const_iterator t_begin,
         std::basic_string<unsigned char>::const_iterator t_end,
         std::vector<std::uint16_t> &, int t_n);

std::vector<std::uint16_t>
unpack_16(std::basic_string<unsigned char>::const_iterator,
          std::basic_string<unsigned char>::const_iterator,
          std::vector<std::uint16_t> &);

#endif /* LZW_SRC_BITPACK_H_ */
