// EXPECT: ^VERIFICATION FAILED$
// getline writes the line into the buffer it is given when the buffer is
// large enough, which an alias sees (#114).
#include <stdio.h>
#include <stdlib.h>
extern void __VERIFIER_assert(int);
int main(void) {
  size_t n = 16;
  char *line = malloc(n);
  if (!line)
    return 0;
  line[0] = 'a';
  char *keep = line;
  getline(&line, &n, stdin);
  if (line == keep)
    __VERIFIER_assert(keep[0] == 'a');
  return 0;
}
