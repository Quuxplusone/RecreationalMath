#include <cstdio>
#include <cstdlib>
#include "./shared-code.h"

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: ./decompress-oracle oracle.in.txt oracle.out.txt\n");
    exit(1);
  }
  Oracle oracle;
  oracle.read_compressed_from_file(argv[1]);
  oracle.write_to_file(argv[2]);
}
