/**
 *   \file compress.cc
 *   \brief Implementation of compression
 */

#include "compress.hh"
#include "utf8.hh"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "io.hh"

using dict_t = std::map<std::pair<uint32_t, uint8_t>, uint32_t>;
using ustring = std::basic_string<uint8_t>; // chaîne non encodée
using uvec = std::vector<std::uint32_t>;         // chaîne encodée
using std::printf;

constexpr size_t CHUNK_SIZE = 32768;

/**
 *
 *  Reçoit une liste de paires std::thread/vecteurs, le premier étant le
 *  processus dont sa sortie est stockée dans le second. La sortie, une liste
 *  de caractères uint32_t, est écrite dans le fichier de sortie \p out.
 *
 *  \param[in] t_threads
 *  \param[out] t_out
 */
void join_and_write(
    std::vector<std::pair<std::unique_ptr<std::thread>, uvec>> &t_threads,
    std::vector<std::vector<std::uint32_t>> &compressed_text) {
  for (auto &elem : t_threads) {
    (*elem.first).join();
  }
  for (auto &elem : t_threads) {
    compressed_text.push_back(std::move(elem.second));
  }
  t_threads.clear();
}

/**
 *  La chaîne de caractère \p t_text est lue caractère par caractère, et est et
 *  selon la valeur de retour de la fonction \ref dico (permettant dans le même
 *  temps la création du dictionnaire), on rajoute un mot ou pas dans le vecteur
 *  de caractères UTF-8 représentant des mots de chars compressés. La fonction
 *  renvoie ledit vecteur de uint32_t via le paramètre \p t_res.
 *
 *  \param[in] t_text Chaîne de caractères uint8_t représentant le fichier d'entrée
 *  \param[out] t_res Chaîne de caractères de sortie
 */
void lzw_compress(const std::vector<char> &t_text, uvec &t_res) {
  dict_t dictionary{};
  std::puts("Compressing...");
  uint32_t w = 0xFFFF;

  constexpr size_t DICT_MAX = 7936; /* 12 bits */

  for (const auto &c : t_text) {
    if (dictionary.size() >= DICT_MAX) {
      t_res.push_back(static_cast<uint32_t>(w));
      w = static_cast<uint32_t>(c);
    } else if (const auto &[exists, pos] =
                   dico(dictionary, w, static_cast<std::uint8_t>(c));
               exists) {
      w = pos;
    } else {
      t_res.push_back(static_cast<uint32_t>(w));
      w = static_cast<std::uint8_t>(c);
    }
  }
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
  // Fichier d’entrée
  std::ifstream input_file{t_in_file};
  if (!input_file.is_open()) {
    std::cerr << "Error at " << __FILE__ << ":" << __LINE__ - 2
              << ": could not open output file \"" << t_in_file
              << "\". Aborting...\n";
    exit(1);
  }

  // Fichier de sortie
  FILE *out =
      (t_out_file) ? fopen(t_out_file, "wb") : fopen("output.lzw", "wb");
  if (!out) {
    std::cerr << "Error at " << __FILE__ << ":" << __LINE__ - 4
              << ": could not open output file. Aborting...\n";
    input_file.close();
    exit(1);
  }

  // collection of chunks
  std::vector<std::vector<std::uint32_t>> compressed_text{};

  // thread pool
  std::vector<std::pair<std::unique_ptr<std::thread>, uvec>> threads{};

  // chunk chars
  std::vector<char> chunk(CHUNK_SIZE, 0);
  while (input_file.read(chunk.data(),
                         static_cast<std::streamsize>(chunk.size()))) {
    threads.emplace_back(nullptr, uvec{});
    threads.back().second.reserve(CHUNK_SIZE);
    threads.back().first = std::make_unique<std::thread>(
        std::thread{lzw_compress, chunk, ref(threads.back().second)});
    assert(threads.back().first);
    if (threads.size() >= 8) {
      join_and_write(threads, compressed_text);
    }
  }

  if (!threads.empty()) {
    join_and_write(threads, compressed_text);
  }

  if (input_file.tellg() != std::ios::end) {
    std::puts("Leftovers, compressing...");
    {
      const auto prev_pos = input_file.tellg();
      input_file.seekg(0, std::ios::end);
      chunk.reserve(static_cast<size_t>(input_file.tellg() - prev_pos));
      input_file.seekg(prev_pos, std::ios::beg);
      std::istreambuf_iterator<char> itr(input_file);
      for (std::streamoff i = 0; i < prev_pos; ++i, ++itr)
        ;
      chunk.assign((itr), std::istreambuf_iterator<char>());
    }
    uvec ret{};
    lzw_compress(chunk, ret);
    compressed_text.push_back(std::move(ret));
  }

  write_file(out, compressed_text);

  fclose(out);
  input_file.close();
}
