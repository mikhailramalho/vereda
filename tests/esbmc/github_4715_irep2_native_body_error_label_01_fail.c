// convert_label turns a label matching --error-label into an ASSERT(false)
// carrying "error label" property/comment metadata, and makes that assertion
// the label's target so a goto lands on it. The native label arm declined the
// shape, taking the whole function to the round-trip (W1-loc,
// esbmc/esbmc#4715). Invisible to any census that does not replay test.desc
// flags, which is why it outlived the rest of the residue.
extern int nd(void);
int main(void)
{
  if (nd())
    goto ERR;
  return 0;
ERR:;
}
// Imported from ESBMC regression/esbmc/github_4715_irep2_native_body_error_label_01_fail
// Needs: --error-label ERR --goto-functions-only
// RUN: --error-label ERR --goto-functions-only
// MODE: FUTURE
// EXPECT: ASSERT 0 // error label
// EXPECT: // Labels: ERR
// Vereda: verdict none
