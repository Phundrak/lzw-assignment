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

/// \brief Compression d'une chaine de caractères
std::vector<std::vector<std::uint16_t>> lzw_compress(std::string &&);

/// \brief Wrapper de \ref lzw_compress
void compress(const std::string &, const char *);

#endif /* LZW_SRC_COMPRESS_H_ */
