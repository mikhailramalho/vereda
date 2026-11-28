// Explicit per-loop bounds select plain BMC, including through a property file.
// RUN: --no-summarize-array-loops --unwindsetname main:0:1
// RUN: --no-summarize-array-loops --unwindsetname main:0:1 --property-file %S/properties/unreach-call.prp
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: unwinding assertion loop 0
// EXPECT-NOT: Checking bound k =
int main(void) {
  int i = 0;
  while (i < 3)
    ++i;
  return 0;
}
