// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.free-non-dynamic-memory\.1\]
// EXPECT: ^  FAILED +\[main\.invalidated-dynamic-object-freed\.2\]
// EXPECT: ^  FAILED +\[main\.invalidated-dynamic-object-freed\.4\]
// getcwd writes into the buffer it gets and returns it: freeing that is
// freeing a local. get_current_dir_name and canonicalize_file_name return
// a new object, which a second free frees twice. Each only where the call
// succeeds (#358).
#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
int main(void) {
  char buf[32];
  free(getcwd(buf, sizeof buf));
  char *a = get_current_dir_name();
  free(a);
  free(a);
  char *b = canonicalize_file_name(".");
  free(b);
  free(b);
  return 0;
}
