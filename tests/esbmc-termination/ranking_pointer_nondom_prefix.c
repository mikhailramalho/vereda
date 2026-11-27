/* Soundness pin for the dominance-aware prefix scan.
 *
 * The pointer `p` is assigned either malloc (one branch) or NULL'd-into-
 * an-existing-pointer alias `q` (the other branch). At the loop head,
 * `*p` may refer to either cell depending on the branch taken, so the
 * deref-substitution's allocation-provenance reasoning must NOT trust
 * the malloc assignment as if it dominated the head.
 *
 * Concretely, the linear `scan_prefix_defs` we initially wrote would
 * record `p = malloc(...)` as the most-recent textual assignment and let
 * the substitution fire. The corrected dominance-aware walk takes the
 * single dominator path: at the prefix's if/else, both branches reach
 * the loop head (disjunctive merge), so the walk sets `has_invalid` and
 * `pointer_cell_identity` refuses. The ranking checker then sees the
 * deref as a memory touch and declines.
 *
 * Expected verdict: VERIFICATION UNKNOWN (control falls through to the
 * existing k-induction / forward-condition machinery). */

#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);

int main()
{
  int *p;
  int *q = malloc(sizeof(int));
  *q = 5;
  if (__VERIFIER_nondet_int())
    p = malloc(sizeof(int));
  else
    p = q; /* aliases q's cell */
  *p = __VERIFIER_nondet_int();
  while (*p >= 0)
    (*p)--;
  return 0;
}
// Imported from ESBMC regression/termination/ranking_pointer_nondom_prefix
// Needs: --force-realloc-success --termination --max-inductive-step 3
// RUN: --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --k-step 2 --vla-size-check=false -m32 --ub-pointer-check=false --no-assertions --termination --max-inductive-step 3
// EXPECT: ^VERIFICATION UNKNOWN$
// MODE: FUTURE
// Vereda: verdict none
