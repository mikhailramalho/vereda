// Models of <stdio.h> streams (C17 7.21). A bodyless call gives the objects
// it gets pointers to nondet contents, so a stream must be an object of its
// own: a pointer from a bodyless fopen could point anywhere, and such a
// call refuses it. fmemopen and open_memstream stay bodyless, as their
// streams write to the program's own memory.
#include <stdio.h>

int __VERIFIER_nondet_int(void);
void *malloc(size_t);

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
