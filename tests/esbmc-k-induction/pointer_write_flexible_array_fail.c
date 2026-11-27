// p->arr[0] lies past the declared struct, so havocking *p does not cover it.
#include <stdlib.h>
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

struct s
{
  int n;
  int arr[];
};

int main()
{
  struct s *p = malloc(sizeof(struct s) + 4 * sizeof(int));
  p->n = 0;
  p->arr[0] = 0;
  int i = 0;
  for (;;)
  {
    p->arr[0] = p->arr[0] + 1;
    i++;
    __VERIFIER_assert(p->arr[0] < 10);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_flexible_array_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
