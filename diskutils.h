#include <stdbool.h>
#define BUFF_SIZE 1000

/*
 * capitalizes string s
 */
void capitalize(char* s);

/*
 * Sets n least significant bytes of num into dest
 * in little endian format
 */
void set_num(unsigned char* dest, int num, int n);

/*
 * Returns n-byte number stored at dest
 * in little endian format
 */
int get_num(unsigned char* dest, int n);

/*
 * Removes leading spaces from string
 */
void strip_leading_spaces(char* str);

