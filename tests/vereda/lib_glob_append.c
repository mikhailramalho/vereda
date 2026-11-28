// With GLOB_APPEND glob keeps the names of the earlier call, after the
// gl_offs null pointers, and globfree frees them all (#503).
// RUN: --unwind 2 --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <assert.h>
#include <glob.h>

int main(void) {
  glob_t g;
  g.gl_offs = 1;
  if (glob("*.c", GLOB_DOOFFS, 0, &g))
    return 0;
  char *first = g.gl_pathv[1];
  glob("*.h", GLOB_DOOFFS | GLOB_APPEND, 0, &g);
  assert(g.gl_pathc >= 1 && !g.gl_pathv[0] && g.gl_pathv[1] == first);
  assert(!g.gl_pathv[1 + g.gl_pathc]);
  globfree(&g);
  return 0;
}
