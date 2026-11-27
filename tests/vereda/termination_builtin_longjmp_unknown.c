// RUN: --property-file %S/properties/termination.prp
// EXPECT: ^VERIFICATION UNKNOWN: non-local jump at
// EXPECT: ^UNKNOWN$
// __builtin_longjmp jumps back to __builtin_setjmp forever, through
// intrinsics the CFG does not show as a loop. The answer used to be TRUE.
static void *buf[5];

int main(void) {
  __builtin_setjmp(buf);
  __builtin_longjmp(buf, 1);
}
