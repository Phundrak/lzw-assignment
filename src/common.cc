/**
 *   \file common.cc
 *   \brief Implementation for functions in common
 */

#include "common.hh"

using std::uint8_t;
using std::uint16_t;
using dic_comp_t = std::map<std::pair<uint16_t, uint8_t>, uint16_t>;
using dic_un_t = std::map<std::uint16_t, std::unique_ptr<std::pair<uint16_t, uint8_t>>>;
using ustring = std::basic_string<unsigned char>;

int ipow(int base, int exp) {
  int result = 1;
  for (;;) {
    if (exp & 1) {
      result *= base;
    }
    exp >>= 1;
    if (exp == 0) {
      break;
    }
    base *= base;
  }
  return result;
}

/**
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
 *  \param t_dictionary Dictionnaire
 *  \param t_nr_chaine Numéro de la chaine précédant le caractères \p t_c dans \p t_dictionary
 *  \param t_c Caractère suivant la chaine de caractères \p t_nr_chaine
 *  \return const std::pair<bool, uint16_t>
 */
std::pair<bool, uint16_t> dico(dic_comp_t &t_dictionary, uint16_t t_nr_chaine,
                               uint8_t t_c) {
  if (t_nr_chaine == 0xFFFF) {
    return std::make_pair(true, t_c);
  }
  auto &e = t_dictionary[std::make_pair(t_nr_chaine, t_c)];
  return (e != 0) ? std::make_pair(true, e)
             : std::make_pair(
                   false,
                   (e = static_cast<
                            typename std::remove_reference<decltype(e)>::type>(
                            t_dictionary.size()) +
                        255));
}
