int main()
{
  int x = 1;
  if (x == 0)
    return 2;
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_6387_17
// Needs: --dead-code-check
// RUN: --dead-code-check
// MODE: FUTURE
// EXPECT: ^\[Dead code\]$
// EXPECT: dead code: unreachable branch
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: \A(?![\s\S]*COVERAGE ANALYSIS)[\s\S]*\Z
// EXPECT: \A(?![\s\S]*Coverage goals:)[\s\S]*\Z
// Vereda: verdict none
