int sink;

void f(int x)
{
  sink = x;
}

int main(void)
{
  f(3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6759
// Needs: --goto-functions-only
// RUN: --goto-functions-only
// MODE: FUTURE
// EXPECT: main \(c:@F@main\):\n[\s\S]*?// \d+ file [^\n]*github_6759\.c line 10 column 3 function main\n\s+FUNCTION_CALL:  f\(3\)
// Vereda: verdict none
