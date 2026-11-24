// Models of <stdio.h> streams (C17 7.21). A bodyless call gives the objects
// it gets pointers to nondet contents, so a stream must be an object of its
// own: a pointer from a bodyless fopen could point anywhere, and such a
// call refuses it. fmemopen and open_memstream stay bodyless, as their
// streams write to the program's own memory.
#include <errno.h>
#include <stdio.h>
#include <sys/uio.h>

int __VERIFIER_nondet_int(void);
long __VERIFIER_nondet_long(void);
unsigned long __VERIFIER_nondet_ulong(void);
void __VERIFIER_assume(int);
void *malloc(size_t);
// Gives the whole object p points into nondet contents (the walker does).
void __vereda_havoc(void *p);

// The standard streams are declared, not defined, so their contents are
// unknown: zeroed ones would put their bytes in memory as stores that a
// read of a stream pointer has to walk past.
extern FILE __vereda_stdin, __vereda_stdout, __vereda_stderr;
FILE *stdin = &__vereda_stdin, *stdout = &__vereda_stdout, *stderr = &__vereda_stderr;

// Each stream opened is a new object, or null when opening fails.
static FILE *stream(void) { return __VERIFIER_nondet_int() ? malloc(sizeof(FILE)) : NULL; }
FILE *fopen(const char *path, const char *mode) { return stream(); }
FILE *fdopen(int fd, const char *mode) { return stream(); }
FILE *tmpfile(void) { return stream(); }
FILE *freopen(const char *path, const char *mode, FILE *s) {
  return __VERIFIER_nondet_int() ? s : NULL;
}

// Under _FILE_OFFSET_BITS=64 glibc's <stdio.h> renames the three above to
// their 64 names. It renames fseeko, ftello, fgetpos and fsetpos too, which
// have no model either way.
FILE *fopen64(const char *path, const char *mode) { return stream(); }
FILE *tmpfile64(void) { return stream(); }
FILE *freopen64(const char *path, const char *mode, FILE *s) {
  return __VERIFIER_nondet_int() ? s : NULL;
}

// getdelim and getline (POSIX): a line of len characters and a NUL in
// *lineptr, which becomes a new object when it is null or holds no more than
// len bytes; glibc starts with 120. The characters are not modelled: the
// object they go to gets nondet contents, and so does the old one when it
// grows, as glibc fills it first. A read that finds the end of the stream or
// an error first returns -1. errno may change at any call (POSIX 2.3).
ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream) {
  errno = __VERIFIER_nondet_int();
  if (!*lineptr || !*n) {
    *n = 120;
    if (!(*lineptr = malloc(*n)))
      return -1;
  }
  size_t len = __VERIFIER_nondet_ulong();
  if (!len)
    return -1;
  __vereda_havoc(*lineptr);
  if (len >= *n) { // a new object, whose contents malloc leaves nondet
    size_t m = __VERIFIER_nondet_ulong();
    __VERIFIER_assume(m > len);
    char *p = malloc(m);
    if (!p)
      return -1;
    *lineptr = p;
    *n = m;
  }
  (*lineptr)[len] = 0;
  return len;
}

ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
  return getdelim(lineptr, n, '\n', stream);
}

// readv (POSIX): reads into the buffers of iov in turn. The bytes are not
// modelled: each buffer's object gets nondet contents. Linux refuses more
// than UIO_MAXIOV buffers, which also bounds the loop.
ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
  errno = __VERIFIER_nondet_int();
  if (iovcnt < 0 || iovcnt > UIO_MAXIOV)
    return -1;
  for (int i = 0; i < iovcnt; ++i)
    __vereda_havoc(iov[i].iov_base);
  ssize_t r = __VERIFIER_nondet_long();
  __VERIFIER_assume(r >= -1);
  return r;
}
