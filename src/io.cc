/**
 *   \file io.cc
 *   \brief Body for file reading and writing
 */

#include "io.hh"

#ifdef Debug
constexpr bool debug_mode = true;
#else
constexpr bool debug_mode = false;
#endif

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
void write_file(FILE *t_out, std::vector<std::vector<std::uint32_t>> &t_text) {
  {
    uint32_t char_size = 12;
    if constexpr (debug_mode) {
      std::printf("Char size: %u\n", char_size);
    }
    fwrite(&char_size, sizeof(uint32_t), 1, t_out);
    auto size = static_cast<uint32_t>(t_text.size());
    if constexpr (debug_mode) {
      std::printf("Number of chunks: %u\n", size);
    }
    fwrite(&size, sizeof(uint32_t), 1, t_out);
  }
  for(const auto &chunk : t_text) {
    // write size of chunk in uint32_t
    {
      auto size = static_cast<uint32_t>(chunk.size());
      if constexpr (debug_mode) {
        std::printf("Size of chunk: %u\n", size);
      }
      fwrite(&size, sizeof(uint32_t), 1, t_out);
    }
    uint8_t remainder = 0x00;
    for(size_t i = 0; i < chunk.size(); ++i) {
      if(i % 2 == 0) {
        // char = xxxx xxxx xxxx
        //        ^^^^^^^^^ ^^^^
        //          write   keep in remainder as xxxx0000
        auto temp = static_cast<unsigned char>(chunk[i] >> 4);
        fwrite(&temp, sizeof(temp), 1, t_out);
        if constexpr (debug_mode) {
          std::printf("writing: %x\t\t", temp);
        }
        remainder = static_cast<uint8_t>(chunk[i] << 4);
      } else {
        // already have `remainder = yyyy0000`
        //          char = xxxx xxxx xxxx
        //                 ^^^^ ^^^^^^^^^
        // remainder = yyyyxxxx   write after remainder
        // remainder = 00000000
        remainder &= static_cast<unsigned char>(chunk[i]) >> 8 & 0xF0;
        fwrite(&remainder, sizeof(remainder), 1, t_out);
        if constexpr (debug_mode) {
          std::printf("writing remainder: %x\t\t", remainder);
        }
        auto temp = static_cast<unsigned char>(chunk[i]);
        fwrite(&temp, sizeof(temp), 1, t_out);
        if constexpr (debug_mode) {
          std::printf("writing: %x\n", temp);
        }
        remainder = 0x00;
      }
    }
    if(remainder != 0) {
      fwrite(&remainder, sizeof(remainder), 1, t_out);
    }
  }
}
