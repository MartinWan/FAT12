#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "dir_entry.h"
#include "diskutils.h"

void dir_entry_filename(unsigned char* entry, char* dest) {
  memcpy(dest, entry + 0, 8);
  dest[8] = '\0';
  strip_leading_spaces(dest);
}

void dir_entry_extension(unsigned char* entry, char *dest) {
  memcpy(dest, entry + 8, 3);
  dest[3] = '\0';
  strip_leading_spaces(dest);
}

unsigned char dir_entry_attr(unsigned char* entry) {
  return entry[11];
}

date_time dir_entry_creation_time(unsigned char* entry) {
  date_time result;
  int creation_time = get_num(entry + 14, 2);
  int creation_date = get_num(entry + 16, 2);

  result.mins = (0b11111) & (creation_time >> 5);
  result.hrs = (0b1111) & (creation_time >> 11);
  result.day = (0b11111) & (creation_date);
  result.month = (0b1111) & (creation_date >> 5);
  result.year = ((0b1111111) & (creation_date >> 9)) + 1980;

  return result;
}

int dir_entry_cluster(unsigned char* entry) {
  return get_num(entry + 26, 2);
}

int dir_entry_size(unsigned char* entry) {
  return get_num(entry + 28, 4);
}

bool dir_entry_free(unsigned char* entry) {
  if (entry[0] == 0xE5) 
    return true;
  if (entry[0] == 0x00)
    return true;
  return false;
}

void dir_entry_set_filename(unsigned char* entry, char* str) {
  memset(entry, ' ', 8);
  memcpy(entry, str, strlen(str));
}

void dir_entry_set_extension(unsigned char* entry, char* str) {
  memset(entry + 8, ' ', 3);
  memcpy(entry + 8, str, strlen(str));
}

void dir_entry_set_size(unsigned char* entry, int size) {
  set_num(entry + 28, size, 4);
}

void dir_entry_set_cluster(unsigned char* entry, int first_logical_cluster) {
  set_num(entry + 26, first_logical_cluster, 2);
}

bool dir_entry_visible(unsigned char* entry) {
  unsigned char attr = dir_entry_attr(entry);
  if (attr & DIR_ENTRY_HIDDEN) return false;
  if (attr & DIR_ENTRY_SYSTEM) return false;
  if (attr & DIR_ENTRY_VOLUME) return false;
  if (attr & DIR_ENTRY_ARCHIVE) return false;
  if (attr & DIR_ENTRY_DEVICE) return false;
  if (attr & DIR_ENTRY_RESERVED) return false;

  return true;
}

void dir_entry_set_visible(unsigned char* entry) {
  entry[11] = 0;
}

void dir_entry_print(unsigned char* entry) {
  char buff[BUFF_SIZE];
  dir_entry_display_name(entry, buff);
  
  int directory = dir_entry_attr(entry) & DIR_ENTRY_SUBDIRECTORY;
  date_time creation = dir_entry_creation_time(entry);

  printf("%c %10d %20s %4d-%02d-%02d %02d:%02d\n",
      directory? 'D': 'F', dir_entry_size(entry), buff, creation.year, creation.month,
      creation.day, creation.hrs, creation.mins);
}

void dir_entry_display_name(unsigned char* entry, char* dest) {
  char filename[BUFF_SIZE], extension[BUFF_SIZE];
  dir_entry_filename(entry, filename);
  dir_entry_extension(entry, extension);
  dest[0] = '\0';
  strcat(dest, filename);
  strcat(dest, ".");
  strcat(dest, extension);
}
