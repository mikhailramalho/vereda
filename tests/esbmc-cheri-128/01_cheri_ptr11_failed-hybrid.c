#include <stdint.h>
#include <cheri/cheric.h>

int __VERIFIER_nondet_int();

void* foo(void *a) {
  return ((void *)((intptr_t)a & (intptr_t)~1));
}

int main() {
  int a = __VERIFIER_nondet_int() % 10;
  int *__capability cap_ptr = cheri_ptr(&a, sizeof(a));
  assert(*cap_ptr < 9);
  return 0;
}
// Imported from ESBMC regression/cheri-128/01_cheri_ptr11_failed-hybrid
// Needs: --cheri hybrid --big-endian
// RUN: --cheri hybrid --big-endian
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
