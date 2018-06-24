#include "io.hh"
#include "bitpack.hh"
#include <array>

using std::uint16_t;
using std::vector;
using vuint16 = vector<uint16_t>;
using vvuint16 = vector<vuint16>;

void write_file(std::ofstream &t_out, const vvuint16 &t_chunks) {
  const auto nr_chunks = static_cast<uint16_t>(t_chunks.size());
#ifdef Debug
  std::printf("Number of chunks: %u\n", nr_chunks);
#endif
  t_out.write(reinterpret_cast<const char *>(&nr_chunks), sizeof(nr_chunks));
  for (const auto &chunk : t_chunks) {
    write_chunk(t_out, chunk);
  }
}

void write_chunk(std::ofstream &t_out, const vuint16 &t_chunk) {
  const auto output = pack(t_chunk);
  const auto chunk_size = static_cast<uint32_t>(output.size());
  t_out.write(reinterpret_cast<const char *>(&chunk_size), sizeof(chunk_size));
  t_out.write(reinterpret_cast<const char *>(output.data()),
              sizeof(output[0]) * output.size());
}
