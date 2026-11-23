#include <assert.h>
#include <stdatomic.h>

/* Passing counterpart of the dowhile_fail/switch_fail pair: the instance the
 * IREP2 pass declares must perform the same arithmetic wherever the call sits,
 * a do-while condition and a switch selector included. */
int main(void)
{
  atomic_int a;
  atomic_init(&a, 0);

  do
    ;
  while (atomic_fetch_add(&a, 1) < 2);

  switch (atomic_load(&a))
  {
  case 3:
    atomic_store(&a, 7);
    break;
  default:
    atomic_store(&a, 9);
    break;
  }

  assert(atomic_load(&a) == 7);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_polymorphic_builtin_stmt_kinds
// Needs: --clang-c-irep2-adjust-only --std c11
// RUN: --clang-c-irep2-adjust-only --std c11 --unwind 6
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
