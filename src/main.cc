/**
 *   \file main.cc
 *   \brief Main file
 *
 *
 *
 */

#include "compress.hh"
#include "getopt.h"
#include <cassert>
#include <tuple>

using std::printf;
using std::puts;
using std::string;
using std::tuple;

// custom types ///////////////////////////////////////////////////////////////

/*
  Dictionnaire :
  <
      <
          numéro chaine précédente,
          caractère ASCII
      >
      numéro chaine courante
  >
 */
using dic_t = std::map<std::pair<uint32_t, uint8_t>, uint32_t>;
using ustring = std::basic_string<uint8_t>; // chaine non encodée
using uvec = std::vector<uint32_t>;         // chaine encodée

/**
 *  \brief Affichage d’aide
 */
void help() {
  puts("Usage:");
  puts("lzw  [-options] [-i path] [-o path]");
  puts("\tThe default action is to compress the input file to a .lzw file");
  puts("\tin which the directory in which the software is executed.");
  puts("\tOptions available:");
  puts("\t-i\tpath to the input file (mandatory)");
  puts("\t-o\tpath to the output file (if the file already exists, it will");
  puts("\t\tbe overwritten). Default: input path + \".lzw\"");
  puts("\t-c\tcompress the input file");
  puts("\t-d\tdecompresses the input file to the output file. If no output");
  puts("\t\tpath has not been entered and if the input file ends with ");
  puts("\t\t\".lzw\", the extension \".lzw\" will be removed; otherwise, the ");
  puts("\t\textension \".uncompresed\" will be added");
}

std::tuple<string, string, bool> process_args(int t_argc, char *t_argv[]) {
  auto ret = std::make_tuple(string{}, string{}, false);
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
    if (c == -1) break;
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

/* TODO: compression multiple : nombre de compressions puis fichier compressé */
int main(int argc, char *argv[]) {
  const auto [input_path, output_path, compressing] = process_args(argc, argv);
  assert(!input_path.empty());
  if (compressing) {
    if (output_path.empty()) {
      compress(input_path, nullptr);
    } else {
      compress(input_path, output_path.c_str());
    }
  } else {
    puts("Not yet implemented :(");
    /* Inversion des types du dictionnaire pour retrouver les chaînes plus aisément */
  }
  return 0;
}
