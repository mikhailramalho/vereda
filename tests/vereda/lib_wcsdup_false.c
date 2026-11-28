// wcsdup of a wide string whose first character is nondet holds three
// characters, but none where that character is 0; and the copy is a new
// object, which a second free frees twice (#503).
// RUN: --unwind 8 --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.assertion\.1\]
// EXPECT: ^  FAILED +\[main\.invalidated-dynamic-object-freed\.1\]
#include <stdlib.h>
#include <wchar.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

int main(void) {
  wchar_t s[4] = {__VERIFIER_nondet_int(), L'b', L'c', 0};
  wchar_t *d = wcsdup(s);
  if (!d)
    return 0;
  __VERIFIER_assert(wcslen(d) == 3);
  free(d);
  free(d);
  return 0;
}
