// An endless loop that calls a function is unrolled, and the bound fails
// its unwinding assertion.
// RUN: --unwind 3
// EXPECT: ^  FAILED       \[main\.unwind\.0\]  line 10  unwinding assertion loop 0$
// EXPECT: ^VERIFICATION FAILED$
static int f(int x) { return x + 1; }

int main(void) {
  int x = 0;
  while (1)
    x = f(x);
}
