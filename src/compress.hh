#ifndef LZW_SRC_COMPRESS_H_
#define LZW_SRC_COMPRESS_H_

#include "common.hh"
#include <iostream>
#include <thread>
#include <vector>

std::vector<std::vector<std::uint16_t>>
lzw_compress(std::basic_string<unsigned char> &&);

void compress(const std::string &, const char *);

#endif /* LZW_SRC_COMPRESS_H_ */
