#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "disk.h"
#include "diskutils.h"
#include "dir_entry.h"

#define ELEVEN_SPACES "           "

void disk_label(unsigned char* disk, char* dest) {
  memcpy(dest, disk + 43, 11);
  dest[11] = '\0';

  if (strcmp(dest, ELEVEN_SPACES) == 0) { 
    // disk label not in boot sector. check root directory
    int offset;
    for (offset = ROOT_START; offset <= ROOT_END && disk[offset] != 0x00; offset += ROOT_ENTRY_SIZE) {
      unsigned char* entry = disk + offset;
      unsigned char attr = dir_entry_attr(entry);
      if ((attr & DIR_ENTRY_VOLUME) && !(attr & DIR_ENTRY_SYSTEM))
        dir_entry_filename(entry, dest);
    }
  }
}

int disk_root_files(unsigned char* disk) {
  int offset;
  int root_files = 0;
  for (offset = ROOT_START; offset <= ROOT_END && disk[offset] != 0x00; offset += ROOT_ENTRY_SIZE) {
    unsigned char* entry = disk + offset;
    unsigned char attr = dir_entry_attr(entry);
    if (dir_entry_visible(entry) && !(attr & DIR_ENTRY_SUBDIRECTORY)) {
      root_files++;
    }
  }
  return root_files;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: diskinfo <disk image>\n");
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

  char os_name[BUFF_SIZE], volume_ID[BUFF_SIZE], buff[BUFF_SIZE];

  disk_os_name(disk, os_name);
  disk_label(disk, volume_ID);
  int root_files = disk_root_files(disk);

  printf("OS Name: %s\n", os_name);
  printf("Label of the disk: %s\n", volume_ID);
  printf("Total size of the disk: %d\n", disk_sectors(disk) * BYTES_PER_SECTOR);
  printf("Free size of the disk: %d\n", disk_free_sectors(disk) * BYTES_PER_SECTOR);
  printf("\n");
  printf("==============\n");
  printf("The number of files in the root directory (not including subdirectories): %d\n", root_files);
  printf("\n");
  printf("=============\n");
  printf("Number of FAT copies: %d\n", disk_FATs(disk));
  printf("Sectors per FAT: %d\n", disk_sectors_per_FAT(disk));

  munmap(disk, buf.st_size);
  close(file);
 
  return EXIT_SUCCESS;
}
