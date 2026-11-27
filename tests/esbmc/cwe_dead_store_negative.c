extern void use(int *);

// Neither store is a dead store: `a` is address-taken (may be read through the
// pointer) and `v` is volatile (the write is an observable side effect,
// C11 5.1.2.3). The analysis must report nothing here — the core soundness
// claim of excluding address-taken and volatile locals.
int main(void)
{
  int a = 1;
  use(&a);
  volatile int v;
  v = 7;
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_dead_store_negative
// Needs: --dead-store-check
// RUN: --dead-store-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: \A(?![\s\S]*dead store)[\s\S]*\Z
// Vereda: verdict none
