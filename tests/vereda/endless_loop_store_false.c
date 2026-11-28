// An endless loop that stores to a global (volatile: main keeps no copy of
// it) is unrolled, and the bound fails its unwinding assertion.
// RUN: --unwind 3
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 9  unwinding assertion loop 0$
// EXPECT: ^VERIFICATION FAILED$
volatile int g;

int main(void) {
  for (;;)
    g++;
}
