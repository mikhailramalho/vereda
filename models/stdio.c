// Models of <stdio.h> streams (C17 7.21). A bodyless call gives the objects
// it gets pointers to nondet contents, so a stream must be an object of its
// own: a pointer from a bodyless fopen could point anywhere, and such a
// call refuses it. fmemopen and open_memstream keep the memory their
// streams write to in the stream (see below).
#include <errno.h>
#include <stdio.h>
#include <sys/uio.h>

int __VERIFIER_nondet_int(void);
long __VERIFIER_nondet_long(void);
unsigned long __VERIFIER_nondet_ulong(void);
void __VERIFIER_assume(int);
void *malloc(size_t);
void free(void *);
char __VERIFIER_nondet_char(void);
// Gives the whole object p points into nondet contents (the walker does).
void __vereda_havoc(void *p);

// The bytes from p to the end of its object; the walker computes it.
size_t __vereda_object_size(const void *p);
// A failed check: pclose of a stream popen did not open.
void __vereda_pclose_not_popen(void);

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
// A popen stream is one byte larger than the others, which tells pclose
// apart from them: a stream's contents change at every bodyless call that
// gets it, its size does not.
FILE *popen(const char *command, const char *mode) {
  return __VERIFIER_nondet_int() ? malloc(sizeof(FILE) + 1) : NULL;
}
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
// *lineptr, which is allocated when null (glibc starts with 120 bytes) and
// grows when it holds no more than len bytes. The characters are not
// modelled: the object gets nondet contents before it grows, as glibc fills
// it first. It grows as models/stdlib.c's realloc does, into a new object
// with the old one freed, but without the copy, since the contents are
// nondet either way. A read that finds the end of the stream or an error
// first returns -1. errno may change at any call (POSIX 2.3).
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
  if (len >= *n) {
    size_t m = __VERIFIER_nondet_ulong();
    __VERIFIER_assume(m > len);
    char *p = malloc(m);
    if (!p)
      return -1;
    free(*lineptr);
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
// modelled: each buffer's object gets nondet contents, and its last byte
// is written when the input may fill it, so that a buffer smaller than its
// length is a write past its end. Linux refuses more than UIO_MAXIOV
// buffers, which also bounds the loop.
ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
  errno = __VERIFIER_nondet_int();
  if (iovcnt < 0 || iovcnt > UIO_MAXIOV)
    return -1;
  for (int i = 0; i < iovcnt; ++i) {
    __vereda_havoc(iov[i].iov_base);
    if (iov[i].iov_len && __VERIFIER_nondet_int())
      ((char *)iov[i].iov_base)[iov[i].iov_len - 1] = __VERIFIER_nondet_char();
  }
  ssize_t r = __VERIFIER_nondet_long();
  __VERIFIER_assume(r >= -1);
  return r;
}

// fmemopen (POSIX) makes a stream that writes to the program's buf, which
// gets nondet contents at the open, as for a bodyless call. The stream is
// an object of its own that keeps buf, so that the bodyless calls writing
// to it change buf too, and fclose frees it.
FILE *fmemopen(void *buf, size_t size, const char *mode) {
  if (buf)
    __vereda_havoc(buf);
  FILE *s = stream();
  if (s)
    s->_IO_buf_base = buf;
  return s;
}

// open_memstream (POSIX) writes into a buffer it allocates, which the
// program frees after fclose, and keeps *ptr and *size up to date at each
// flush. Here the buffer, of a nondet size, is allocated at the open and
// never moves: *ptr gets it there, and the stream keeps it and size, so
// that the bodyless calls writing to the stream change its contents and
// *size but not *ptr.
FILE *open_memstream(char **ptr, size_t *size) {
  FILE *s = stream();
  if (!s)
    return NULL;
  char *b = malloc(__VERIFIER_nondet_ulong());
  if (!b) {
    free(s);
    return NULL;
  }
  *ptr = b;
  *size = __VERIFIER_nondet_ulong();
  s->_IO_buf_base = b;
  s->_IO_buf_end = (char *)size;
  return s;
}

// C17 7.21.5.1: fclose flushes the stream and closes it whether or not that
// succeeds, so the object of an opened stream is freed and a second fclose
// is a double free. The standard streams are no allocation and stay.
int fclose(FILE *s) {
  fflush(s);
  if (s != &__vereda_stdin && s != &__vereda_stdout && s != &__vereda_stderr)
    free(s);
  return __VERIFIER_nondet_int() ? EOF : 0;
}

// POSIX pclose closes a popen stream and returns its command's exit status,
// or -1. A popen stream is an allocation as fopen's is, so fclose frees it
// too (POSIX leaves that unspecified), and closing it twice, by either, is a
// double free. pclose of any other stream is undefined, and reported.
int pclose(FILE *s) {
  if (__vereda_object_size(s) != sizeof(FILE) + 1)
    __vereda_pclose_not_popen();
  free(s);
  return __VERIFIER_nondet_int();
}
