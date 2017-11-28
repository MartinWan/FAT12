#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "diskutils.h"
#include "dir_entry.h"
#include "disk.h"

bool end_cluster_chain(int cluster) {
  int i;
  for (i = 0xFF8; i <= 0xFFF; i++)
    if (cluster == i) return true;
  return false;
}

bool last_cluster(unsigned char* disk, int cluster) {
  int next_cluster = get_FAT_entry(disk, cluster);
  return end_cluster_chain(next_cluster);
}

void copy_directory_entry(unsigned char* disk, unsigned char* entry, FILE* dest) {
  int cluster = dir_entry_cluster(entry);
  
  while (!end_cluster_chain(cluster)) {
    int physical_sector = 33 + cluster - 2;
    int offset = physical_sector * BYTES_PER_SECTOR;

    if (last_cluster(disk, cluster) && (dir_entry_size(entry) % (BYTES_PER_SECTOR)) != 0) {
      // handle case when last sector isn't divisible by 512
      fwrite(disk + offset, sizeof(char), dir_entry_size(entry) % BYTES_PER_SECTOR, dest);
    } else {
      fwrite(disk + offset, sizeof(char), BYTES_PER_SECTOR, dest);
    }
    cluster = get_FAT_entry(disk, cluster);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "usage: diskget <disk image> <disk file>\n");
    return EXIT_FAILURE;
  }
  capitalize(argv[2]);

  int file = open(argv[1], O_RDWR);
  if (file < 0) {
    fprintf(stderr, "Disk file not found.\n");
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
  char buff[BUFF_SIZE];
  for (offset = ROOT_START; offset <= ROOT_END && disk[offset] != 0x00; offset += ROOT_ENTRY_SIZE) {
    dir_entry_display_name(disk + offset, buff);
    if (strcmp(buff, argv[2]) == 0) {
      FILE* dest = fopen(argv[2], "w");
      if (dest == NULL) {
        fprintf(stderr, "fopen error\n");
        return EXIT_FAILURE;
      }
      copy_directory_entry(disk, disk + offset, dest);
      fclose(dest);
      return EXIT_SUCCESS;
    }
  }
  
  fprintf(stderr, "File not found.\n");
  munmap(disk, buf.st_size);
  close(file);

  return EXIT_FAILURE;
}
