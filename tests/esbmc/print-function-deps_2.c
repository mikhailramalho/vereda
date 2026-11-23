void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

int main() {
  int x = 1, y = 2;
  swap(&x, &y);
  return 0;
}
// Imported from ESBMC regression/esbmc/print-function-deps_2
// Needs: --show-call-sites
// RUN: --show-call-sites
// MODE: FUTURE
// EXPECT: ^c:@F@main -> c:@F@swap\(&x, &y\) \[.*print\-function\-deps_2\.c:\d+\]$
// EXPECT: ^__ESBMC_main -> c:@F@main\(\) \[.*print\-function\-deps_2\.c:\d+\]$
// Vereda: verdict none
