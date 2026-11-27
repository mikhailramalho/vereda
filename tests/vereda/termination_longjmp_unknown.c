// RUN: --property-file %S/properties/termination.prp
// EXPECT: ^VERIFICATION UNKNOWN: non-local jump at .*line 14 
// EXPECT: ^UNKNOWN$
// ir lowers only a longjmp given a jmp_buf and a value; this one, declared
// without a prototype, stays a bodyless call that jumps back to the setjmp
// forever. It used to count as a call that returns, and the answer was TRUE.
typedef long jmp_buf[50];
int _setjmp(jmp_buf);
void __longjmp_chk();
jmp_buf b;

int main(void) {
  _setjmp(b);
  __longjmp_chk(b);
  return 0;
}
