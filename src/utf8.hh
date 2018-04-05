/**
 *   \file utf8.hh
 *   \brief Header for UTF-8 related functions
 */

#ifndef LZW_SRC_UTF8_H_
#define LZW_SRC_UTF8_H_

#include <cstdio>
#include <cstdint>

/*
  L’encodage des caractères se fait en UTF-8
  char < 128 => "0xxxxxxx" 7bits
  char < 2,048 => "110xxxxx 10xxxxxx" 11bits
  char < 65,536 => "1110xxxx 10xxxxxx 10xxxxxx" 16bits
  char < 2,097,152 => "11110xxx 10xxxxxx 10xxxxxx 10xxxxxx" 21bits
  char < 67,108,864 => "111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx" 26bits
  char < 2,147,483,648 => "1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx" 31bits
*/


/// \brief Écrit les caractères au format UTF-8
void write_utf8(std::FILE* t_out, std::uint32_t t_c);

#endif /* LZW_SRC_UTF8_H_ */
