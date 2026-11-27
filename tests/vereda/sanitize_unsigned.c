// RUN: --unsigned-overflow-check
// EXPECT: ^Violated property:\n  file \S*sanitize_unsigned\.c line 9 column 35 function main\n  arithmetic overflow on add$
// EXPECT: ^VERIFICATION FAILED$
// --unsigned-overflow-check, through clang's
// -fsanitize=unsigned-integer-overflow: unsigned wrap-around is a failure.
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  return __VERIFIER_nondet_uint() + 1u > 0;
}
