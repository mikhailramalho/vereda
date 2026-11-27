void assert(_Bool a) {}

int main()
{
  assert(0);
  return 0;
}

// Imported from ESBMC regression/esbmc/github_371_8
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
