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



constexpr unsigned char char_size = 12;

/**
 *  Écrit dans le fichier \p t_out les chunks passés en paramètre. Le fichier de
 *  sortie est composé des éléments suivants :\n
 *  - Sur deux octets sont écrit un `uint16_t` déterminant la taille d'un
 *    caractère\n
 *  - Sur deux octets sont écrit un `uint16_t` déterminant le nombre de chunk
 *    composant le fichier\n
 *  - Sont ensuite écrits les chunks sur un nombre variable d’octets suivant la
 *    taille des chunks\n
 *  \n
 *  Un chunk est composé de la manière qui suit :\n
 *  - Sur deux octets sont écrit un `uint16_t` déterminant le nombre de
 *    caractères (uint16_t) composant le chunk\n
 *  - Sur n*2 octets la chaîne de caractères compressés (n représentant le
 *    nombre de caractères dans le chunk).\n
 *  Les caractères uint16_t sont à nouveau compressés en 12bits via du
 *  bit-packing, intégrant dont en trois unsigned char deux uint16_t. Le premier
 *  char contient les huit bits de poids fort des douze bits significatifs. Les
 *  quatre bits de poids fort du second char contient les quatre bits de poids
 *  faible du premier uint16_t. Les quatre bits de poids faible du second char
 *  contiennent les quatre bits de poids fort du second uint16_t, et le
 *  troisième char contient les huit bits de poids faible du second uint16_t.\n
 *  Si le nombre de charactères dans le chunk est impair, alors les trois
 *  derniers chars seront remplis comme si le chunk disposait d’un caractère nul
 *  supplémentaire.
 *
 *  \param[out] t_out Fichier de sortie
 *  \param[in] t_text Collection ordonnée des chunks à écrire dans \p t_out
 */
void write_file(FILE *const t_out, const vvuint16 &t_text) {
  const auto size = static_cast<uint16_t>(t_text.size());
  if constexpr (debug_mode) {
    std::printf("Number of chunks: %u\n", size);
  }
  fwrite(&char_size, sizeof(char_size), 1, t_out);
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
  fwrite((const char *)(output.data()), sizeof(output[0]), output.size(),
         t_out);
}
