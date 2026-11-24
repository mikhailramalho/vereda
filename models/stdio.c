// Models of <stdio.h> streams (C17 7.21). A bodyless call gives the objects
// it gets pointers to nondet contents, so a stream must be an object of its
// own: a pointer from a bodyless fopen could point anywhere, and such a
// call refuses it. fmemopen and open_memstream stay bodyless, as their
// streams write to the program's own memory.
#include <stdio.h>

int __VERIFIER_nondet_int(void);

static FILE in, out, err, file;
FILE *stdin = &in, *stdout = &out, *stderr = &err;

// Every stream opened is the same object, or null when opening fails.
FILE *fopen(const char *path, const char *mode) { return __VERIFIER_nondet_int() ? &file : NULL; }
FILE *fdopen(int fd, const char *mode) { return __VERIFIER_nondet_int() ? &file : NULL; }
FILE *tmpfile(void) { return __VERIFIER_nondet_int() ? &file : NULL; }
FILE *freopen(const char *path, const char *mode, FILE *s) {
  return __VERIFIER_nondet_int() ? s : NULL;
}
