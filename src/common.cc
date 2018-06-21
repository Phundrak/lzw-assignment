/**
 *   \file common.cc
 *   \brief Implementation for functions in common
 */

#include "common.hh"

using std::uint16_t;
using std::uint8_t;
using dic_comp_t = std::map<std::pair<uint16_t, uint8_t>, uint16_t>;
using ustring = std::basic_string<unsigned char>;
using p_ustring = std::shared_ptr<ustring>;

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
 *  dans le dictionnaire, ou bien l’ajout d’une nouvelle chaîne si celle-ci
 * n’est pas déjà présente. Une chaine de caractères est représentée par un
 * couple numéro de chaine / caractère, le numéro de chaine renvoyant au
 * caractère précédent (soit son code ASCII, soit son indice dans le
 * dictionnaire) et le caractère se référant au dernier caractère de la chaine
 * courante. Si le numéro de chaine est -1, alors il s’agit du premier caractère
 * de la chaine, et la valeur renvoyée sera la valeur ASCII du caractère. La
 * fonction renvoie une paire bool/uint16_t, la valeur booléene indiquant si une
 * nouvelle fut ajoutée dans le dictionnaire ou non, et le uint16_t indiquant la
 * valeur numérique de la chaîne dans le dictionnaire.
 *
 *  \param t_dictionary Dictionnaire
 *  \param t_nr_chaine Numéro de la chaine précédant le caractères \p t_c dans
 * \p t_dictionary \param t_c Caractère suivant la chaine de caractères \p
 * t_nr_chaine \return const std::pair<bool, uint16_t>
 */
std::pair<bool, uint16_t> dico(dic_comp_t &t_dictionary,
                               const uint16_t t_nr_chaine, const uint8_t t_c) {
  if (t_nr_chaine == 0xFFFF) {
    return std::make_pair(true, t_c);
  }
  auto &e = t_dictionary[std::make_pair(t_nr_chaine, t_c)];
  return (e != 0) ? std::make_pair(true, e)
                  : std::make_pair(false, (e = static_cast<uint16_t>(
                                               t_dictionary.size() + 255)));
}

/**
 *  Detailed description
 *
 *  \param t_dict Dictionnaire
 *  \return Retourne une chaîne de caractères non signés
 */
ustring dico_uncompress(std::map<uint16_t, ustring> &t_dict,
                        const uint16_t t_code, const uint16_t t_old) {
  // le code existe dans le dictionnaire s’il est < 256
  if (t_code < 256) {
    ustring e{static_cast<unsigned char>(t_code)};
    // 256 car on n'a pas encore tenté d'insérer de nouveau caractère
    if (t_old < 256) {
      t_dict[static_cast<uint16_t>(t_dict.size() + 256)] =
          static_cast<unsigned char>(t_old) + e;
    } else {
      t_dict[static_cast<uint16_t>(t_dict.size() + 256)] = t_dict[t_old] + e;
    }
    return e;
  }

  auto &e = t_dict[t_code];
  auto str = (t_old < 256) ? ustring{static_cast<unsigned char>(t_old)}
                           : t_dict[t_old];

  // le code existe dans le dictionnaire
  if (!e.empty()) {
    str += e[0];
    const auto index = static_cast<uint16_t>(t_dict.size() + 256);
    t_dict[index] = str;
    return e;
  }

  // le code n'existe pas encore dans le dictionnaire
  str += str[0];
  e = str;
  t_dict[t_code] = e;
  return e;
}
