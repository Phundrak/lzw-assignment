#include "uncompress.hh"
#include "bitpack.hh"
#include "common.hh"
#include <cassert>
#include <cstdio>
#include <cstdlib>

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
  uint16_t v = t_compressed[0];
  ret.append({static_cast<unsigned char>(v)});
  old = v;
  for (auto it = t_compressed.begin() + 1; it != t_compressed.end(); ++it) {
    v = *it;
    const auto uncompressed{dico_uncompress(dict, v, old)};
    ret.insert(ret.end(), uncompressed.begin(), uncompressed.end());
    old = v;
  }

  return ret;
}

void uncompress(const string &t_input_name, const char *t_output_name) {
  FILE *const input = std::fopen(t_input_name.c_str(), "rb");
  assert(input);

  FILE *const output =
      (t_output_name != nullptr)
          ? std::fopen(t_output_name, "wb")
          : std::fopen((t_input_name + "_uncompressed").c_str(), "wb");
  assert(output);

  uint16_t nb_chunks = 0;
  std::fseek(input, 0, SEEK_SET);
  std::fread(&nb_chunks, sizeof(nb_chunks), 1, input);

  for (uint16_t i = 0; i < nb_chunks; ++i) {
    uint32_t size_chunk = 0;
    fread(&size_chunk, sizeof(size_chunk), 1, input);
    auto *chunk = static_cast<unsigned char *>(
        std::malloc(sizeof(unsigned char) * size_chunk));
    fread(chunk, sizeof(unsigned char), size_chunk, input);

    auto unpacked = unpack(ustring{chunk, chunk + size_chunk});
    const auto uncompressed_chunk = lzw_uncompress(std::move(unpacked));
    // sometimes will add null char
    std::fwrite(uncompressed_chunk.data(), sizeof(uncompressed_chunk[0]),
                uncompressed_chunk.size(), output);
  }

  std::fclose(output);
  std::fclose(input);
}
