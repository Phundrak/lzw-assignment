/**
 *   \file io.cc
 *   \brief Body for file reading and writing
 */

#include "io.hh"
#include "bitpack.hh"
#include <array>

using std::uint16_t;
using std::vector;
using vuint16 = vector<uint16_t>;
using vvuint16 = vector<vuint16>;

/**
 *  Écrit dans le fichier \p t_out les chunks passés en paramètre. Le fichier de
 *  sortie est composé des éléments suivants :\n
 *  - Sur deux octets sont écrit un `uint16_t` (deux octets) déterminant le
 *    nombre de chunk composant le fichier\n
 *  - Sont ensuite écrits les chunks sur un nombre variable d’octets suivant la
 *    taille des chunks\n
 *  \n
 *  Un chunk est composé de la manière qui suit :\n
 *  - Sur deux octets sont écrit un `uint32_t` (quatre octets) déterminant le
 *    nombre d’octets composant le chunk\n
 *  - Sur le nombre d’octets précisés par le header du chunk se trouvent les
 *    données compressées par l’algorithme lzw puis via bit-packing.\n
 *
 *  \param[out] t_out Fichier de sortie
 *  \param[in] t_chunks Collection ordonnée des chunks à écrire dans \p t_out
 */
void write_file(std::ofstream &t_out, const vvuint16 &t_chunks) {
  const auto nr_chunks = static_cast<uint16_t>(t_chunks.size());
#ifdef Debug
  std::printf("Number of chunks: %u\n", nr_chunks);
#endif
  t_out.write(reinterpret_cast<const char *>(&nr_chunks), sizeof(nr_chunks));
  for (const auto &chunk : t_chunks) {
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
void write_chunk(std::ofstream &t_out, const vuint16 &t_chunk) {
#ifdef Debug
  std::printf("Chunk!\n");
#endif
  const auto output = pack(t_chunk);
  const auto chunk_size = static_cast<uint32_t>(output.size());
  t_out.write(reinterpret_cast<const char *>(&chunk_size), sizeof(chunk_size));
  t_out.write(reinterpret_cast<const char *>(output.data()),
              sizeof(output[0]) * output.size());
}
