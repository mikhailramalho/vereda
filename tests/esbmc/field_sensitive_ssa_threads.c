#include <assert.h>
#include <pthread.h>

struct S
{
  int a, b;
};

void *worker(void *arg)
{
  return arg;
}

int main()
{
  pthread_t t;
  pthread_create(&t, 0, worker, 0);
  struct S s;
  s.a = 1;
  s.b = 2;
  assert(s.a + s.b == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_threads
// MODE: FUTURE
// EXPECT: ^WARNING: --field-sensitive-ssa is ignored: the program may create threads$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict UNKNOWN
