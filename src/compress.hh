/**
 *   \file compress.hh
 *   \brief Header for compression functions
 */

#ifndef LZW_SRC_COMPRESS_H_
#define LZW_SRC_COMPRESS_H_

#include "common.hh"
#include <vector>
#include <iostream>
#include <thread>
#include <optional>

/// \brief Compression d'une chaine de caractères
std::vector<std::vector<std::uint32_t>> lzw_compress(std::string &&);

std::optional<std::uint32_t>
lzw_compress_char(std::vector<std::vector<std::uint32_t>> &,
                  std::vector<std::uint32_t> &, const char);

/// \brief Wrapper de \ref lzw_compress
void compress(const std::string &, const char *);

#endif /* LZW_SRC_COMPRESS_H_ */
