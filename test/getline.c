#include "getline.h"

#ifdef __MINGW32__
size_t getline(char** lineptr, size_t* n, FILE* stream)
{
  char* bufptr;
  char* p;
  size_t size;
  int c;
  if (!lineptr || !n || !stream)
    return -1;
  bufptr = *lineptr;
  size = *n;
  c = fgetc(stream);
  if (c == EOF)
    return -1;
  if (!bufptr) {
    if ((bufptr = (char*)malloc(GETLINE_BUFLEN)) == NULL)
      return -1;
    size = GETLINE_BUFLEN;
  }
  p = bufptr;
  while (c != EOF) {
    if ((p - bufptr) > (size - 1)) {
      size = size + GETLINE_BUFLEN;
      if ((bufptr = (char*)realloc(bufptr, size)) == NULL)
        return -1;
    }
    *p++ = c;
    if (c == '\n') {
      break;
    }
    c = fgetc(stream);
  }
  *p++ = 0;
  *lineptr = bufptr;
  *n = size;
  return p - bufptr - 1;
}
#endif
