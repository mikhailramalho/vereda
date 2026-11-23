int __VERIFIER_nondet_int();

// The pointer under a dereference and the index of an array store are values,
// so GCSE may still replace them inside an assignment target; storing to
// `a[k + 1]` does not invalidate `k + 1`.
int main()
{
  int arr[3] = {0, 0, 0};
  int k = __VERIFIER_nondet_int();
  int *p = arr;
  int x = *(p + k);
  *(p + k) = 5;

  int a[4] = {0, 0, 0, 0};
  int y = a[k + 1];
  a[k + 1] = 7;
  a[k + 1] = 8;
}
// Imported from ESBMC regression/esbmc/gcse_lvalue_value_operands
// Needs: --gcse --goto-functions-only
// RUN: --gcse --goto-functions-only
// MODE: FUTURE
// EXPECT: ASSIGN \*cse_symbol\$\d+=5;
// EXPECT: ASSIGN a\[cse_symbol\$\d+\]=7;(\n\s*(//|ASSERT)[^\n]*)*\n\s*ASSIGN a\[cse_symbol\$\d+\]=8;
// Vereda: verdict none
