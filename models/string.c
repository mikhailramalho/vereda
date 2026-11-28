// Models of <string.h> and <strings.h> functions, written from the C
// standard (C17 7.24) for programs that call them without a body. memcpy,
// memmove and memset are handled by the walker itself, and so are memcmp,
// bcmp and memchr when their length has a constant bound; their loops here
// run only for a length the walker cannot bound. Each loop runs only
// as far as the result needs, as glibc's do: the walker unrolls every
// iteration, and --unwind bounds each loop.
#include <stddef.h>

void *memset(void *, int, size_t);
void *malloc(size_t);
void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
int tolower(int);
void *__VERIFIER_nondet_pointer(void);
int __VERIFIER_nondet_int(void);
unsigned long __VERIFIER_nondet_ulong(void);
// Gives the whole object p points into nondet contents (the walker does).
void __vereda_havoc(void *p);

size_t strlen(const char *s) {
  size_t n = 0;
  while (s[n])
    ++n;
  return n;
}

size_t strnlen(const char *s, size_t max) {
  size_t n = 0;
  while (n < max && s[n])
    ++n;
  return n;
}

int strcmp(const char *a, const char *b) {
  const unsigned char *x = (const unsigned char *)a, *y = (const unsigned char *)b;
  while (*x && *x == *y) {
    ++x;
    ++y;
  }
  return *x - *y;
}

int strncmp(const char *a, const char *b, size_t n) {
  const unsigned char *x = (const unsigned char *)a, *y = (const unsigned char *)b;
  for (; n; --n, ++x, ++y)
    if (*x != *y || !*x)
      return *x - *y;
  return 0;
}

char *strcpy(char *dst, const char *src) {
  char *d = dst;
  while ((*d++ = *src++))
    ;
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i = 0;
  for (; i < n && src[i]; ++i)
    dst[i] = src[i];
  memset(dst + i, 0, n - i); // the padding, without a loop
  return dst;
}

char *strcat(char *dst, const char *src) {
  strcpy(dst + strlen(dst), src);
  return dst;
}

char *strncat(char *dst, const char *src, size_t n) {
  size_t i = strlen(dst);
  size_t j = 0;
  while (j < n && src[j]) {
    dst[i++] = src[j++];
  }
  dst[i] = 0;
  return dst;
}

char *strchr(const char *s, int c) {
  for (;; ++s) {
    if (*s == (char)c)
      return (char *)s;
    if (!*s)
      return NULL;
  }
}

// From one match to the next, so that its own loop runs once per match.
char *strrchr(const char *s, int c) {
  if (!(char)c)
    return strchr(s, 0);
  const char *last = NULL;
  for (const char *p; (p = strchr(s, c)); s = p + 1)
    last = p;
  return (char *)last;
}

size_t strspn(const char *s, const char *accept) {
  size_t n = 0;
  for (; s[n]; ++n) {
    const char *a;
    for (a = accept; *a && *a != s[n]; ++a)
      ;
    if (!*a)
      break;
  }
  return n;
}

size_t strcspn(const char *s, const char *reject) {
  size_t n = 0;
  for (; s[n]; ++n) {
    const char *r;
    for (r = reject; *r && *r != s[n]; ++r)
      ;
    if (*r)
      break;
  }
  return n;
}

char *strpbrk(const char *s, const char *accept) {
  for (; *s; ++s) {
    const char *a;
    for (a = accept; *a && *a != *s; ++a)
      ;
    if (*a)
      return (char *)s;
  }
  return NULL;
}

// Returns from inside the loop, so that comparing n equal bytes takes n
// iterations and no extra visit to test the count.
int memcmp(const void *a, const void *b, size_t n) {
  const unsigned char *x = a, *y = b;
  if (!n)
    return 0;
  do {
    if (*x != *y)
      return *x - *y;
    ++x;
    ++y;
  } while (--n);
  return 0;
}

int bcmp(const void *a, const void *b, size_t n) { return memcmp(a, b, n); }

void *memchr(const void *s, int c, size_t n) {
  const unsigned char *p = s;
  for (; n; --n, ++p)
    if (*p == (unsigned char)c)
      return (void *)p;
  return NULL;
}

char *strstr(const char *haystack, const char *needle) {
  if (!*needle)
    return (char *)haystack;
  for (; *haystack; ++haystack) {
    const char *h = haystack, *n = needle;
    while (*n && *h == *n) {
      ++h;
      ++n;
    }
    if (!*n)
      return (char *)haystack;
  }
  return NULL;
}

