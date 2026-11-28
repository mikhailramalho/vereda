// Models of <stdlib.h> functions, written from the C standard (C17 7.22),
// and of POSIX and glibc functions that allocate their result.
#include <dirent.h>
#include <errno.h>
#include <glob.h>
#include <inttypes.h>
#include <limits.h>
#include <netdb.h>
#include <stdlib.h>

void *malloc(size_t);
void free(void *);
void *memcpy(void *, const void *, size_t);
// The bytes from p to the end of its object; the walker computes it.
size_t __vereda_object_size(const void *p);
// The walker's checks of free(p), without the free.
void __vereda_check_free(void *p);
void *memalign(size_t, size_t);
int __VERIFIER_nondet_int(void);
unsigned long __VERIFIER_nondet_ulong(void);
void __VERIFIER_assume(int);
// Gives the whole object p points into nondet contents (the walker does).
void __vereda_havoc(void *p);

// A new object with the old contents up to the smaller size, as glibc
// does; realloc(p, 0) frees p and returns null, as glibc does. On failure
// the old object stays. A p that free could not take is undefined even
// where the allocation fails (C17 7.22.3.5p3), so it is checked first.
void *realloc(void *p, size_t n) {
  if (!p)
    return malloc(n);
  __vereda_check_free(p);
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

// reallocarray (glibc 2.26, POSIX 2024): realloc of n * size bytes, but
// null with errno ENOMEM where the product overflows, p left as it was.
void *reallocarray(void *p, size_t n, size_t size) {
  size_t bytes;
  if (__builtin_mul_overflow(n, size, &bytes)) {
    errno = ENOMEM;
    return NULL;
  }
  return realloc(p, bytes);
}

// realpath (POSIX): the absolute path of path, len characters and a NUL,
// in resolved, which must hold PATH_MAX bytes, or with a null resolved in
// a new object of len + 1 bytes, as glibc allocates. getcwd (POSIX): the
// working directory's, in buf of size bytes, null with ERANGE where it
// does not fit and EINVAL for a size of 0; with a null buf glibc
// allocates size bytes, or len + 1 for a size of 0. The characters are
// not modelled. Either may fail (ENOENT, EACCES, ...), and errno may
// change at any call (POSIX 2.3).
char *realpath(const char *path, char *resolved) {
  errno = __VERIFIER_nondet_int();
  size_t len = __VERIFIER_nondet_ulong();
  if (!len || len >= PATH_MAX)
    return NULL;
  if (!resolved && !(resolved = malloc(len + 1)))
    return NULL;
  __vereda_havoc(resolved);
  resolved[len] = 0;
  return resolved;
}

char *getcwd(char *buf, size_t size) {
  errno = __VERIFIER_nondet_int();
  size_t len = __VERIFIER_nondet_ulong();
  if (!len || len >= PATH_MAX || (buf && !size) || (size && len >= size))
    return NULL;
  if (!buf && !(buf = malloc(size ? size : len + 1)))
    return NULL;
  __vereda_havoc(buf);
  buf[len] = 0;
  return buf;
}

// glibc's: realpath(path, NULL), and getcwd(NULL, 0) unless $PWD names the
// working directory, which gives the same allocation.
char *canonicalize_file_name(const char *path) { return realpath(path, NULL); }
char *get_current_dir_name(void) { return getcwd(NULL, 0); }

// scandir (POSIX): reads n entries of dir, each into a new object, keeps
// those filter accepts (all with a null filter) in a new array stored in
// *namelist, sorted with compar where given, and frees the others; -1
// where dir cannot be read or memory runs out, everything freed, as glibc
// does. The entries' contents are not modelled.
int scandir(const char *dir, struct dirent ***namelist,
            int (*filter)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **)) {
  errno = __VERIFIER_nondet_int();
  int n = __VERIFIER_nondet_int();
  if (n < 0)
    return -1;
  struct dirent **v = NULL;
  if (n && !(v = calloc(n, sizeof *v)))
    return -1;
  int k = 0;
  for (int i = 0; i < n; ++i) {
    struct dirent *d = malloc(sizeof *d);
    if (!d) {
      while (k)
        free(v[--k]);
      free(v);
      return -1;
    }
    __vereda_havoc(d);
    if (!filter || filter(d))
      v[k++] = d;
    else
      free(d);
  }
  if (compar)
    qsort(v, k, sizeof *v, (int (*)(const void *, const void *))compar);
  *namelist = v;
  return k;
}

