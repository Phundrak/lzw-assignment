/**
 *   \file utf8.cc
 *   \brief Implementation for UTF-8 related functions
 */

#include "utf8.hh"
#include <string>

using FILE = std::FILE;
using uint8_t = std::uint8_t;
using uint32_t = std::uint32_t;
using ustring = std::basic_string<uint8_t>; // chaine non encodée

/**
 *  Les caractères \c passés en argument sont écrit dans le fichier de sortie au
 *  format UTF-8
 *
 *  \param out Fichier de sortie
 *  \param c Caractères à écrire dans \p out
 */
void write_utf8(FILE* t_out, uint32_t t_c) {
  if(t_c < 128) {
    fwrite(&t_c, sizeof(unsigned char), 1, t_out);
    return;
  }
  size_t loops = 0;
  unsigned char header = 0;
  if (t_c < 2048) {
    loops = 1;
    header = 0xC0;
  } else if (t_c < 65536) {
    loops = 2;
    header = 0xE0;
  } else if (t_c < 2097152) {
    loops = 3;
    header = 0xF0;
  } else if (t_c < 67108864) {
    loops = 4;
    header = 0xF8;
  } else {
    loops = 5;
    header = 0xFC;
  }

  ustring str(loops + 1, 0);
  for (size_t i = 0; i <= loops; ++i) {
    str[i] = static_cast<unsigned char>(
        ((t_c & ((i == loops) ? 0x3F : 0xFF)) >> ((loops - i) * 6)) +
        ((i == 0) ? header : 0x80));
  }
  fwrite(str.data(), sizeof(unsigned char), str.size(), t_out);
}
