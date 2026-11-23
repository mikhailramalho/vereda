/* main calls no spawn primitive: pthread_create is reached only by calling
 * through a function pointer. symex resolves such a call through the value set
 * (get_function_list in symex_function.cpp), so it can only reach a function
 * whose address is taken -- which is what the guard's worklist is seeded with.
 * Without that seeding this program reads as single-threaded and its loop is
 * cut. */
#include <pthread.h>

static void *body(void *arg)
{
  return 0;
}

static void spawn(void)
{
  pthread_t t;
  pthread_create(&t, 0, body, 0);
}

int main(void)
{
  void (*fp)(void) = spawn;
  unsigned int i = 0;
  unsigned int s = 0;
  while (i < 4)
  {
    s = s + 1;
    i = i + 1;
  }
  fp();
  __VERIFIER_assert(s == 4); // s == 4
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_threadptr
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 6
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^WARNING: --synthesise-loop-invariants: the program creates threads, and cutting a loop deletes the interleaving points its body carried\. Synthesising nothing rather than report a claim only violable through one of them as passed$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
