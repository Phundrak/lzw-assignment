/**
 *   \file common.hh
 *   \brief Header for functions in common
 */

#ifndef LZW_SRC_COMMON_H_
#define LZW_SRC_COMMON_H_

#include <cstdint>
#include <map>

/// \brief Recherche ou ajout de chaine dans le dictionnaire
std::pair<bool, std::uint32_t>
dico(std::map<std::pair<std::uint32_t, std::uint8_t>, std::uint32_t>
         &t_dictionary,
     std::uint32_t t_nr_chaine, std::uint8_t t_c);

#endif /* LZW_SRC_COMMON_H_ */
