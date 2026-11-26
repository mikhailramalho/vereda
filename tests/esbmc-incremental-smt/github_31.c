float MaxCost = 100;
unsigned char PanelQuant = 4;
float PanelData[4][13];
unsigned char __VERIFIER_nondet_uchar();

int Faux(float cost)
{
  unsigned char PanelChoice;
  PanelChoice = __VERIFIER_nondet_uchar();
  __VERIFIER_assume(PanelChoice <= 3);
  float Ap = PanelData[PanelChoice][0];
  return 0;
}

int main()
{
  float HintCost;
  for(HintCost = 0; HintCost <= MaxCost; HintCost++)
  {
    Faux(HintCost);
  }
  return 0;
}
// Imported from ESBMC regression/incremental-smt/github_31
// RUN: --no-bounds-check --no-pointer-check --no-div-by-zero-check --z3 --unwind 1
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
