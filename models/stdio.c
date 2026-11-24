// Models of <stdio.h> streams (C17 7.21). A bodyless call gives the objects
// it gets pointers to nondet contents, so a stream must be an object of its
// own: a pointer from a bodyless fopen could point anywhere, and such a
// call refuses it. fmemopen and open_memstream stay bodyless, as their
// streams write to the program's own memory.
#include <stdio.h>

int __VERIFIER_nondet_int(void);
void *malloc(size_t);

static FILE in, out, err;
FILE *stdin = &in, *stdout = &out, *stderr = &err;

// Each stream opened is a new object, or null when opening fails.
static FILE *stream(void) { return __VERIFIER_nondet_int() ? malloc(sizeof(FILE)) : NULL; }
FILE *fopen(const char *path, const char *mode) { return stream(); }
FILE *fdopen(int fd, const char *mode) { return stream(); }
FILE *tmpfile(void) { return stream(); }
FILE *freopen(const char *path, const char *mode, FILE *s) {
  return __VERIFIER_nondet_int() ? s : NULL;
}
