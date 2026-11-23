int main()
{
  int SIZE = 16;
  int vec[ SIZE ];
  int max = vec[0];
  int idx_v0;
  int idx;
  //------------------------------------------------------------
  for ( idx = 1; idx < SIZE; ++idx )
  { if ( vec[idx] >= max )
      max = vec[ idx ];
  }
  //------------------------------------------------------------
  __VERIFIER_assert(
    __ESBMC_forall(
      &idx_v0,
      !( 0 <= idx_v0 ) ||
      !( idx_v0 < SIZE ) ||
      ( vec[ idx_v0 ] <= max )
    )

); // end of loop assertion
  //------------------------------------------------------------
  return 0;
  //------------------------------------------------------------
}
// Imported from ESBMC regression/esbmc/no_standard_checks_vla
// Needs: --show-claims --no-standard-checks
// RUN: --show-claims --no-standard-checks
// MODE: FUTURE
// EXPECT: ^Claim 1:
// Vereda: verdict none
