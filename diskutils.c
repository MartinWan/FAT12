#include <ctype.h>
#include <string.h>
#include "diskutils.h"

void capitalize(char* s) {
  int i, n = strlen(s);
  for (i = 0; i < n; i++)
    if (isalpha(s[i]))
      s[i] = toupper(s[i]);
}

void set_num(unsigned char* dest, int num, int n) {
  int i;
  unsigned char ibyte;
  for (i = 0; i < n; i++) {
    ibyte = num >> (8*i);
    dest[i] = ibyte;
  }
}

int get_num(unsigned char* dest, int n) {
  int i;
  int result = 0;
  for (i = 0; i < n; i++) {
    int ibyte = dest[i];
    result |= ibyte << (i*8);
  }
  return result;
}

void strip_leading_spaces(char *str) {
  int n = strlen(str);
  int i = n-1;
  while (i >= 0 && str[i] == ' ') {
    str[i--] = '\0';
  }
}
