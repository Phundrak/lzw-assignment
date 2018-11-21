#include "compress.hh"
#include "common.hh"
#include "io.hh"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iterator>
using std::ios;
using std::string;
using std::uint16_t;
using std::uint8_t;
using std::vector;
using vuint16 = vector<uint16_t>;
using vvuint16 = vector<vuint16>;
using ustring = std::basic_string<unsigned char>;

using dict_t = std::map<std::pair<uint16_t, uint8_t>, uint16_t>;
using std::printf;

ustring read_file(const string &filename) {
  std::ifstream file{filename, ios::binary};
  assert(file);
  file.unsetf(ios::skipws);
  file.seekg(0, ios::end);
  const auto file_size = file.tellg();
  file.seekg(0, ios::beg);
  ustring res{};
  res.reserve(file_size);
  res.insert(res.begin(), std::istream_iterator<unsigned char>(file),
             std::istream_iterator<unsigned char>());
  file.close();
  return res;
}

vvuint16 lzw_compress(ustring &&t_text) {
  vvuint16 res{};
  const auto DICT_MAX = static_cast<size_t>(ipow(2, 14) - 256); /* 16 bits */
  uint16_t w = 0xFFFF;
  vuint16 chunk{};
  dict_t dict{};
  for (const auto c : t_text) {
    if (dict.size() >= DICT_MAX) {
      if (w != 0xFFFF) {
        chunk.push_back(w);
      }
      res.push_back(chunk);
      w = 0xFFFF;
      chunk.clear();
      dict.clear();
    }
    if (const auto &[exists, pos] = dico(dict, w, static_cast<uint8_t>(c));
        exists) {
      w = pos;
    } else {
      chunk.push_back(w);
      w = static_cast<uint16_t>(c);
    }
  }
  if (w != 0xFFFF) {
    chunk.push_back(w);
    res.push_back(std::move(chunk));
  }
  return res;
}

void compress(const std::string &t_in_file, const char *t_out_file) {
  std::ofstream out{(t_out_file != nullptr) ? t_out_file : "output.lzw",
                    ios::out | ios::binary};
  if (!out.is_open()) {
    std::cerr << "Error at " << __FILE__ << ":" << __LINE__ - 4
              << ": could not open output file. Aborting...\n";
    exit(1);
  }
  const auto compressed_text(lzw_compress(read_file(t_in_file)));
  write_file(out, compressed_text);
  out.close();
}
