// With an SV-COMP property file and incremental BMC (the default), vereda.py
// raises the bound without limit, as ESBMC's SV-COMP runs do: this bug needs
// 400 iterations, past --max-k-step's default of 384 (#84). Under
// --no-incremental-bmc it adds no k-step flag, which would be an error.
// RUN: --property-file %S/properties/unreach-call.prp
// RUN: --no-incremental-bmc --property-file %S/properties/unreach-call.prp
// EXPECT: ^FALSE_REACH$
extern void reach_error(void);
int main(void) {
  int i = 0;
  while (i < 400)
    i++;
  if (i == 400)
    reach_error();
  return 0;
}
