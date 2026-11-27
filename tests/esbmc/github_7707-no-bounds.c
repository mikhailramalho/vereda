#include <stdint.h>

/* The size cap in object_base_alignment() is justified by the bounds check
 * running first, so the access width never exceeds the object. --no-bounds-check
 * voids that premise: the base of a one-byte object really is unconstrained, and
 * the misalignment is then the only thing left to report. */

int main(void)
{
  char c = 0;
  uint64_t *p = (uint64_t *)&c;
  uint64_t z = *p;
  (void)z;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7707-no-bounds
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*\[main\.assertion\.1\].*dereference failure: Incorrect alignment when accessing data object.*$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
