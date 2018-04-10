/**
 *   \file compress.hh
 *   \brief Header for compression functions
 */

#ifndef LZW_SRC_COMPRESS_H_
#define LZW_SRC_COMPRESS_H_

#include "common.hh"
#include <vector>

/// \brief Compression d'une chaine de caractères
const std::vector<std::uint32_t>
lzw_compress(const std::basic_string<uint8_t> &t_text,
         std::map<std::pair<std::uint32_t, std::uint8_t>, std::uint32_t>
             &t_dictionary);

/// \brief Wrapper de \ref lzw_compress
void compress(const std::string &t_in_file, const char *t_out_file);

#endif /* LZW_SRC_COMPRESS_H_ */
