/* The declaration main() sees has an empty parameter list, so the call carries
   no operand for the parameter the definition declares. */
void f();

int main(void)
{
  f();
  return 0;
}

void f(int a)
{
}
// Imported from ESBMC regression/esbmc/interval_call_arity_mismatch
// Needs: --interval-analysis
// RUN: --std c17 --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
