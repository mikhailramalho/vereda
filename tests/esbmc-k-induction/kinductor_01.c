int __VERIFIER_nondet_int();

int rotate(int N)
{
  int a, b, c, x, i, temp;
  a = 1;
  b = 2;
  c = 3;
  x = 0;

  for(i = 0; i < N; i++)
  {
    //__CPROVER_k_induction_hint(3,3,0,0);
    assert(a != b);
    temp = a;
    a = b;
    b = c;
    c = temp;
  }

  assert(x == 0);

}

int main()
{
  rotate(__VERIFIER_nondet_int());  
  return 0;
}
// Imported from ESBMC regression/k-induction/kinductor_01
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
