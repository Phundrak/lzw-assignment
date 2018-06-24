#ifndef LZW_SRC_IO_H_
#define LZW_SRC_IO_H_

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

void write_file(std::ofstream &,
                const std::vector<std::vector<std::uint16_t>> &);

void write_chunk(std::ofstream &, const std::vector<std::uint16_t> &);

#endif /* LZW_SRC_IO_H_ */
