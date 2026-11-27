// The slice leaves loops whole: with the store into g, which nothing reads,
// gone, the loop would only count and be summarised, without the unwinding
// assertion that fails here.
// RUN: --unwind 5
// EXPECT: ^  FAILED       \[main\.unwind\.0\]  line 9  unwinding assertion loop 0$
// EXPECT: ^VERIFICATION FAILED$
int g;
int main(void) {
  for (int i = 0; i < 1000; i++)
    g = i;
  return 0;
}
