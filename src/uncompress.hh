#ifndef LZW_SRC_UNCOMPRESS_H_
#define LZW_SRC_UNCOMPRESS_H_

#include <string>
#include <vector>

[[nodiscard]] std::basic_string<unsigned char>
lzw_uncompress(std::vector<std::uint16_t> &&);

void uncompress(const std::string &, const char*);

#endif /* LZW_SRC_UNCOMPRESS_H_ */
