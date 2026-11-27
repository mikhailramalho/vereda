#include <pthread.h>
#include <assert.h>

int __VERIFIER_nondet_int();

int g = 0;

struct S
{
  int *q;
  int *p;
};

struct S arr[2] = {{0, &g}, {0, &g}};
int **pp;

// R32 through the struct arm: a symbolic index unsets the offset on the array,
// and the descent then has to consider every member of the element struct
// rather than the one an offset would have named. The pointer is declared
// *second* so that a descent stopping at the first member misses it -- with the
// pointer first, _symbolic_offset's array arm alone would carry this test.
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
  pp = &arr[i].p;
  pthread_create(&t, 0, writer, 0);
  g = 2;
  int seen = g;
  pthread_join(t, 0);
  assert(seen == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/mpor_aggregate_ptr_race_symbolic_struct_member
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
