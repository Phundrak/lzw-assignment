/**
 *   \file compress.cc
 *   \brief Implementation of compression
 */

#include "compress.hh"
#include "io.hh"
#include "common.hh"
#include <cassert>
#include <cstdlib>
#include <fstream>
using std::string;
using std::uint16_t;
using std::uint8_t;
using std::vector;
using vuint16 = vector<uint16_t>;
using vvuint16 = vector<vuint16>;
using ustring = std::basic_string<unsigned char>;

using dict_t = std::map<std::pair<uint16_t, uint8_t>, uint16_t>;
using std::printf;

const size_t DICT_MAX = static_cast<size_t>(ipow(2, 17) - 256); /* 16 bits */

/**
 *  La chaîne de caractères \p t_text est lue caractère par caractère, et est
 * selon la valeur de retour de la fonction \ref dico (permettant dans le même
 * temps la création du dictionnaire), on rajoute ou non un nouveau caractère
 * encodé sur 12bits dans le chunk courant. Dès que le dictionnaire est plein
 * (2^12 caractères), le chunk est sauvegardé et vidé, et le dictionnaire est
 * réinitialisé.
 *
 *  \param t_text Chaîne de caractères uint8_t représentant le fichier d'entrée
 *  \return Vecteur de chunks (vecteurs de uint16_t)
 */
vvuint16 lzw_compress(ustring &&t_text) {
  std::puts("Compressing...");
  uint16_t w = 0xFFFF;
  vuint16 chunk{};
  vvuint16 res{};
  dict_t dict{};
  for (const auto c : t_text) {
    if (dict.size() >= DICT_MAX) {
      res.push_back(std::move(chunk));
      chunk = vuint16{};
      dict = dict_t{};
      w = 0xFFFF;
    }
    if (const auto &[yes, pos] = dico(dict, w, static_cast<uint8_t>(c)); yes) {
      w = pos;
    } else {
      chunk.push_back(w);
      w = static_cast<uint16_t>(c);
    }
  }
  if (w != 0xFFFF) {
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
  FILE *const input_file = fopen(t_in_file.c_str(), "rb");
  assert(input_file);
  FILE *const out = (t_out_file != nullptr) ? fopen(t_out_file, "wb")
                                            : fopen("output.lzw", "wb");
  if (out == nullptr) {
    std::cerr << "Error at " << __FILE__ << ":" << __LINE__ - 4
              << ": could not open output file. Aborting...\n";
    // input_file.close();
    std::fclose(input_file);
    exit(1);
  }

  std::fseek(input_file, 0L, SEEK_END);
  const auto file_size = static_cast<size_t>(ftell(input_file));
  std::rewind(input_file);

  auto raw_text = std::make_unique<unsigned char[]>(file_size);
  std::fread(raw_text.get(), sizeof(unsigned char), file_size, input_file);
  const auto compressed_text(lzw_compress(ustring{raw_text.get(), &raw_text[file_size]}));
  write_file(out, compressed_text);
  fclose(out);
  fclose(input_file);
}
