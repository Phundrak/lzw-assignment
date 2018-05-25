/**
 *   \file compress.cc
 *   \brief Implementation of compression
 */

#include "compress.hh"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include "io.hh"
using std::vector;
using std::uint8_t;
using std::uint32_t;
using std::string;
using vuint32 = vector<uint32_t>;
using vvuint32 = vector<vuint32>;
using std::string;

using dict_t = std::map<std::pair<uint32_t, uint8_t>, uint32_t>;
using ustring = std::basic_string<uint8_t>; // chaîne non encodée
using uvec = std::vector<std::uint32_t>;         // chaîne encodée
using std::printf;

constexpr int ipow(int base, int exp) {
  int result = 1;
  for (;;) {
    if (exp & 1) {
      result *= base;
    }
    exp >>= 1;
    if (exp == 0) {
      break;
    }
    base *= base;
  }
  return result;
}

constexpr size_t DICT_MAX = ipow(2, 13) - 256; /* 12 bits */

/**
 *  La chaîne de caractère \p t_text est lue caractère par caractère, et est et
 *  selon la valeur de retour de la fonction \ref dico (permettant dans le même
 *  temps la création du dictionnaire), on rajoute un mot ou pas dans le vecteur
 *  de caractères UTF-8 représentant des mots de chars compressés.
 *
 *  \param t_text Chaîne de caractères uint8_t représentant le fichier d'entrée
 *  \return Vecteur de chunks (vecteurs de uint32_t)
 */
vvuint32 lzw_compress(string &&t_text) {
  std::puts("Compressing...");
  uint32_t w = 0xFFFF;
  vuint32 chunk{};
  vvuint32 res{};
  dict_t dict{};

  for(const auto c : t_text) {
    if(dict.size() >= DICT_MAX) {
      // Dictionary full -> chunk pushed, dict emptied
      res.push_back(std::move(chunk));
      chunk = vuint32{};
      dict = dict_t{};
      w = 0xFFFF;
    }
    if (const auto &[yes, pos] = dico(dict, w, static_cast<uint8_t>(c)); yes) {
      w = pos;
    } else {
      chunk.push_back(static_cast<uint32_t>(w));
      w = static_cast<uint32_t>(c);
    }
  }
  if(w != 0xFFFF) {
    chunk.push_back(w);
    res.push_back(std::move(chunk));
  }

  return res;
}

/**
 *  Wrapper de la fonction \ref lzw_compress gérant l'ouverture, la lecture,
 *  l'écriture et la fermeture des fichiers d’entrée et de sortie. Si \p
 *  t_out_file est nul (chemin non spécifié), il prendra alors la valeur de
 *  \p t_in_file à laquelle sera annexé l’extension `.lzw`.
 *
 *  \param[in] t_in_file Chemin vers le fichier d’entrée
 *  \param[in] t_out_file Chemin vers le fichier de sortie
 */
void compress(const std::string &t_in_file, const char *t_out_file) {
  std::ifstream input_file{t_in_file};
  assert(input_file.is_open());
  FILE *const out = (t_out_file != nullptr) ? fopen(t_out_file, "wb") : fopen("output.lzw", "wb");
  if (out == nullptr) {
    std::cerr << "Error at " << __FILE__ << ":" << __LINE__ - 4 << ": could not open output file. Aborting...\n";
    input_file.close();
    exit(1);
  }
  const auto compressed_text{lzw_compress(std::string{std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>()})};
  write_file(out, compressed_text);
  fclose(out);
  input_file.close();
}
