/**
 *   \file compress.hh
 *   \brief Header for compression functions
 */

#ifndef LZW_SRC_COMPRESS_H_
#define LZW_SRC_COMPRESS_H_

#include "common.hh"
#include <vector>
#include <iostream>

/// \brief Compression d'une chaine de caractères
void lzw_compress(const std::vector<char> &t_text,
                  std::vector<std::uint16_t> &t_rec);

/// \brief Wrapper de \ref lzw_compress
void compress(const std::string &t_in_file, const char *t_out_file);

#endif /* LZW_SRC_COMPRESS_H_ */
