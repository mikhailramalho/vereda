// EXPECT: ^VERIFICATION SUCCESSFUL$
// realpath and canonicalize_file_name with no buffer, getcwd with none and
// get_current_dir_name allocate their result, which free takes (#358).
#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
int main(void) {
  free(realpath(".", NULL));
  free(canonicalize_file_name("."));
  free(getcwd(NULL, 0));
  char *b = getcwd(NULL, 64);
  if (b)
    b[63] = 0;
  free(b);
  free(get_current_dir_name());
  return 0;
}
