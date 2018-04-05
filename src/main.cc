/**
 *   \file main.cc
 *   \brief Main file
 *
 *
 *
 */

#include "utf8.hh"
#include "compress.hh"
#include <fstream>
#include <iostream>
#include <streambuf>

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
