/**
 *   \file io.hh
 *   \brief Header for file reading and writing
 */

#ifndef LZW_SRC_IO_H_
#define LZW_SRC_IO_H_

#include <cstdio>
#include <cstdint>
#include <iostream>
#include <vector>

/*
 * Un fichier compressé se compose ainsi :
 * char_size : taille d'un caractère en bits (1B)
 * nb_chunk : nombre de chunks (4B)
 * chunks* : chunks
 *
 * Un chunk se compose ainsi :
 * nb_char_chunk : nombre de caractères du chunk (2B)
 * text* : caractères de taille char_size (ceil((char_size * nb_char_chunk) / 8))
 *
 * Si le dernier caractère ne termine pas le dernier octet du chunk, les
 * derniers bits sont mit à zéro
 */


/// \brief Écrit dans le fichier le texte compressé
void write_file(FILE *const, const std::vector<std::vector<std::uint16_t>> &);

/// \brief Écrit un chunk dans le fichier de sortie
void write_chunk(FILE *const, const std::vector<std::uint16_t> &);

#endif /* LZW_SRC_IO_H_ */
