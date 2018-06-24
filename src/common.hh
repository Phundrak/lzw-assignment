#ifndef LZW_SRC_COMMON_H_
#define LZW_SRC_COMMON_H_

#include <cstdint>
#include <map>
#include <memory>

int ipow(int, int);

std::pair<bool, std::uint16_t>
dico(std::map<std::pair<std::uint16_t, std::uint8_t>, std::uint16_t> &,
     const std::uint16_t, const std::uint8_t);

std::basic_string<unsigned char>
dico_uncompress(std::map<std::uint16_t, std::basic_string<unsigned char>> &,
                const std::uint16_t, const std::uint16_t);

#endif /* LZW_SRC_COMMON_H_ */
