#include "disk.h"
#include "diskutils.h"

int disk_free_sectors(unsigned char* disk) {
  int physical_sector, FAT_entry, result = 0;
  for (physical_sector = 33; physical_sector <= 2879; physical_sector++) {
    FAT_entry = physical_sector - 33 + 2;
    if (get_FAT_entry(disk, FAT_entry) == 0)
      result++;
  }
  return result;
}

void disk_os_name(unsigned char* disk, char* dest) {
  memcpy(dest, disk + 3, 8);
  dest[8] = '\0';
}

int disk_FATs(unsigned char* disk) {
  return get_num(disk + 16, 1);
}

int disk_sectors(unsigned char* disk) {
  return get_num(disk + 19, 2);
}

int disk_sectors_per_FAT(unsigned char* disk) {
  return get_num(disk + 22, 2);
}

int get_FAT_entry(unsigned char* bytes, int n) {
  int result;
  int byte1;
  int byte2;

  if ((n % 2) == 0) {
    byte1 = bytes[BYTES_PER_SECTOR + ((3*n) / 2) + 1] & 0x0F;
    byte2 = bytes[BYTES_PER_SECTOR + ((3*n) / 2)] & 0xFF;
    result = (byte1 << 8) + byte2;
  } else {
    byte1 = bytes[BYTES_PER_SECTOR + ((3*n) / 2)] & 0xF0;
    byte2 = bytes[BYTES_PER_SECTOR + ((3*n) / 2) + 1] & 0xFF;
    result = (byte1 >> 4) + (byte2 << 4);
  }

  return result;
}

/*
 * Writes FAT entry to FAT table specified table (1 or 2)
 */
void put_FAT_entry_internal(unsigned char* bytes, int n, int entry, int table) {
  int offset;
  if (table == 1)
    offset = BYTES_PER_SECTOR;
  else
    offset = BYTES_PER_SECTOR * 10;
  if ((n % 2) == 0) {
      bytes[offset + 1 + 3*n/2] &= 0xF0; // clear low 4 bits
      int mask = (entry & 0xF00) >> 8; // 4 msb's of 12 bit entry
      bytes[offset + 1 + 3*n/2] |= mask;
      bytes[offset + 3*n/2] = 0x0FF & entry; // 8 lsb's of 12 bit entry
    } else {
      bytes[offset + 3*n/2] &= 0x0F; // clear high 4 bits
      int mask = (entry & 0x0F) << 4; // 4 lsb of 12 bit entry in high 4 bits
      bytes[offset + 3*n/2] |= mask;
      bytes[offset + 1 + 3*n/2] = (entry & 0xFF0) >> 4; // 8 msb of 12 bit entry
    }
}

void put_FAT_entry(unsigned char* bytes, int n, int entry) {
  put_FAT_entry_internal(bytes, n, entry, 1);
  put_FAT_entry_internal(bytes, n, entry, 2);
}


