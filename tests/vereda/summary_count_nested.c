// RUN: -m32 --unwind 200000
// EXPECT: ^  FAILED +\[main\.assertion\.1\]  line 11  unwinding assertion loop 1$
// EXPECT: ^VERIFICATION FAILED$
// loops/heavy-1.c without its store: the inner loop only counts, so each of
// the middle loop's 200000 iterations summarises it without walking an
// iteration of it (#370).
int main(void) {
  static char array[1024 * 1024] = {1};
  unsigned int i, j, k;
  for (i = 0; i < sizeof(array); i++)
    for (j = 0; j < sizeof(array); j++)
      for (k = 0; k < sizeof(array); k++)
        ;
  return array[100];
}
