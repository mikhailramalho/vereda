#include<assert.h>

int main() { 
  int i, n, sn=0;
  for(i=1; i<=n; i++)
    if (i<10)
      sn = sn + 2;
  assert(sn==n*2 || sn == 0);
}
// Imported from ESBMC regression/k-induction-parallel/sum01_bug
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
