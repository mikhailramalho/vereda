#include <pthread.h>
#include <assert.h>

int __VERIFIER_nondet_int();

int g = 0;

union U
{
  long pad;
  int *p;
};

union U u[2] = {{.p = &g}, {.p = &g}};
int **pp;

// R32 through the union arm: an unset offset makes the descent consider every
// member, which for a union is the same over-approximation the constant route
// already makes -- an offset cannot say which member is live either. `long pad`
// is rejected by the exact type match, so only `.p` yields a path. Pairs with
// _symbolic_struct_member, which is the struct-shaped walk.
void *writer(void *arg)
{
  (void)arg;
  **pp = 1;
  return 0;
}

int main(void)
{
  pthread_t t;
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  pp = &u[i].p;
  pthread_create(&t, 0, writer, 0);
  g = 2;
  int seen = g;
  pthread_join(t, 0);
  assert(seen == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/mpor_aggregate_ptr_race_symbolic_union
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
