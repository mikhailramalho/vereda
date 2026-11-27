// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^UNKNOWN$
// EXPECT-NOT: ^TRUE$
// A bodyless reach_error is an unknown function that returns, not the end
// of the run, so the walk goes on into the loop after it.
extern void reach_error(void);

int main(void) {
  reach_error();
  while (1)
    ;
}
