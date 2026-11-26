int Foo(int F_a, int F_b)
{
  int F_res = F_a + F_b;
  return F_res;
}

int main()
{
  int M_x, M_y, M_z;
  M_z = Foo(M_x, M_y);
  __VERIFIER_assert(M_z == M_x + M_y + 1); // result
}
// Imported from ESBMC regression/esbmc/function_return_location
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: State [0-9]+ file \S+function_return_location\.c line 10 column 9 function main thread 0\n  M_z =
// EXPECT: (?s)\A(?!.*line 4 column 3 function Foo.*\n.*\n.*M_z =)
// Vereda: unmet EXPECT: State [0-9]+ file \S+function_return_location\.c line 10 column 9 function main thread 0\n-+\n  M_z =
// Vereda: verdict FAILED
// MODE: FUTURE
// RUN: --no-div-by-zero-check
