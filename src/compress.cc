/**
 *   \file compress.cc
 *   \brief Implementation of compression
 */

#include "compress.hh"
#include <cstdlib>

using dict_t = std::map<std::pair<uint32_t, uint8_t>, uint32_t>;
using ustring = std::basic_string<uint8_t>; // chaine non encodée
using uvec = std::vector<uint32_t>; // chaine encodée

/**
 *  La chaine de caractère \p text est lue caractère par caractère, et est et
 * selon la valeur de retour de la fonction \ref dico (permettant dans le même
 * temps la création du dictionnaire), on rajoute un mot ou pas dans le vecteur
 * de caractères UTF-8 représentant des mots de chars compressés. La fonction
 * renvoie ledit vecteur de uint32_t.
 *
 *  \param t_text Chaine de caractères uint8_t représentant le fichier d'entrée
 *  \param t_dictionary Dictionnaire de compression
 *  \return std::vector<uint16_t>
 */
const uvec compress(const ustring &t_text, dict_t &t_dictionary) {
  std::puts("Compressing...");
  uvec res{};
  uint32_t w = 0xFFFFFFFF;
  uint32_t len = 0;
#ifdef Debug
  size_t progress = 0;
#endif
  for (const auto &c : t_text) {
    ++len;
#ifdef Debug
    printf("\rprogress: %zu / %zu", progress++, t_text.size());
#endif
    if (len > 9) {
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
