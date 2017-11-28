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
#define DATA_SECTOR_START 33
#define DATA_SECTOR_END 2879


/*
 * Writes file into disk clusters and updates FAT entries
 *
 * Returns the first logical cluster of the written file
 */
int write_clusters(unsigned char* disk, unsigned char* file, int file_size) {
  int n = (file_size + BYTES_PER_SECTOR - 1) / BYTES_PER_SECTOR;
  int free_logical_clusters[n+1];
  int sector, count;
  for (sector = DATA_SECTOR_START, count = 0; sector <= DATA_SECTOR_END && count < n; sector++) {
    int FATEntry = sector - 33 + 2;
    if (get_FAT_entry(disk, FATEntry) == 0x000) 
      free_logical_clusters[count++] = sector - 33 + 2;
  }
  free_logical_clusters[n] = 0xFF8;

  int i;
  for (i = 0; i < n; i++) {
    int cluster_disk = BYTES_PER_SECTOR;
    if (i == n-1 && ((file_size) % BYTES_PER_SECTOR) != 0)
      cluster_disk = (file_size) % BYTES_PER_SECTOR;

    int file_offset = i * BYTES_PER_SECTOR;
    int disk_sector = free_logical_clusters[i] + 33 - 2;
    int disk_offset = disk_sector * BYTES_PER_SECTOR;

    memcpy(disk + disk_offset, file + file_offset, cluster_disk);
    int s0 = free_logical_clusters[i];
    int s1 = free_logical_clusters[i+1];

    put_FAT_entry(disk, s0, s1);
  }

  return free_logical_clusters[0];
}

bool enough_space(unsigned char* disk, int src_file_size) {
  int free_space = disk_free_sectors(disk) * BYTES_PER_SECTOR;
  return free_space >= src_file_size;
}

/*
 * Returns true iff file with display name (filename.extension) exists
 * in root directory
 */
bool file_in_root(unsigned char* disk, char* filename, char* extension) {
  int offset;
  char entry_filename[BUFF_SIZE], entry_extension[BUFF_SIZE];
  for (offset = ROOT_START; offset <= ROOT_END && disk[offset] != 0x00; offset += ROOT_ENTRY_SIZE) {
    dir_entry_filename(disk + offset, entry_filename);
    dir_entry_extension(disk + offset, entry_extension);
    if (strcmp(filename, entry_filename) == 0 && strcmp(extension, entry_extension) == 0)
      return true;
  }
  return false;
}

/*
 * Given a file display name of form FOO.TXT, writes string
 * "FOO" to filename and string "TXT" to extension.
 * If the file is of form "FOO", then the empty string is written to 
 * extension.
 */
void parse_file_display_name(char* s, char* filename, char* extension) {
  int i, n = strlen(s);
  int filename_length = n;
  for (i = 0; i < n; i++) {
    if (s[i] == '.') filename_length = i; 
  }
  memcpy(filename, s, filename_length);
  filename[filename_length] = '\0';
  if (filename_length == n) {
    strcpy(extension, "");
  } else {
    memcpy(extension, s + filename_length + 1, 3);
    extension[3] = '\0';
    strip_leading_spaces(extension);
  }
}

/*
 * Converts user input to valid FAT12 filenames and extensions.
 * (Capitalized strings)
 * Exits program if this is not possible
 */
void process_filename(char* input, char* filename, char* extension) {
  capitalize(input);
  strip_leading_spaces(input);
  parse_file_display_name(input, filename, extension);
  if (strlen(filename) > 8) {
    fprintf(stderr, "Error. File name cannot exceed 8 characters\n");
    exit(EXIT_FAILURE);
  }
  if (strlen(extension) > 3) {
    fprintf(stderr, "Error. File extension cannot exceed 3 characters\n");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "usage: diskput <disk image> <file>\n");
    return EXIT_FAILURE;
  }

  // open disk image
  int disk_file = open(argv[1], O_RDWR);
  if (disk_file < 0) {
    fprintf(stderr, "File not found.\n");
    return EXIT_FAILURE;
  }

  // map disk image
  struct stat disk_file_stat;
  fstat(disk_file, &disk_file_stat);
  unsigned char* disk = mmap(NULL, disk_file_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, disk_file, 0);
  if (disk == MAP_FAILED) {
    fprintf(stderr, "mmap error.\n");
    return EXIT_FAILURE;
  }

  // open file
  int src_file = open(argv[2], O_RDWR);
  if (src_file < 0) {
    fprintf(stderr, "File not found.\n");
    return EXIT_FAILURE;
  }

  // map file
  struct stat src_file_stat;
  fstat(src_file, &src_file_stat);
  unsigned char* src = mmap(NULL, src_file_stat.st_size, PROT_READ, MAP_SHARED, src_file, 0);
  if (src == MAP_FAILED) {
    fprintf(stderr, "mmap error.\n");
    return EXIT_FAILURE;
  }
  
  // process filename from user input
  char filename[BUFF_SIZE], extension[BUFF_SIZE];
  process_filename(argv[2], filename, extension);

  if (!enough_space(disk, src_file_stat.st_size)) {
    fprintf(stderr, "Not enough free space in the disk image.\n");
  } else if (file_in_root(disk, filename, extension)) {
    fprintf(stderr, "Error: file already exists in disk image.\n");
  } else {
    // write to disk
    int offset;
    for (offset = ROOT_START; offset <= ROOT_END; offset += ROOT_ENTRY_SIZE) {
      unsigned char* entry = disk + offset;
      if (dir_entry_free(entry)) {
        dir_entry_set_filename(entry, filename);
        dir_entry_set_extension(entry, extension);
        dir_entry_set_size(entry, src_file_stat.st_size);
        dir_entry_set_cluster(entry, write_clusters(disk, src, src_file_stat.st_size));
        dir_entry_set_visible(entry);

        break;
      }
    }
  }


  munmap(src, src_file_stat.st_size);
  munmap(disk, disk_file_stat.st_size);
  close(disk_file);
  close(src_file);

  return EXIT_SUCCESS;
}
