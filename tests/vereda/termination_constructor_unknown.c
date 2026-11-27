// RUN: --property-file %S/properties/termination.prp
// EXPECT: ^VERIFICATION UNKNOWN: constructors or destructors$
// EXPECT: ^UNKNOWN$
// The loader runs what .init_array lists before main, out of the walk's sight.
static void forever(void) {
  for (;;)
    ;
}

__attribute__((used, section(".init_array"))) static void (*run)(void) = forever;

int main(void) { return 0; }
