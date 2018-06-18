[![CircleCI](https://circleci.com/gh/Phundrak/lzw-assignment/tree/master.svg?style=svg)](https://circleci.com/gh/Phundrak/lzw-assignment/tree/master)
[![Travis Badge](https://travis-ci.org/Phundrak/lzw-assignment.svg?branch=master)](https://travis-ci.org/Phundrak/lzw-assignment)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/80cf9a0514554f368effaf78d8e4ae15)](https://www.codacy.com/app/Phundrak/lzw-assignment?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Phundrak/lzw-assignment&amp;utm_campaign=Badge_Grade)
[![CodeFactor](https://www.codefactor.io/repository/github/phundrak/lzw-assignment/badge)](https://www.codefactor.io/repository/github/phundrak/lzw-assignment)
[![Spacemacs Badge](https://cdn.rawgit.com/syl20bnr/spacemacs/442d025779da2f62fc86c2082703697714db6514/assets/spacemacs-badge.svg)](http://spacemacs.org)

# LZW Compressing tool

This is a university assignment for which I aim to create an LZW algorithm implementation to create a small tool similar to `gzip` and `gunzip` that can compress and uncompress files in a lossless fashion.

This project is written is C++17, compiled with clang under a UNIX environment. Other compilers and environments will not be tested.

## How to use it

Currently, five different options are available to the user:
- `-h` or `--help` will show how to use `projet_lzw`
- `-c` or `--compress` tells `projet_lzw` to compress the input file
- `-u` or `--uncompress` tells `projet_lzw` to uncompress the input file
- `-i <file>` or `--input <file path>` specifies the input file to be compressed or uncompressed (**MANDATORY**)
- `-o <file>` or `--output <file path>` specifies the name of the output file. If not used, the default output name for compression is `output.lzw`, and the default name for uncompressing is `<filename>_uncompressed`.

By default, `projet_lzw` will uncompress the (mandatory) input file.

It is planned to add in the future a sixth option, `-p` or `--passes` that will allow to compress multiple times the input file.
