/**
 *   \file compress.hh
 *   \brief Header for compression functions
 */

#ifndef LZW_SRC_COMPRESS_H_
#define LZW_SRC_COMPRESS_H_

#include "common.hh"
#include <vector>
#include <iostream>
#include <thread>

/// \brief Exécution des threads et écriture de leur résultat dans le fichier de sortie
void join_and_write(
    std::vector<std::pair<std::unique_ptr<std::thread>, std::vector<uint32_t>>>
        &,
    FILE *);

/// \brief Compression d'une chaine de caractères
void lzw_compress(const std::vector<char> &, std::vector<std::uint32_t> &);

/// \brief Wrapper de \ref lzw_compress
void compress(const std::string &, const char *);

#endif /* LZW_SRC_COMPRESS_H_ */
