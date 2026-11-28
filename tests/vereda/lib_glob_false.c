// Each name glob stores is a new object, which globfree frees: freeing the
// first name before globfree is a double free. And glob may match more
// than one name (#503).
// RUN: --unwind 3 --no-unwinding-assertions --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.assertion\.1\]
// EXPECT: ^  FAILED +\[main\.invalidated-dynamic-object-freed\.1\]
#include <assert.h>
#include <glob.h>
#include <stdlib.h>

int main(void) {
  glob_t g;
  if (glob("*", 0, NULL, &g))
    return 0;
  assert(g.gl_pathc == 1);
  free(g.gl_pathv[0]);
  globfree(&g);
  return 0;
}
