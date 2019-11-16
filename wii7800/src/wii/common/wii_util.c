#include <ctype.h>
#include <string.h>
#include <sys/stat.h>

#include "wii_util.h"

void Util_chomp(char *s)
{
  int len;

  len = strlen(s);
  if (len >= 2 && s[len - 1] == '\n' && s[len - 2] == '\r')
    s[len - 2] = '\0';
  else if (len >= 1 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
    s[len - 1] = '\0';
}

void Util_trim(char *s)
{
  char *p = s;
  char *q;
  /* skip leading whitespace */
  while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
    p++;
  /* now p points at the first non-whitespace character */

  if (*p == '\0') {
    /* only whitespace */
    *s = '\0';
    return;
  }

  q = s + strlen(s);
  /* skip trailing whitespace */
  /* we have found p < q such that *p is non-whitespace,
  so this loop terminates with q >= p */
  do
  q--;
  while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n');

  /* now q points at the last non-whitespace character */
  /* cut off trailing whitespace */
  *++q = '\0';

  /* move to string */
  memmove(s, p, q + 1 - p);
}

int Util_sscandec(const char *s)
{
  int neg = 0;
  int result;
  if (*s == '\0')
    return -1;
  result = 0;
  for (;;) {
    if (*s >= '0' && *s <= '9')
      result = 10 * result + *s - '0';
    else if (*s == '\0')
      return neg ? -result : result;
    else if (*s == '-')
      neg = 1;
    else
      return -1;
    s++;
  }
}

char *Util_strlcpy(char *dest, const char *src, size_t size)
{
  strncpy(dest, src, size);
  dest[size - 1] = '\0';
  return dest;
}

int Util_fileexists( char *filename )
{
  struct stat buf;
  int i = stat( filename, &buf );       
  return i == 0;
}

void Util_splitpath(const char *path, char *dir_part, char *file_part)
{
  const char *p;
  /* find the last DIR_SEP_CHAR except the last character */
  for (p = path + strlen(path) - 2; p >= path; p--) {
    if (*p == DIR_SEP_CHAR
#ifdef BACK_SLASH
      /* on DOSish systems slash can be also used as a directory separator */
      || *p == '/'
#endif
      ) {
        if (dir_part != NULL) {
          int len = p - path;
          if (p == path || (p == path + 2 && path[1] == ':'))
            /* root dir: include DIR_SEP_CHAR in dir_part */
            len++;
          memcpy(dir_part, path, len);
          dir_part[len] = '\0';
        }
        if (file_part != NULL)
          strcpy(file_part, p + 1);
        return;
    }
  }
  /* no DIR_SEP_CHAR: current dir */
  if (dir_part != NULL)
    dir_part[0] = '\0';
  if (file_part != NULL)
    strcpy(file_part, path);
}

void Util_getextension( char *filename, char *ext )
{
  char *ptr = strrchr( filename, '.' );
  int index = 0;
  if( ptr != NULL )
  {
    ++ptr;    
    while( *ptr != '\0' )
    {
      ext[index++] = tolower((unsigned char)*ptr);        
      ++ptr;
    }
  }
  ext[index] = '\0';
}

