/**
 *   \file compress.cc
 *   \brief Implementation of compression
 */

#include "compress.hh"
#include "utf8.hh"
#include <cstdlib>
#include <fstream>

using dict_t = std::map<std::pair<uint32_t, uint8_t>, uint32_t>;
using ustring = std::basic_string<uint8_t>; // chaine non encodée
using uvec = std::vector<uint32_t>; // chaine encodée
using std::printf;

/**
 *  La chaine de caractère \p text est lue caractère par caractère, et est et
 *  selon la valeur de retour de la fonction \ref dico (permettant dans le même
 *  temps la création du dictionnaire), on rajoute un mot ou pas dans le vecteur
 *  de caractères UTF-8 représentant des mots de chars compressés. La fonction
 *  renvoie ledit vecteur de uint32_t.
 *
 *  \param t_text Chaine de caractères uint8_t représentant le fichier d'entrée
 *  \param t_dictionary Dictionnaire de compression
 *  \return std::vector<uint16_t>
 */
const uvec lzw_compress(const ustring &t_text, dict_t &t_dictionary) {
  std::puts("Compressing...");
  uvec res{};
  uint32_t w = 0xFFFFFFFF;
  uint32_t len = 0;

  constexpr size_t DICT_MAX = 7936; /* 12 bits */

#ifdef Debug
  size_t progress = 0;
#endif

  for (const auto &c : t_text) {
    ++len;

#ifdef Debug
    printf("\rprogress: %zu / %zu", ++progress, t_text.size());
#endif

    if (/* len > LENGTH_MAX  || */ t_dictionary.size() >= DICT_MAX  ) {
      res.push_back(static_cast<uint32_t>(w));
      w = c;
      len = 0;
      } else if (const auto &[exists, pos] = dico(t_dictionary, w, c);
               exists) {
      w = pos;
    } else {
      res.push_back(static_cast<uint32_t>(w));
      w = c;
      len = 0;
    }
  }
  printf("\n");
  return res;
}

/**
 *  \brief function description
 *
 *  Wrapper de la fonction \ref lzw_compress gérant l'ouverture, la lecture,
 *  l'écriture et la fermeture des fichiers d’entrée et de sortie. Si \p
 *  t_out_file est nul (chemin non spécifié), il prendra alors la valeur de
 *  \p t_in_file à laquelle sera annexé l’extension `.lzw`.
 *
 *  \param t_in_file Chemin vers le fichier d’entrée
 *  \param t_out_file Chemin vers le fichier de sortie
 */
void compress(const std::string &t_in_file, const char *t_out_file) {
  // Fichier d’entrée
  std::ifstream input_file{t_in_file};

  // Fichier de sortie
  // FILE *out = nullptr;
  // if (t_out_file) {
  //   out = fopen(t_out_file, "wb");
  // } else {
  //   std::string out_name{t_in_file};
  //   out_name.append(".lzw");
  //   out = fopen(out_name.c_str(), "wb");
  // }
  FILE *out = (t_out_file)
                  ? fopen(t_out_file, "wb")
                  : fopen(std::string{t_out_file, ".lzw"}.c_str(), "wb");

  input_file.seekg(0, std::ios::end);
  // string contenant le fichier d’entrée
  ustring str(static_cast<unsigned long>(input_file.tellg()),
              static_cast<unsigned char>(0));
  input_file.seekg(0, std::ios::beg);

  // assignation du contenu du fichier à str
  str.assign((std::istreambuf_iterator<char>(input_file)),
             std::istreambuf_iterator<char>());

  printf("Size of input file: %zu\n", str.size());

  dict_t dictionary{};

  const auto comp_str{lzw_compress(str, dictionary)};

  printf("\n############################################\n");
  printf("                Compressed!\n");
  printf("############################################\n\n");
  printf("Size of compressed string: %zu\n", comp_str.size());
  printf("Size of dictionary: %zu\n", dictionary.size());
  printf("Compression ratio: %.10f\n",
         static_cast<double>(str.size() / comp_str.size()));

  for(const auto c : comp_str)
    write_utf8(out, c);

  fclose(out);
  input_file.close();

  return;
}
