/**
 *   \file io.cc
 *   \brief Body for file reading and writing
 */

#include "io.hh"
#include <array>

#ifdef Debug
constexpr bool debug_mode = true;
#include <algorithm>
#else
constexpr bool debug_mode = false;
#endif

using std::vector;
using std::uint32_t;
using vuint32 = vector<uint32_t>;
using vvuint32 = vector<vuint32>;



constexpr unsigned char char_size = 12;

/**
 *  Écrit dans le fichier \p t_out les chunks passés en paramètre. Le fichier de
 *  sortie est composé des éléments suivants :\n
 *  - Sur quatre octets sont écrit un `uint32_t` déterminant la taille d'un
 *    caractère\n
 *  - Sur quatre octets sont écrit un `uint32_t` déterminant le nombre de chunk
 *    composant le fichier\n
 *  - Sont ensuite écrits les chunks sur des nombres d’octets variable suivant
 *    la taille d’un caractère et le nombre de caractères\n
 *  \n
 *  Un chunk est composé de la manière qui suit :\n
 *  - Sur quatre octets sont écrit un `uint32_t` déterminant le nombre de
 *    caractères composant le chunk\n
 *  - Les caractères composant le chunk, accolés les uns au autres. Si le
 *    dernier caractère ne remplis pas le dernier octet du chunk, alors ces
 *    derniers bits seront initialisés à 0.\n
 *  La taille d’un chunk est donc la taille d’un caractère multiplié par le
 *  nombre de caractères du chunk, le tout divisé par 8. Si le résultat n’est
 *  pas un entier, alors il est nivelé vers le haut pour avoir un nombre entier
 *  d’octets (e.g. si le résultat est 103.4, alors 104 octets seront utilisés).
 *
 *  \param[out] t_out Fichier de sortie
 *  \param[in] t_text Collection ordonnée des chunks à écrire dans \p t_out
 */
void write_file(FILE *const t_out, const vvuint32 &t_text) {
  const auto size = static_cast<uint32_t>(t_text.size());
  if constexpr (debug_mode) {
    std::printf("Char size: %u\n", char_size);
    std::printf("Number of chunks: %u\n", size);
  }
  fwrite(&char_size, sizeof(char_size), 1, t_out);
  fwrite(&size, sizeof(size), 1, t_out);
  for (const auto &chunk : t_text) {
    if constexpr (debug_mode) {
      std::printf("Size of chunk: %zu\n", chunk.size());
    }
    write_chunk(t_out, chunk);
  }
}

/**
 *  \param t_out Output file
 *  \param t_chunk Chunk to be written to \p t_out
 */
void write_chunk(FILE *const t_out, const vuint32 &t_chunk) {
  const auto chunk_size = static_cast<uint32_t>(t_chunk.size());
  fwrite(&chunk_size, sizeof(chunk_size), 1, t_out);
  std::array<unsigned char, 3> data = {0, 0, 0};
  for (size_t i = 0; i < t_chunk.size(); ++i) {
    if (i % 2 == 0) {
      data[0] = static_cast<unsigned char>(t_chunk[i] >> 4);
      data[1] = static_cast<unsigned char>(t_chunk[i] << 4);
    } else {
      data[1] |= static_cast<unsigned char>(t_chunk[i] >> 8) & 0xC;
      data[2] = static_cast<unsigned char>(t_chunk[i]);
      fwrite(data.data(), sizeof(data[0]), 3, t_out);
    }
  }
  if (t_chunk.size() % 2 != 0) {
    fwrite(data.data(), sizeof(data[0]), 3, t_out);
  }
}
