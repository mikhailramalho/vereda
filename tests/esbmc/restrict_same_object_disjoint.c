/* restrict allows two pointers into the same object as long as the accessed
   regions do not overlap. Writing distinct elements of one array must verify
   under --restrict-check (exercises the offset-range disjointness branch). */
void f(int *restrict a, int *restrict b)
{
  *a = 1;
  *b = 2;
}

int main(void)
{
  int arr[4] = {0};
  f(&arr[0], &arr[2]);
  return 0;
}
// Imported from ESBMC regression/esbmc/restrict_same_object_disjoint
// Needs: --restrict-check
// RUN: --restrict-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