// glob (POSIX): adds n names matching pattern, each in a new object, to
// gl_pathv after gl_offs null pointers (GLOB_DOOFFS) and, with GLOB_APPEND,
// the names of earlier calls, and ends it with a null pointer. It fails
// with GLOB_NOSPACE, with GLOB_ABORTED where a directory cannot be read and
// errfunc returns nonzero or GLOB_ERR is set, and with GLOB_NOMATCH where
// nothing matches, unless GLOB_NOCHECK makes the pattern the one name; the
// names added before a failure stay, as POSIX allows. The names are not
// modelled. globfree frees what glob allocated, as glibc's does.
int glob(const char *pattern, int flags, int (*errfunc)(const char *, int), glob_t *g) {
  if (!(flags & GLOB_DOOFFS))
    g->gl_offs = 0;
  if (!(flags & GLOB_APPEND)) {
    g->gl_pathc = 0;
    g->gl_pathv = NULL;
  }
  size_t n = __VERIFIER_nondet_ulong();
  int r = 0;
  if (__VERIFIER_nondet_int() &&
      ((errfunc && errfunc(pattern, __VERIFIER_nondet_int())) || (flags & GLOB_ERR)))
    r = GLOB_ABORTED;
  else if (!n && (flags & GLOB_NOCHECK))
    n = 1;
  else if (!n)
    r = GLOB_NOMATCH;
  if (!n && (g->gl_pathv || !(flags & GLOB_DOOFFS)))
    return r;
  size_t old = g->gl_offs + g->gl_pathc, max = SIZE_MAX / sizeof(char *);
  if (old >= max || n >= max - old)
    return GLOB_NOSPACE;
  char **v = g->gl_pathv ? realloc(g->gl_pathv, (old + n + 1) * sizeof *v)
                         : calloc(old + n + 1, sizeof *v);
  if (!v)
    return GLOB_NOSPACE;
  g->gl_pathv = v;
  v[old] = NULL;
  for (size_t i = 0; i < n; ++i) {
    size_t len = __VERIFIER_nondet_ulong();
    __VERIFIER_assume(len < PATH_MAX);
    char *s = malloc(len + 1);
    if (!s)
      return GLOB_NOSPACE;
    __vereda_havoc(s);
    s[len] = 0;
    v[old + i] = s;
    v[old + i + 1] = NULL;
    ++g->gl_pathc;
  }
  return r;
}

void globfree(glob_t *g) {
  if (!g->gl_pathv)
    return;
  for (size_t i = 0; i < g->gl_pathc; ++i)
    free(g->gl_pathv[g->gl_offs + i]);
  free(g->gl_pathv);
  g->gl_pathv = NULL;
}

