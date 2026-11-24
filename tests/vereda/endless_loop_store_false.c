// An endless loop that stores to a global is unrolled, and the bound fails
// its unwinding assertion.
// RUN: --unwind 3
// EXPECT: ^  FAILED       \[main\.unwind\.0\]  line 9  unwinding assertion loop 0$
// EXPECT: ^VERIFICATION FAILED$
int g;

int main(void) {
  for (;;)
    g++;
}
