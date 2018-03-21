#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <streambuf>
#include <string>

#ifdef Debug
#define pdeb printf
#else
#define pdeb                                                                   \
  if (0)                                                                       \
  printf
#endif

using dic_t = std::map<std::pair<int, uint8_t>, int>;
using std::printf;
using std::puts;
using ustring = std::basic_string<unsigned char>;
using c32string = std::basic_string<uint32_t>;

std::pair<bool, int> dico(dic_t &dictionary, int nr_chaine, uint8_t c) {
  if(nr_chaine == -1)
    return std::make_pair(true, c);
  auto &e = dictionary[std::make_pair(nr_chaine, c)];
  return (e) ? std::make_pair(true, e)
             : std::make_pair(
                   false,
                   (e = static_cast<
                            typename std::remove_reference<decltype(e)>::type>(
                            dictionary.size()) +
                        255));
}

const c32string compress(const ustring &text, dic_t &dictionary) {
  c32string res{};
  int w = -1;
  for(const auto &c : text) {
    if (const auto & [ exists, pos ] = dico(dictionary, w, c);
        exists) {
      w = pos;
    } else {
      res.push_back(static_cast<unsigned int>(w));
      w = c;
    }
  }
  return res;
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

  std::ofstream out{"outfile.lzw"};

  // string contenant le fichier d'entrée
  ustring str{};

  // réserver la mémoire dans le string pour le fichier à compresser
  t.seekg(0, std::ios::end);
  str.reserve(static_cast<unsigned long>(t.tellg()));
  t.seekg(0, std::ios::beg);

  // assigner au string la valeur du fichier
  str.assign((std::istreambuf_iterator<char>(t)),
             std::istreambuf_iterator<char>());

  // test pour voir si tout est bien chargé
  printf("Size of str: %zu\n", str.size());
  for (size_t i = 0; i < 64 && i < str.size(); ++i)
    printf("%02x%s", str[i],
           (((i + 1) % 16 == 0) ? "\n" : ((i + 1) % 2 == 0 ? " " : "")));
  printf("\n");

  // le dictionnaire, initialement vide.
  dic_t dictionary{};

  printf("Number of custom words in the dictionary: %zu\n", dictionary.size());

  const auto comp_str{compress(str, dictionary)};

  printf("Compressed, size of compressed string: %zu\n", comp_str.size());
  for(const auto &c : comp_str)
    std::cout << std::hex << c << "\t";
  printf("Number of custom words in the dictionary: %zu\n", dictionary.size());



  for(const auto &c : comp_str)
    out << c;
  t.close();
  out.close();

  return 0;
}
