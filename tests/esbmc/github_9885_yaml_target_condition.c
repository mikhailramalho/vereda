#include <stdlib.h>

int main(void)
{
  char *p = malloc(10);
  if (!p)
    return 0;
  free(p);
  if (p[2] == 'a')
    return 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9885_yaml_target_condition
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: type: "target"\n.*\n.*\n.*\n\s*line: 9\n\s*column: 7$
// Vereda: verdict none
