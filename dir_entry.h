#include <time.h>

#define DIR_ENTRY_READ_ONLY 0x01
#define DIR_ENTRY_HIDDEN 0x02
#define DIR_ENTRY_SYSTEM 0x04
#define DIR_ENTRY_VOLUME 0x08
#define DIR_ENTRY_SUBDIRECTORY 0x10
#define DIR_ENTRY_ARCHIVE 0x20
#define DIR_ENTRY_DEVICE 0x40
#define DIR_ENTRY_RESERVED 0x80

typedef struct {
  int year, month, day, hrs, mins;
} date_time;

/*
 * Writes the (non-space-padded) filename of the directory entry
 * into dest as a string.
 */
void dir_entry_filename(unsigned char* entry, char* dest);

/*
 * Writes the (non-space-padded) extension of the directory entry 
 * into dest as a string.
 */
void dir_entry_extension(unsigned char* entry, char *dest);

/*
 * Returns the attribute bitmask of the directory entry
 */
unsigned char dir_entry_attr(unsigned char* entry);

/*
 * Returns the creation time of the directory entry
 */
date_time dir_entry_creation_time(unsigned char* entry);

/*
 * Returns the first logical cluster of the directory entry
 */
int dir_entry_cluster(unsigned char* entry);

/*
 * Returns file size (in bytes)
 */
int dir_entry_size(unsigned char *entry);

/*
 * Returns true iff the directory entry is free
 * (i.e. currently unused)
 */
bool dir_entry_free(unsigned char* entry);

/*
 * Sets filename of directory entry equal to string
 * (pads with spaces)
 * Preconditions: strlen(str) <= 8
 */
void dir_entry_set_filename(unsigned char* entry, char* str);

/*
 * Sets filename of directory entry equal to string
 * (pads with spaces)
 * Preconditions: strlen(str) <= 3
 */
void dir_entry_set_extension(unsigned char* entry, char* str);

/*
 * Sets size of entry
 */
void dir_entry_set_size(unsigned char* entry, int size);

/*
 * Set first logical cluster of entry
 */
void dir_entry_set_cluster(unsigned char* entry, int first_logical_cluster);

/*
 * Returns true iff entry is visible to user
 */
bool dir_entry_visible(unsigned char* entry);

/*
 * Make dir entry visible
 */
void dir_entry_set_visible(unsigned char* entry);

/*
 * Writes directory entry display name to dest
 */
void dir_entry_display_name(unsigned char* entry, char* dest);
