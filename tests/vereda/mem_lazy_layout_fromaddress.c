// Globals passed only to non-capturing calls (like strcmp or assert) are not
// exposed, so an outside pointer cannot alias them; exposed globals are laid
// out on demand when fromAddress runs and remain aligned and non-zero.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <assert.h>
#include <stdint.h>
#include <string.h>

extern void __VERIFIER_assert(int);
extern uintptr_t __VERIFIER_nondet_ulong(void);

static char secret[4] = "abc";
static int exposed_var = 7;

int main(void) {
  assert(strcmp(secret, "abc") == 0);
  uintptr_t ae = (uintptr_t)&exposed_var;
  __VERIFIER_assert(ae != 0);
  __VERIFIER_assert((ae & (_Alignof(int) - 1)) == 0);

  uintptr_t a = __VERIFIER_nondet_ulong();
  char *p = (char *)a;
  if (a != 0)
    *p = 'z';
  __VERIFIER_assert(secret[0] == 'a');
  return 0;
}
