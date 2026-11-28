// #pragma unroll 4 bounds the loop as ESBMC's --unwindset does (ESBMC's
// __ESBMC_unroll(3), which the importer writes so): its body runs three
// times, the three iterations this loop takes, and the bound holds (#74).
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
int a[8];

int main(void) {
  #pragma unroll 4
  for (int i = 0; i < 3; i++)
    a[i] = i + 1;
  __VERIFIER_assert(a[2] == 3);
  return 0;
}
