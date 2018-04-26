/**
 *   \file main.cc
 *   \brief Main file
 *
 *
 *
 */

#ifdef Debug
constexpr bool debug_mode = true;
#else
constexpr bool debug_mode = false;
#endif

#include "compress.hh"
#include "getopt.h"

using std::printf;
using std::puts;

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

int main(int argc, char *argv[]) {

  if constexpr (debug_mode) {
    for (int i = 0; i < argc; ++i)
      printf("argv[%d] = %s\n", i, argv[i]);
  }

  std::string input_path{};
  std::string output_path{};
  bool compressing = true;

  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"input", required_argument, nullptr, 'i'},
        {"output", required_argument, nullptr, 'o'},
        {"compress", no_argument, nullptr, 'c'},
        {"uncompress", no_argument, nullptr, 'u'},
        {0, 0, 0, 0}};
    int c = getopt_long(argc, argv, "hi:o:cu", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 0: {
      if constexpr (debug_mode) {
        printf("\noption %s", long_options[option_index].name);
        if (optarg) {
          printf(" with arg %s\n", optarg);
        }
      }
      break;
    }
    case 'h': {
      if constexpr (debug_mode) {
        printf("From main - option --help passed\n");
      }
      help();
      return 0;
    }
    case 'i': {
      if constexpr (debug_mode) {
        printf("From main - option --input with value '%s'\n", optarg);
      }
      input_path = optarg;
      break;
    }
    case 'o': {
      if constexpr (debug_mode) {
        printf("From main - option --output with value '%s'\n", optarg);
      }
      output_path = optarg;
      break;
    }
    case 'c': {
      if constexpr (debug_mode) {
        printf("From main - option --compress\n");
      }
      compressing = true;
      break;
    }
    case 'u': {
      if constexpr (debug_mode) {
        printf("From main - option --uncompress\n");
      }
      compressing = false;
      break;
    }
    case '?':
    default: {
      puts("Error: unknown parameter.");
      if constexpr (debug_mode) {
        printf("From main - option -?\n");
      }
      help();
      return 1;
    }
    }
  }

  if (input_path == "") {
    puts("Error: no input file specified");
    return 2;
  }

  if (compressing) {
    /*
      TODO:
      - compresser le fichier d’entrée morceaux par morceaux, 16Ko à la fois
      - écrire la taille du segment compressé, puis le segment compressé
      - multithreading
      - compression multiple : nombre de compressions puis fichier compressé
      - bit-packing, limiter la taille du dictionnaire pour un certain nombre de
        bits.
     */
    if constexpr (debug_mode) {
      puts("Beginning compression");
    }
    if(output_path.empty()) {
      compress(input_path, nullptr);
    } else {
      compress(input_path, output_path.c_str());
    }
    // compress(input_path, output_path.c_str());
  } else {
    puts("Not yet implemented :(");
    /*
      Inversion des types du dictionnaire pour retrouver les chaînes plus aisément
     */
  }

  return 0;
}
