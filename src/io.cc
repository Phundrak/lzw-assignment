/**
 *   \file io.cc
 *   \brief Body for file reading and writing
 */

#include "io.hh"
#include "bitpack.hh"
#include <array>
#include <algorithm>

#ifdef Debug
constexpr bool debug_mode = true;
#include <algorithm>
#else
constexpr bool debug_mode = false;
#endif

using std::vector;
using std::uint16_t;
using vuint16 = vector<uint16_t>;
using vvuint16 = vector<vuint16>;

/**
 *  Écrit dans le fichier \p t_out les chunks passés en paramètre. Le fichier de
 *  sortie est composé des éléments suivants :\n
 *  - Sur deux octets sont écrit un `uint16_t` déterminant le nombre de chunk
 *    composant le fichier\n
 *  - Sont ensuite écrits les chunks sur un nombre variable d’octets suivant la
 *    taille des chunks\n
 *  \n
 *  Un chunk est composé de la manière qui suit :\n
 *  - Sur deux octets sont écrit un `uint16_t` déterminant le nombre d’octets
 *    composant le chunk\n
 *  - Sur le nombre d’octets précisés par le header du chunk se trouvent les
 *    données compressées par l’algorithme lzw puis via bit-packing.\n
 *
 *  \param[out] t_out Fichier de sortie
 *  \param[in] t_text Collection ordonnée des chunks à écrire dans \p t_out
 */
void write_file(FILE *const t_out, const vvuint16 &t_text) {
  const auto size = static_cast<uint16_t>(t_text.size());
  if constexpr (debug_mode) {
    std::printf("Number of chunks: %u\n", size);
  }
  fwrite(&size, sizeof(size), 1, t_out);
  for (const auto &chunk : t_text) {
    write_chunk(t_out, chunk);
  }
}

/**
 *  Écrit dans le fichier \p t_out le chunk unique \p t_chunk. Se référer à la
 *  documentation de \ref write_file pour plus de détails.
 *
 *  \param t_out Output file
 *  \param t_chunk Chunk to be written to \p t_out
 */
void write_chunk(FILE *const t_out, const vuint16 &t_chunk) {
  const auto output = pack(t_chunk);
  const auto chunk_size = static_cast<uint16_t>(output.size());
  fwrite(&chunk_size, sizeof(chunk_size), 1, t_out);
  fwrite(output.data(), sizeof(output[0]), output.size(), t_out);
}
