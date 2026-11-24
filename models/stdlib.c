// Models of <stdlib.h> functions, written from the C standard (C17 7.22).
#include <inttypes.h>
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

// C17 7.22.5.2.
// Use insertion sort: for i from 1 to nmemb - 1, for j = i while j > 0 and
// compar(element j - 1, element j) > 0, swap the two elements and decrement j.
// Swap byte by byte with a loop over size (a char temporary), not memcpy:
// the walker cannot do a memcpy of symbolic size.
void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *)) {
  for (size_t i = 1; i < nmemb; ++i) {
    char tmp[size];
    char *elem_i = (char *)base + i * size;
    char *elem_j = elem_i;
    size_t j = i;

    while (j > 0) {
      char *elem_prev = (char *)base + (j - 1) * size;
      int r = compar(elem_prev, elem_j);
      if (r <= 0)
        break;

      // Swap elements byte by byte
      for (size_t k = 0; k < size; ++k) {
        tmp[k] = elem_prev[k];
        elem_prev[k] = elem_j[k];
        elem_j[k] = tmp[k];
      }

      --j;
      elem_j = elem_prev;
    }
  }
}
