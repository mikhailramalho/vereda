// RUN: --property-file %S/properties/termination.prp
// EXPECT: ^VERIFICATION UNKNOWN: constructors, destructors or ifuncs$
// EXPECT: ^UNKNOWN$
// The loader runs what .init_array lists before main, out of the walk's
// sight. Nothing uses run, which ir must keep anyway: it used to drop it as
// unreachable from main, and the answer was TRUE.
static void forever(void) {
  for (;;)
    ;
}

__attribute__((section(".init_array"))) void (*run)(void) = forever;

int main(void) { return 0; }
