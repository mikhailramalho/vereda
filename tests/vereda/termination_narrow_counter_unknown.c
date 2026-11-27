// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^UNKNOWN$
// EXPECT-NOT: ^TRUE$
// An unsigned char is always below 300, so the loop never ends.
int main(void) {
  for (unsigned char c = 0; c < 300; c++)
    ;
  return 0;
}
