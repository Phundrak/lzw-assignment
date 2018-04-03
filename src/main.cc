#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <streambuf>
#include <string>
#include <vector>

#ifdef Debug
#define pdeb printf
#else
#define pdeb                                                                   \
  if (0)                                                                       \
  printf
#endif

using std::printf;
using std::puts;

// custom types ///////////////////////////////////////////////////////////////

/*
  Dictionnaire :
  <
      <
          numéro chaine précédente,
          caractère ASCII
      >
      numéro chaine courante
  >
 */
using dic_t = std::map<std::pair<uint32_t, uint8_t>, uint32_t>;
using ustring = std::basic_string<uint8_t>; // chaine non encodée
using uvec = std::vector<uint32_t>; // chaine encodée

size_t size_str(dic_t &dict, uint32_t nr_chaine) {
  for(const auto &[key, value] : dict) {
    if(value == nr_chaine) {
      if(std::get<0>(key) == 0xFFFFFFFF)
        return 1;
      return 1 + size_str(dict, std::get<0>(key));
    }
  }
  return 0;
}

/**
 *  \brief Recherche ou ajout de chaine dans le dictionnaire
 *
 *  Cette fonction a pour double usage la recherche d’une chaine de caractères
 *  dans le dictionnaire, ou bien l’ajout d’une nouvelle chaîne si celle-ci n’est
 *  pas déjà présente. Une chaine de caractères est représentée par un couple
 *  numéro de chaine / caractère, le numéro de chaine renvoyant au caractère
 *  précédent (soit son code ASCII, soit son indice dans le dictionnaire) et le
 *  caractère se référant au dernier caractère de la chaine courante. Si le
 *  numéro de chaine est -1, alors il s’agit du premier caractère de la chaine,
 *  et la valeur renvoyée sera la valeur ASCII du caractère. La fonction renvoie
 *  une paire bool/uint16_t, la valeur booléene indiquant si une nouvelle fut
 *  ajoutée dans le dictionnaire ou non, et le uint16_t indiquant la valeur
 *  numérique de la chaîne dans le dictionnaire.
 *
 *  \param dictionary Dictionnaire
 *  \param nr_chaine Numéro de la chaine précédant le caractères \p c dans \p dictionary
 *  \param c Caractère suivant la chaine de caractères \p nr_chaine
 *  \return std::pair<bool, uint16_t>
 */
std::pair<bool, uint32_t> dico(dic_t &dictionary, uint32_t nr_chaine, uint8_t c) {
  if (nr_chaine == 0xFFFFFFFF)
    return std::make_pair(true, c);
  auto &e = dictionary[std::make_pair(nr_chaine, c)];
  return (e) ? std::make_pair(true, e)
             : std::make_pair(
                   false,
                   (e = static_cast<
                            typename std::remove_reference<decltype(e)>::type>(
                            dictionary.size()) +
                        255));
  // auto &e = dictionary[std::make_tuple(nr_chaine, c, len)];
  // return (e) ? std::make_pair(true, e)
  //            : std::make_pair(
  //                  false,
  //                  (e = static_cast<
  //                           typename std::remove_reference<decltype(e)>::type>(
  //                           dictionary.size()) +
  //                       255));
}

/**
 *  \brief Compression d'une chaine de caractères
 *
 *  La chaine de caractère \p text est lue caractère par caractère, et est et
 * selon la valeur de retour de la fonction \ref dico (permettant dans le même
 * temps la création du dictionnaire), on rajoute un mot ou pas dans le vecteur
 * de caractères UTF-8 représentant des mots de chars compressés. La fonction
 * renvoie ledit vecteur de uint32_t.
 *
 *  \param text Chaine de caractères uint8_t représentant le fichier d'entrée
 *  \param dictionary Dictionnaire de compression
 *  \return std::vector<uint16_t>
 */
const uvec compress(const ustring &text, dic_t &dictionary) {
  std::puts("Compressing...");
  uvec res{};
  uint32_t w = 0xFFFFFFFF;
  uint32_t len = 0;
#ifdef Debug
  size_t progress = 0;
#endif
  for (const auto &c : text) {
    ++len;
#ifdef Debug
    printf("\rprogress: %zu / %zu", progress++, text.size());
#endif
    if (len > 9) {
      res.push_back(static_cast<uint32_t>(w));
      w = c;
      len = 0;
    } else if (const auto &[exists, pos] = dico(dictionary, w, c);
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

/*
  L’encodage des caractères se fait en UTF-8
  char < 128 => "0xxxxxxx" 7bits
  char < 2,048 => "110xxxxx 10xxxxxx" 11bits
  char < 65,536 => "1110xxxx 10xxxxxx 10xxxxxx" 16bits
  char < 2,097,152 => "11110xxx 10xxxxxx 10xxxxxx 10xxxxxx" 21bits
  char < 67,108,864 => "111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx" 26bits
  char < 2,147,483,648 => "1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx" 31bits
*/

/**
 *  \brief Écrit les caractères au format UTF-8
 *
 *  Les caractères \c passés en argument sont écrit dans le fichier de sortie au
 *  format UTF-8
 *
 *  \param out Fichier de sortie
 *  \param c Caractères à écrire dans \p out
 */
void write_utf8(FILE* out, uint32_t c) {
  if(c < 128) {
    fwrite(&c, sizeof(unsigned char), 1, out);
    return;
  }
  size_t loops = 0;
  unsigned char header = 0;
  if (c < 2048) {
    loops = 1;
    header = 0xC0;
  } else if (c < 65536) {
    loops = 2;
    header = 0xE0;
  } else if (c < 2097152) {
    loops = 3;
    header = 0xF0;
  } else if (c < 67108864) {
    loops = 4;
    header = 0xF8;
  } else {
    loops = 5;
    header = 0xFC;
  }

  ustring str(loops + 1, 0);
  for (size_t i = 0; i <= loops; ++i) {
    str[i] = static_cast<unsigned char>(
        ((c & (i == loops) ? 0x3F : 0xFF) >> ((loops - i) * 6)) +
        ((i == 0) ? header : 0x80));
  }
  fwrite(str.data(), sizeof(unsigned char), str.size(), out);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    puts("Usage: lzw <filename>");
    return 1;
  } else {
    for (int i = 0; i < argc; ++i)
      printf("argv[%d] = %s\n", i, argv[i]);
  }

  // fichier d'entrée
  std::ifstream t{argv[1]};

  // fichier de sortie
  FILE *out = fopen("output.lzw", "wb");

  // string contenant le fichier d'entrée
  ustring str{};

  // réserver la mémoire dans le string pour le fichier à compresser
  t.seekg(0, std::ios::end);
  str.reserve(static_cast<unsigned long>(t.tellg()));
  t.seekg(0, std::ios::beg);

  // assigner au string les caractères du fichier
  str.assign((std::istreambuf_iterator<char>(t)),
             std::istreambuf_iterator<char>());

  // test pour voir si tout est bien chargé
  printf("Size of str: %zu\n", str.size());

  // le dictionnaire, initialement vide.
  dic_t dictionary{};

  // compresssion du texte
  const auto comp_str{compress(str, dictionary)};

  printf("Compressed, size of compressed string: %zu\n", comp_str.size());
  printf("Compression ratio: %.10f\n",
         static_cast<double>(str.size()) / static_cast<double>(comp_str.size()));

  printf("Number of custom words in the dictionary: %zu\n", dictionary.size());

  for(const auto c : comp_str)
    write_utf8(out, c);

  fclose(out);
  t.close();

  return 0;
}
