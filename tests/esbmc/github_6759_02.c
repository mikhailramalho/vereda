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
// Imported from ESBMC regression/esbmc/github_6759_02
// Needs: --no-irep2-native-body --goto-functions-only
// RUN: --no-irep2-native-body --goto-functions-only
// MODE: FUTURE
// EXPECT: main \(c:@F@main\):\n[\s\S]*?// \d+ file [^\n]*github_6759_02\.c line 10 column 3 function main\n\s+FUNCTION_CALL:  f\(3\)
// Vereda: verdict none
