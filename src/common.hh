/**
 *   \file common.hh
 *   \brief Header for functions in common
 */

#ifndef LZW_SRC_COMMON_H_
#define LZW_SRC_COMMON_H_

#include <cstdint>
#include <map>
#include <memory>

int ipow(int, int);

/// \brief Recherche ou ajout de chaine dans le dictionnaire
std::pair<bool, std::uint16_t>
dico(std::map<std::pair<std::uint16_t, std::uint8_t>, std::uint16_t> &,
     std::uint16_t, std::uint8_t);

#endif /* LZW_SRC_COMMON_H_ */
