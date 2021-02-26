// The -*- C++ -*- null-terminated string header.
// This file is part of the GNU ANSI C++ Library.

#ifndef __CSTRING__
#define __CSTRING__

// The ANSI C prototypes for these functions have a const argument type and
// non-const return type, so we can't use them.

#define strchr  __hide_strchr
#define strpbrk __hide_strpbrk
#define strrchr __hide_strrchr
#define strstr  __hide_strstr
#define memchr  __hide_memchr

#include_next <string.h>

#undef strchr
#undef strpbrk
#undef strrchr
#undef strstr
#undef memchr

#include <std/cstddef.h>

#ifdef __GNUG__
#pragma interface "std/cstring.h"
#endif

extern "C" const char *strchr (const char *, int);
/*inline char *
strchr (char *s, int c)
{
  return (char*) strchr ((const char *) s, c);
}
*/
extern "C" const char *strpbrk (const char *, const char *);
/*inline char *
strpbrk (char *s1, const char *s2)
{
  return (char *) strpbrk ((const char *) s1, s2);
}
*/
extern "C" const char *strrchr (const char *, int);
/*inline char *
strrchr (char *s, int c)
{
  return (char *) strrchr ((const char *) s, c);
}
*/
extern "C" const char *strstr (const char *, const char *);
/*
inline char *
strstr (char *s1, const char *s2)
{
  return (char *) strstr ((const char *) s1, s2);
}
*/
extern "C" const void *memchr (const void *, int, size_t);
/*inline void *
memchr (void *s, int c, size_t n)
{
  return (void *) memchr ((const void *) s, c, n);
}
*/
#endif
