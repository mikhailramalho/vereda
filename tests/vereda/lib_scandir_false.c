// Each entry scandir stores is a new object, so freeing the first one
// twice is a double free wherever it found one (#503).
// RUN: --unwind 3 --no-unwinding-assertions
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.invalidated-dynamic-object-freed\.3\]
#include <dirent.h>
#include <stdlib.h>

int main(void) {
  struct dirent **v;
  int n = scandir(".", &v, NULL, NULL);
  if (n < 1)
    return 0;
  free(v[0]);
  free(v[0]);
  return 0;
}
