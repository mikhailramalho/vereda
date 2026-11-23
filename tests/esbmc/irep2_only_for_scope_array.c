/* The loop variable's DEAD must come after the loop that reads it; the
   for-init hoist is what puts it there. See docs/roadmap/scope-clang-c-irep2.md
   §100. */
int __VERIFIER_nondet_int(void);
char buf[11];

int main(void)
{
  for (int i = 0; i < 10; ++i)
    buf[i] = (char)__VERIFIER_nondet_int();

  buf[10] = 0;
  return buf[0];
}
// Imported from ESBMC regression/esbmc/irep2_only_for_scope_array
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^ *2: DEAD c:irep2_only_for_scope_array\.c@[0-9]*@F@main@i$
// Vereda: verdict none