char *strdup(const char *s) {
  size_t n = strlen(s);
  char *d = malloc(n + 1);
  if (!d)
    return NULL;
  return memcpy(d, s, n + 1);
}

char *strndup(const char *s, size_t n) {
  size_t m = strnlen(s, n);
  char *d = malloc(m + 1);
  if (!d)
    return NULL;
  memcpy(d, s, m);
  d[m] = 0;
  return d;
}

size_t wcslen(const wchar_t *s) {
  size_t n = 0;
  while (s[n])
    ++n;
  return n;
}

// wcsdup (POSIX): strdup for a wide string.
wchar_t *wcsdup(const wchar_t *s) {
  size_t n = (wcslen(s) + 1) * sizeof(wchar_t);
  wchar_t *d = malloc(n);
  if (!d)
    return NULL;
  return memcpy(d, s, n);
}

char *index(const char *s, int c) { return strchr(s, c); }

char *rindex(const char *s, int c) { return strrchr(s, c); }

void bzero(void *p, size_t n) { memset(p, 0, n); }

void bcopy(const void *src, void *dst, size_t n) { memmove(dst, src, n); }

void *memccpy(void *dst, const void *src, int c, size_t n) {
  unsigned char *d = dst;
  const unsigned char *s = src;
  for (; n; --n) {
    *d = *s++;
    if (*d++ == (unsigned char)c)
      return d;
  }
  return NULL;
}

int strcasecmp(const char *a, const char *b) {
  const unsigned char *x = (const unsigned char *)a, *y = (const unsigned char *)b;
  while (*x && tolower(*x) == tolower(*y)) {
    ++x;
    ++y;
  }
  return tolower(*x) - tolower(*y);
}

int strncasecmp(const char *a, const char *b, size_t n) {
  const unsigned char *x = (const unsigned char *)a, *y = (const unsigned char *)b;
  for (; n; --n, ++x, ++y)
    if (tolower(*x) != tolower(*y) || !*x)
      return tolower(*x) - tolower(*y);
  return 0;
}

// Collation is strcmp's order in the "C" locale, where a program starts
// (C17 7.11.1.1p4). Once setlocale has been given a locale, strcoll and
// strxfrm give any result; setlocale returns any pointer, as it would
// without a body.
static int c_collation = 1;

char *setlocale(int category, const char *locale) {
  if (locale)
    c_collation = 0;
  return __VERIFIER_nondet_pointer();
}

int strcoll(const char *a, const char *b) {
  return c_collation ? strcmp(a, b) : __VERIFIER_nondet_int();
}

size_t strxfrm(char *dst, const char *src, size_t n) {
  if (!c_collation) {
    if (n)
      __vereda_havoc(dst);
    return __VERIFIER_nondet_ulong();
  }
  size_t len = strlen(src);
  if (len < n)
    for (size_t i = 0; i <= len; ++i)
      dst[i] = src[i];
  return len;
}

char *strerror(int errnum) { return (char *)(errnum ? "Unknown error" : "Success"); }

// POSIX strtok_r: skips the leading delimiters, then ends the token at the
// next one, if any, and saves where the next call starts.
char *strtok_r(char *str, const char *delim, char **save) {
  if (!str)
    str = *save;
  str += strspn(str, delim);
  if (!*str) {
    *save = str;
    return NULL;
  }
  char *end = str + strcspn(str, delim);
  if (*end)
    *end++ = 0;
  *save = end;
  return str;
}

char *strtok(char *str, const char *delim) {
  static char *save;
  return strtok_r(str, delim, &save);
}

// BSD strsep: unlike strtok, it does not skip leading delimiters, so a
// token can be empty.
char *strsep(char **sp, const char *delim) {
  char *tok = *sp;
  if (!tok)
    return NULL;
  char *end = tok + strcspn(tok, delim);
  if (*end)
    *end++ = 0;
  else
    end = NULL;
  *sp = end;
  return tok;
}

size_t strlcpy(char *dst, const char *src, size_t size) {
  size_t i = 0;
  size_t srclen = strlen(src);
  if (size > 0) {
    for (; i < size - 1 && src[i]; ++i)
      dst[i] = src[i];
    dst[i] = 0;
  }
  return srclen;
}
