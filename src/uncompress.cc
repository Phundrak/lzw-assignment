#include "uncompress.hh"
#include "bitpack.hh"
#include "common.hh"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>

using std::fclose;
using std::fopen;
using std::fseek;
using std::string;
using std::uint16_t;
using std::vector;
using ustring = std::basic_string<unsigned char>;
using vuint16 = vector<uint16_t>;

ustring lzw_uncompress(vuint16 &&t_compressed) {
  ustring ret{};
  uint16_t old = 0;
  std::map<uint16_t, ustring> dict{};
  ret.append({static_cast<unsigned char>(t_compressed[0])});
  old = t_compressed[0];
  for (auto it = t_compressed.begin() + 1; it != t_compressed.end(); ++it) {
    const auto uncompressed{dico_uncompress(dict, *it, old)};
    ret.insert(ret.end(), uncompressed.begin(), uncompressed.end());
    old = *it;
  }

  return ret;
}

void uncompress(const string &t_input_name, const char *t_output_name) {
  FILE *const input = std::fopen(t_input_name.c_str(), "rb");
  assert(input);
  std::ofstream output{(t_output_name != nullptr)
                           ? t_output_name
                           : t_input_name + "_uncompressed",
                       std::ios::out | std::ios::binary};
  assert(output.is_open());
  uint16_t nb_chunks = 0;
  std::fread(&nb_chunks, sizeof(nb_chunks), 1, input);
  for (uint16_t i = 0; i < nb_chunks; ++i) {
    uncompress_chunk(input, output);
  }
  output.close();
  std::fclose(input);
}

void uncompress_chunk(FILE *const t_input, std::ofstream &t_output) {
  uint32_t size_chunk = 0;
  fread(&size_chunk, sizeof(size_chunk), 1, t_input);
  auto chunk = std::make_unique<unsigned char[]>(size_chunk);
  fread(chunk.get(), sizeof(unsigned char), size_chunk, t_input);
  auto unpacked = unpack(ustring{chunk.get(), chunk.get() + size_chunk});
  auto uncompressed_chunk = lzw_uncompress(std::move(unpacked));

  // attention here for bug ///////////////////////////////////////////////////
  uncompressed_chunk.push_back(0xFF);
  t_output.write(reinterpret_cast<const char *>(uncompressed_chunk.data()),
                 sizeof(uncompressed_chunk[0]) * uncompressed_chunk.size());
}
