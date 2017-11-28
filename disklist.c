#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "diskutils.h"
#include "disk.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: disklist <disk image>\n");
    return EXIT_FAILURE;
  }

  int file = open(argv[1], O_RDWR);
  if (file < 0) {
    fprintf(stderr, "File not found.\n");
    return EXIT_FAILURE;
  }

  struct stat buf;
  fstat(file, &buf);

  unsigned char* disk = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, file, 0);  
  if (disk == MAP_FAILED) {
    fprintf(stderr, "mmap error.\n");
    return EXIT_FAILURE;
  }

  int offset;
  for (offset = ROOT_START; offset <= ROOT_END && disk[offset] != 0x00; offset += ROOT_ENTRY_SIZE) {
    if (dir_entry_visible(disk + offset))
      dir_entry_print(disk + offset);
  }
  
  munmap(disk, buf.st_size);
  close(file);
  
  return EXIT_SUCCESS;
}
