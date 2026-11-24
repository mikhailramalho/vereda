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

// Helper function to convert string to integer
static unsigned long long atoi_helper(const char *s) {
  // Skip leading whitespace (isspace)
  while (*s == ' ' || (*s >= '\t' && *s <= '\r'))
    s++;

  // Check for sign
  int neg = 0;
  if (*s == '-') {
    neg = 1;
    s++;
  } else if (*s == '+') {
    s++;
  }

  // Convert digits
  unsigned long long result = 0;
  while (*s >= '0' && *s <= '9') {
    result = result * 10 + (*s - '0');
    s++;
  }

  // Apply sign if needed
  if (neg)
    return -result;

  return result;
}

// Convert string to integer (C17 7.22.1.2)
int atoi(const char *s) {
  return (int)atoi_helper(s);
}

long atol(const char *s) {
  return (long)atoi_helper(s);
}

long long atoll(const char *s) {
  return (long long)atoi_helper(s);

// Binary search (C17 7.22.5.1): searches the sorted array base of nmemb elements
// of size bytes for an element equal to *key and returns a pointer to it, or NULL.
void *bsearch(const void *key, const void *base, size_t nmemb, size_t size,
              int (*compar)(const void *, const void *)) {
  size_t lo = 0;
  size_t hi = nmemb;

  while (lo < hi) {
    size_t mid = lo + (hi - lo) / 2;
    const char *p = (const char *)base + mid * size;
    int r = compar(key, p);

    if (r == 0)
      return (void *)p;
    else if (r < 0)
      hi = mid;
    else
      lo = mid + 1;
  }

  return NULL;
}

// Helper function to avoid llvm.scmp.i32.i32 intrinsic - replace the inline comparison logic with explicit comparisons
int __vereda_bsearch_cmp(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    if (x > y)
        return 1;
    else if (x < y)
        return -1;
    else
        return 0;
}
