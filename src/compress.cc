/**
 *   \file compress.cc
 *   \brief Implementation of compression
 */

#ifdef Debug
constexpr bool debug_mode = true;
#else
constexpr bool debug_mode = false;
#endif

#include "compress.hh"
#include "utf8.hh"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <thread>

using dict_t = std::map<std::pair<uint16_t, uint8_t>, uint16_t>;
using ustring = std::basic_string<uint8_t>; // chaine non encodée
using uvec = std::vector<uint16_t>;         // chaine encodée
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
void lzw_compress(const std::vector<char> &t_text, uvec &t_res) {
  dict_t dictionary{};
  std::puts("Compressing...");
  // uvec res{};
  uint16_t w = 0xFFFF;
  uint16_t len = 0;

  constexpr size_t DICT_MAX = 7936; /* 12 bits */

  size_t progress = 0;

  for (const auto &c : t_text) {
    ++len;

    if constexpr (debug_mode) {
      printf("\rprogress: %zu / %zu", ++progress, t_text.size());
    }

    if (/* len > LENGTH_MAX  || */ dictionary.size() >= DICT_MAX) {
      t_res.push_back(static_cast<uint16_t>(w));
      w = static_cast<uint16_t>(c);
      len = 0;
    } else if (const auto &[exists, pos] =
                   dico(dictionary, w, static_cast<std::uint8_t>(c));
               exists) {
      w = pos;
    } else {
      t_res.push_back(static_cast<uint16_t>(w));
      w = static_cast<std::uint8_t>(c);
      len = 0;
    }
  }
  printf("\n");
  // return res;
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
  if(!input_file.is_open()) {
    std::cerr << "Error at " << __FILE__ << ":" << __LINE__ - 2
              << ": could not open output file \"" << t_in_file << "\". Aborting...\n";
    exit(1);
  }

  // Fichier de sortie
  const char *filename =
      (t_out_file) ? t_out_file : "output.lzw";
  FILE *out = fopen(filename, "wb");
  if(!out) {
    std::cerr << "Error at " << __FILE__ << ":" << __LINE__ - 4
              << ": could not open output file \"" << filename << "\". Aborting...\n";
    input_file.close();
    exit(1);
  }

  // input_file.seekg(0, std::ios::end);
  // // string contenant le fichier d’entrée
  // ustring str(static_cast<unsigned long>(input_file.tellg()),
  //             static_cast<unsigned char>(0));
  // input_file.seekg(0, std::ios::beg);

  // // assignation du contenu du fichier à str
  // str.assign((std::istreambuf_iterator<char>(input_file)),
  //            std::istreambuf_iterator<char>());

  // printf("Size of input file: %zu\n", str.size());

  // dict_t dictionary{};

  // const auto comp_str{lzw_compress(str, dictionary)};

  // thread pool
  std::vector<std::pair<std::unique_ptr<std::thread>, uvec>> threads{};

  // char chunk[32768];
  std::vector<char> chunk{};
  chunk.reserve(32768);
  while (input_file.read(chunk.data(), 32768)) {
    threads.push_back(std::make_pair(nullptr, uvec{}));
    threads.back().first = std::make_unique<std::thread>(
        std::thread{lzw_compress, chunk, ref(threads.back().second)});
    if (threads.size() >= 8) {
      for (auto &elem : threads) {
        (*elem.first).join();
      }
      for (auto &elem : threads) {
        for (const auto c : elem.second) {
          write_utf8(out, c);
        }
      }
      threads.clear();
    }
  }

  if(threads.size() != 0) {
    for (auto &elem : threads) {
      (*elem.first).join();
    }
    for (auto &elem : threads) {
      for (const auto c : elem.second) {
        write_utf8(out, c);
      }
    }
    threads.clear();
  }

  if(input_file.tellg() != std::ios::end) {
    std::puts("Leftovers...");
  }

  // for (const auto c : comp_str)
  //   write_utf8(out, c);

  fclose(out);
  input_file.close();
}
