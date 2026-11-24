// Models of <stdlib.h> functions, written from the C standard (C17 7.22).
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>

void *malloc(size_t);
void free(void *);
void *memcpy(void *, const void *, size_t);
// The bytes from p to the end of its object; the walker computes it.
size_t __vereda_object_size(const void *p);

// A new object with the old contents up to the smaller size, as glibc
// does; realloc(p, 0) frees p and returns null, as glibc does. On failure
// the old object stays.
void *realloc(void *p, size_t n) {
  if (!p)
    return malloc(n);
  if (!n) {
    free(p);
    return NULL;
  }
  void *q = malloc(n);
  if (!q)
    return NULL;
  size_t old = __vereda_object_size(p);
  memcpy(q, p, old < n ? old : n);
  free(p);
  return q;
}

int __VERIFIER_nondet_int(void);
void __VERIFIER_assume(int);

// Any value from 0 to RAND_MAX (C17 7.22.2.1), so srand needs no model.
int rand(void) {
  int r = __VERIFIER_nondet_int();
  __VERIFIER_assume(r >= 0 && r <= 2147483647); // glibc's RAND_MAX
  return r;
}

// Quotient and remainder, truncated toward zero (C17 7.22.6.2); imaxdiv is
// <inttypes.h>'s (C17 7.8.2.2). A struct result of a bodyless call is
// unsupported, so these need their bodies.
div_t div(int n, int d) { return (div_t){n / d, n % d}; }
ldiv_t ldiv(long n, long d) { return (ldiv_t){n / d, n % d}; }
lldiv_t lldiv(long long n, long long d) { return (lldiv_t){n / d, n % d}; }
imaxdiv_t imaxdiv(intmax_t n, intmax_t d) { return (imaxdiv_t){n / d, n % d}; }

// atoi, atol and atoll share one reader; it accumulates in unsigned long long,
// so the model wraps where an out-of-range result is undefined in C.
static unsigned long long atoi_helper(const char *s) {
  while (*s == ' ' || (*s >= '\t' && *s <= '\r'))
    ++s;
  int neg = *s == '-';
  if (*s == '-' || *s == '+')
    ++s;
  unsigned long long n = 0;
  for (; *s >= '0' && *s <= '9'; ++s)
    n = n * 10 + (*s - '0');
  return neg ? -n : n;
}

int atoi(const char *s) { return atoi_helper(s); }
long atol(const char *s) { return atoi_helper(s); }
long long atoll(const char *s) { return atoi_helper(s); }

// C17 7.22.5.1.
void *bsearch(const void *key, const void *base, size_t nmemb, size_t size,
              int (*compar)(const void *, const void *)) {
  size_t lo = 0, hi = nmemb;
  while (lo < hi) {
    size_t mid = lo + (hi - lo) / 2;
    const char *p = (const char *)base + mid * size;
    int r = compar(key, p);
    if (!r)
      return (void *)p;
    if (r < 0)
      hi = mid;
    else
      lo = mid + 1;
  }
  return NULL;
}

// C17 7.22.5.2, as an insertion sort. Elements are swapped a byte at a
// time: the walker cannot copy a symbolic size with memcpy.
void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *)) {
  char *b = base;
  for (size_t i = 1; i < nmemb; ++i)
    for (size_t j = i; j; --j) {
      char *p = b + (j - 1) * size, *q = p + size;
      if (compar(p, q) <= 0)
        break;
      for (size_t k = 0; k < size; ++k) {
        char t = p[k];
        p[k] = q[k];
        q[k] = t;
      }
    }
}

// The value of digit c in bases up to 36, or 36 if c is no digit.
static int digit(int c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  c |= 32;
  return c >= 'a' && c <= 'z' ? c - 'a' + 10 : 36;
}

// Reads an integer as strtol and its kin do (C17 7.22.1.4), for a type whose
// largest value is max, signed or not. Past the type's range it sets errno to
// ERANGE and gives the nearest limit; an unsigned type negates in its own
// arithmetic, so "-1" gives its largest value. A base C does not define
// sets errno to EINVAL, as glibc does.
static unsigned long long strto(const char *nptr, char **endptr, int base,
                                unsigned long long max, int sign) {
  if (base < 0 || base == 1 || base > 36) {
    errno = EINVAL;
    return 0;
  }
  const char *s = nptr;
  while (*s == ' ' || (*s >= '\t' && *s <= '\r'))
    ++s;
  int neg = *s == '-';
  if (*s == '-' || *s == '+')
    ++s;
  if ((base == 0 || base == 16) && s[0] == '0' && (s[1] | 32) == 'x' && digit(s[2]) < 16) {
    s += 2;
    base = 16;
  } else if (!base)
    base = *s == '0' ? 8 : 10;
  unsigned long long limit = max + (sign && neg), n = 0;
  int over = 0;
  const char *start = s;
  for (int d; (d = digit(*s)) < base; ++s) {
    if (n > (limit - d) / base)
      over = 1;
    else
      n = n * base + d;
  }
  if (endptr)
    *endptr = (char *)(s == start ? nptr : s);
  if (over) {
    errno = ERANGE;
    return sign && neg ? -limit : max;
  }
  return neg ? -n : n;
}

long strtol(const char *s, char **end, int base) { return strto(s, end, base, LONG_MAX, 1); }
unsigned long strtoul(const char *s, char **end, int base) {
  return strto(s, end, base, ULONG_MAX, 0);
}
long long strtoll(const char *s, char **end, int base) { return strto(s, end, base, LLONG_MAX, 1); }
unsigned long long strtoull(const char *s, char **end, int base) {
  return strto(s, end, base, ULLONG_MAX, 0);
}
