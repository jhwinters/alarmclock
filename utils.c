#include "includes.h"

void safe_copy(
    char       *dest,
    const char *src,
    int         limit,
    const char *setting) {

  if (strlen(src) > limit) {
    LOG_Warning("Setting \"%s\" truncated.\n", setting);
    strncpy(dest, src, limit);
    dest[limit] = '\0';
  } else {
    strcpy(dest, src);
  }
}


int integer(const char *string) {
  return (int) strtol(string, NULL, 10);
}



