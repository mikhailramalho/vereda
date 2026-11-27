//#include<assert.h>

int main()
{
  int b=56;

  b;
  if(b) b;

  int c= ({int y; int z;
        if (y > 0) z = 3;
        else z = -3;
        z;});

  assert(c==3 || c==-3);
  return 0.0f;
}
// Imported from ESBMC regression/llvm/stmtexpr
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
