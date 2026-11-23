// #7792: goto_check dedups claims through std::set<expr2tc>, so the ordering
// must keep literals that differ only in the sign of zero apart.
int main(void)
{
  float f;
  int a = (int)(f + 0.0f) + (int)(f + -0.0f);
  return a;
}
// Imported from ESBMC regression/floats-regression/github_7792_claims
// Needs: --show-claims
// RUN: --overflow-check --show-claims
// MODE: FUTURE
// EXPECT: ^  isinf\(f\) \|\| !\(isinf\(IEEE_ADD\(f, 0\.000000\)\)\)$
// EXPECT: ^  isinf\(f\) \|\| !\(isinf\(IEEE_ADD\(f, -0\.000000f\)\)\)$
// EXPECT: ^  IEEE_ADD\(f, 0\.000000\) >= -2\.147484e\+9f && IEEE_ADD\(f, 0\.000000\) < 2\.147484e\+9f$
// EXPECT: ^  IEEE_ADD\(f, -0\.000000f\) >= -2\.147484e\+9f && IEEE_ADD\(f, -0\.000000f\) < 2\.147484e\+9f$
// Vereda: verdict none
