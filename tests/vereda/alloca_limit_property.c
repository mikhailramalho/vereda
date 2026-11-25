// RUN: --property-file %S/properties/unreach-call.prp
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^TRUE$
// EXPECT-NOT: size exceeds PTRDIFF_MAX
extern void reach_error(void);
int main(void) {
  volatile char *p = __builtin_alloca(-1);
  *p = 1;
  reach_error();
}
