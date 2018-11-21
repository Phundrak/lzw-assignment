#include "compress.hh"
#include "uncompress.hh"
#include <cassert>
#include <getopt.h>
#include <tuple>

using std::printf;
using std::puts;
using std::string;
using std::tuple;

// custom types ///////////////////////////////////////////////////////////////

using dic_t = std::map<std::pair<uint32_t, uint8_t>, uint32_t>;
using ustring = std::basic_string<uint8_t>; // chaine non encodée
using uvec = std::vector<uint32_t>;         // chaine encodée

void help() {
  puts("Usage:\n\
lzw  [-options] [-i path] [-o path]\n\n\
The default action is to compress the input file to a .lzw file\n\
in which the directory in which the software is executed.\n\
Options available:\n\
-h --help\n\
\tdisplay the current message\n\
-i --input\n\
\tpath to the input file (MANDATORY)\n\
-o --output\n\
\tpath to the output file (if the file already exists, it will be\n\n\
\toverwritten). Default: input path + \".lzw\\n\
-c --compress\n\
\tcompress the input file\n\
-u --uncompress\n\
\tuncompresses the input file to the output file. If no output path\n\
\thas not been entered and if the input file ends with \".lzw\",\n\
\tthe extension \".lzw\" will be removed; otherwise, the extension\n\
\t\"_uncompresed\" will be added");
}

std::tuple<string, string, bool> process_args(int t_argc, char *t_argv[]) {
  auto ret = std::make_tuple(string{}, string{}, true);
  while (true) {
    int option_index = 0;
    static struct option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"input", required_argument, nullptr, 'i'},
        {"output", required_argument, nullptr, 'o'},
        {"compress", no_argument, nullptr, 'c'},
        {"uncompress", no_argument, nullptr, 'u'},
        {nullptr, 0, nullptr, 0}};
    int c = getopt_long(t_argc, t_argv, "hi:o:cu", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 0:
      break;
    case 'h':
      help();
      exit(0);
    case 'i':
      std::get<0>(ret) = optarg;
      break;
    case 'o':
      std::get<1>(ret) = optarg;
      break;
    case 'c':
      std::get<2>(ret) = true;
      break;
    case 'u':
      std::get<2>(ret) = false;
      break;
    case '?':
      [[fallthrough]];
    default:
      puts("Error: unknown parameter.");
      help();
      exit(1);
    }
  }
  return ret;
}

int main(int argc, char *argv[]) {
  const auto [input_path, output_path, compressing] = process_args(argc, argv);
  if(input_path.empty()) {
    help();
    return 0;
  }
  if (compressing) {
    compress(input_path, (output_path.empty()) ? nullptr : output_path.c_str());
  } else {
    uncompress(input_path,
               (output_path.empty()) ? nullptr : output_path.c_str());
  }
  return 0;
}
