// RUN: --property-file %S/properties/termination.prp
// EXPECT: ^VERIFICATION UNKNOWN: module-level assembly$
// EXPECT: ^UNKNOWN$
// Module-level assembly puts spin in .init_array, so the loader runs it
// before main; no global shows it, and the walk never sees it.
void spin(void) __attribute__((used));
void spin(void) { for (;;) __asm__ volatile(""); }
__asm__(".pushsection .init_array,\"aw\",@init_array\n.quad spin\n.popsection\n");
int main(void) { return 0; }
