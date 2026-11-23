// With an SV-COMP property file and --incremental-bmc, vereda.py raises the
// bound without limit, as ESBMC's SV-COMP runs do: this bug needs 60
// iterations, past --max-k-step's default of 50 (#84).
// RUN: --incremental-bmc --property-file %S/properties/unreach-call.prp
// EXPECT: ^FALSE_REACH$
extern void reach_error(void);
int main(void) {
  int i = 0;
  while (i < 60)
    i++;
  if (i == 60)
    reach_error();
  return 0;
}
