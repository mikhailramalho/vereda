// glob puts its names after gl_offs null pointers with GLOB_DOOFFS, ends
// gl_pathv with a null pointer whatever it returns, calls errfunc before
// it aborts, and gives a name with GLOB_NOCHECK; globfree frees what it
// allocated (#503).
// RUN: --unwind 3 --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <assert.h>
#include <glob.h>
#include <stddef.h>

static int errors;

static int stop(const char *path, int err) {
  ++errors;
  return 1;
}

int main(void) {
  glob_t g;
  g.gl_offs = 2;
  int r = glob("*.c", GLOB_DOOFFS, stop, &g);
  assert(r != 0 || g.gl_pathc > 0);
  assert(r != GLOB_NOMATCH || g.gl_pathc == 0);
  assert(r != GLOB_ABORTED || errors == 1);
  if (g.gl_pathv)
    assert(!g.gl_pathv[0] && !g.gl_pathv[1] && !g.gl_pathv[2 + g.gl_pathc]);
  globfree(&g);
  assert(!g.gl_pathv);

  r = glob("x", GLOB_NOCHECK, NULL, &g);
  assert(r != 0 || g.gl_pathc > 0);
  assert(r != GLOB_NOMATCH && r != GLOB_ABORTED);
  globfree(&g);
  return 0;
}
