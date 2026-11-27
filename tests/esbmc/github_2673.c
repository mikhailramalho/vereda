int magic = 5;

int main() {
  
  int *ptr = (int*) magic;
  *ptr = 20;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_2673
// EXPECT: dereference failure: invalid pointer
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: dereference failure: invalid pointer
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
