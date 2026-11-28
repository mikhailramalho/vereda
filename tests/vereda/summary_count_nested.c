// RUN: -m32 --unwind 200000
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: unwinding assertion
// loops/heavy-1.c without its store: the inner loop only counts, and so,
// once it is summarised, do the two around it, so the outer loop is
// summarised without walking any of them (#370, #504).
int main(void) {
  static char array[1024 * 1024] = {1};
  unsigned int i, j, k;
  for (i = 0; i < sizeof(array); i++)
    for (j = 0; j < sizeof(array); j++)
      for (k = 0; k < sizeof(array); k++)
        ;
  return array[100];
}
