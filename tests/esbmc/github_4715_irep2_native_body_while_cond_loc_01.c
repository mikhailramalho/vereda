// W1-loc spike Phase C (esbmc/esbmc#4715): a `while` whose condition is itself
// side-effecting (`t--`) is lowered by generate_conditional_branch, which reads
// the location for each instruction it emits off the operand being lowered
// rather than off the statement. IREP2 value expressions carry no location, so
// the back-migrated condition arrived unlocated and those instructions came out
// with no location at all under native conversion, while the legacy path had
// them stamped by restore_value_locations. countdown() below is deliberately
// free of assignment statements so that it converts natively end to end on the
// current supported-kind set -- adding one would make the whole function fall
// back to goto_convert_rec and stop exercising the native path.
void countdown(unsigned t)
{
  while (t--)
  {
  }
}

int main(void)
{
  countdown(3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4715_irep2_native_body_while_cond_loc_01
// Needs: --goto-functions-only
// RUN: --goto-functions-only
// MODE: FUTURE
// EXPECT: github_4715_irep2_native_body_while_cond_loc_01\.c line 13 column 3 function countdown\n\s+ASSIGN tmp\$1=t;
// EXPECT: github_4715_irep2_native_body_while_cond_loc_01\.c line 13 column 10 function countdown\n\s+ASSIGN t=t - 1;
// Vereda: verdict none