// getaddrinfo (POSIX): a list of one or more addresses in *res, each in a
// new object that holds its socket address too, the first with a canonical
// name in an object of its own under AI_CANONNAME, as glibc allocates them
// for freeaddrinfo; or an EAI_ code (negative in glibc), EAI_NONAME where
// node and service are both null and EAI_BADFLAGS for AI_CANONNAME without
// a node. The fields are not modelled.
int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints,
                struct addrinfo **res) {
  if (!node && !service)
    return EAI_NONAME;
  int canon = hints && (hints->ai_flags & AI_CANONNAME);
  if (canon && !node)
    return EAI_BADFLAGS;
  int e = __VERIFIER_nondet_int();
  if (e) {
    __VERIFIER_assume(e < 0);
    return e;
  }
  struct addrinfo *head = NULL, **p = &head;
  do {
    struct addrinfo *a = malloc(sizeof *a + sizeof(struct sockaddr_storage));
    if (!a) {
      freeaddrinfo(head);
      return EAI_MEMORY;
    }
    __vereda_havoc(a);
    __VERIFIER_assume(a->ai_addrlen <= sizeof(struct sockaddr_storage));
    a->ai_addr = (struct sockaddr *)(a + 1);
    a->ai_canonname = NULL;
    a->ai_next = NULL;
    *p = a;
    p = &a->ai_next;
  } while (__VERIFIER_nondet_int());
  if (canon) {
    size_t len = __VERIFIER_nondet_ulong();
    __VERIFIER_assume(len < NI_MAXHOST);
    if (!(head->ai_canonname = malloc(len + 1))) {
      freeaddrinfo(head);
      return EAI_MEMORY;
    }
    __vereda_havoc(head->ai_canonname);
    head->ai_canonname[len] = 0;
  }
  *res = head;
  return 0;
}

void freeaddrinfo(struct addrinfo *ai) {
  while (ai) {
    struct addrinfo *p = ai;
    ai = ai->ai_next;
    free(p->ai_canonname);
    free(p);
  }
}

// Any value from 0 to RAND_MAX (C17 7.22.2.1), so srand needs no model.
int rand(void) {
  int r = __VERIFIER_nondet_int();
  __VERIFIER_assume(r >= 0 && r <= 2147483647); // glibc's RAND_MAX
  return r;
}

// C17 7.22.3.1: null for an alignment the implementation does not support,
// here one that is not a power of two (errno EINVAL, as glibc); C17 dropped
// C11's rule that n be a multiple of it. The object comes from malloc, so
// free takes it, at an address that is a multiple of align.
void *aligned_alloc(size_t align, size_t n) {
  if (!align || (align & (align - 1))) {
    errno = EINVAL;
    return NULL;
  }
  void *p = malloc(n);
  __VERIFIER_assume(((uintptr_t)p & (align - 1)) == 0);
  return p;
}

// POSIX: align must be a power of two and a multiple of sizeof(void *);
// there is no condition on n. *p is written only on success, and a null
// from malloc(0) is a success.
int posix_memalign(void **p, size_t align, size_t n) {
  if (!align || (align & (align - 1)) || align % sizeof(void *))
    return EINVAL;
  void *q = aligned_alloc(align, n);
  if (n && !q)
    return ENOMEM;
  *p = q;
  return 0;
}

// The obsolete memalign (glibc's <malloc.h>) does not fail on an alignment
// that is not a power of two: glibc takes 0 to 16 as malloc does and rounds
// a larger one up. Such an alignment is plain malloc here, which promises
// less alignment than glibc gives. valloc aligns to the page, 4096 bytes.
void *memalign(size_t align, size_t n) {
  return !align || (align & (align - 1)) ? malloc(n) : aligned_alloc(align, n);
}
void *valloc(size_t n) { return aligned_alloc(4096, n); }

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
// time, which needs no buffer of an element's size.
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

// C17 7.22.4.2: atexit registers f, called without arguments when the
// program ends normally; at least 32 registrations must succeed. glibc takes
// any number, here 32, and a 33rd fails. ir calls __vereda_atexit_run before
// each exit and at each return of the entry function (7.22.4.4p3, 5.1.2.2.3);
// it calls them last first, and one a handler registers runs next, as glibc.
static void (*atexit_handlers[32])(void);
static unsigned atexit_count;

int atexit(void (*f)(void)) {
  if (atexit_count == 32)
    return -1;
  atexit_handlers[atexit_count++] = f;
  return 0;
}

void __vereda_atexit_run(void) {
  while (atexit_count)
    atexit_handlers[--atexit_count]();
}
