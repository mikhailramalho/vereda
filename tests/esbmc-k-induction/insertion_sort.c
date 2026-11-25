
int main()
{
  unsigned int SIZE;
  int i, j, k, key;
  int v[SIZE];

  for (j = 1; j < SIZE; j++)
  {
    key = v[j];
    i = j - 1;

    while ((i >= 0) && (v[i] > key))
    {
      v[i + 1] = v[i];
      i = i - 1;
    }
    v[i + 1] = key;
  }

  for (k = 1; k < SIZE; k++)
    assert(v[k - 1] <= v[k]);
  return 0;
}
// Imported from ESBMC regression/k-induction/insertion_sort
// Needs: --k-induction
// RUN: --k-induction --32
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
