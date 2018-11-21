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

std::pair<bool, uint16_t> dico(dic_comp_t &t_dictionary,
                               const uint16_t t_nr_chaine, const uint8_t t_c) {
  if (t_nr_chaine == 0xFFFF) {
    return std::make_pair(true, t_c);
  }
  auto &e = t_dictionary[std::make_pair(t_nr_chaine, t_c)];
  if(e != 0)
    return std::make_pair(true, e);
  e = static_cast<uint16_t>(t_dictionary.size() + 255);
  return std::make_pair(false, e);
}

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
