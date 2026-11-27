// realloc of a freed pointer is undefined even where the new allocation
// fails, as the size here always makes it (C17 7.22.3.5p3); the check in
// the C library's model is reported at the call. realloc(q, 0) frees q, so
// the free after it is a second one.
// RUN: --multi-property
// EXPECT: ^  FAILED       \[main\.invalidated-dynamic-object-freed\.1\]  line 16  dereference failure: invalidated dynamic object freed$
// EXPECT: ^  FAILED       \[main\.invalidated-dynamic-object-freed\.3\]  line 18  dereference failure: invalidated dynamic object freed$
// EXPECT: ^VERIFICATION FAILED$
#include <stdint.h>
#include <stdlib.h>

int main(void) {
  char *p = malloc(4);
  char *q = malloc(4);
  free(p);
  realloc(p, SIZE_MAX);
  realloc(q, 0);
  free(q);
  return 0;
}
