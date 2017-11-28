#include <string.h>
#include <stdbool.h>
#define BYTES_PER_SECTOR 512
#define ROOT_START (19 * BYTES_PER_SECTOR)
#define ROOT_END (32 * BYTES_PER_SECTOR)
#define ROOT_ENTRY_SIZE 32

/*
 * Returns the number of data sectors free
 */
int disk_free_sectors(unsigned char* disk);

/*
 * Writes the os name of the disk to dest
 */
void disk_os_name(unsigned char* disk, char* dest);

/*
 * Returns number of FAT copies
 */
int disk_FATs(unsigned char* disk);

int disk_sectors(unsigned char* disk);

int disk_sectors_per_FAT(unsigned char* disk);

/*
 * Returns the nth FAT entry on disk image 
 */
int get_FAT_entry(unsigned char* disk, int n);

/*
 * Writes to the nth FAT entry on disk image
 */
void put_FAT_entry(unsigned char* disk, int n, int entry);

bool dir_entry_visible(unsigned char* disk);

void dir_entry_print(unsigned char* disk);
