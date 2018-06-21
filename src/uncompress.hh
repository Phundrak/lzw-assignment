#ifndef LZW_SRC_UNCOMPRESS_H_
#define LZW_SRC_UNCOMPRESS_H_

#include <memory>
#include <string>
#include <vector>

std::basic_string<unsigned char> lzw_uncompress(std::vector<std::uint16_t> &&);

void uncompress(const std::string &, const char *);

void uncompress_chunk(FILE *, FILE *);

#endif /* LZW_SRC_UNCOMPRESS_H_ */
