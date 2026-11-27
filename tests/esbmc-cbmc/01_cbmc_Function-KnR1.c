struct X
{
  int i;
};

f(a, b, c, d)
int a;
float b;
struct X c;
int d;
{
  return 10;
}

struct X g(a, b)
float a, b;
{
}

static void
sm_abort_defaulthandler(filename, lineno, msg)
const char *filename;
int lineno;
const char *msg;
{
}

int main()
{
  struct X x;
  
  assert(f(0, 0, x, 0)==10);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function-KnR1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
