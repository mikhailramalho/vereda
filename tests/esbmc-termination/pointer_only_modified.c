/* Pointer-only-modified loop (strrchr-style). The loop's only modified
 * variable is the pointer `t`. Pre-merge HEAD had a dedicated gate
 * (has_pointer_only_loop) disabling the inductive step for this shape;
 * the upstream k-induction deep-dive merge removed it on the claim
 * that the value-set assume made it unnecessary. The assume protects
 * pointer aliasing (post-havoc `t` stays inside its pre-havoc points-
 * to set) but does NOT constrain the pointed-to data — for loops
 * whose termination depends on what's read through the pointer (here,
 * the `*t == 0` exit condition), IS sees a state where `*t != 0` for
 * every k it unwinds and reports non-termination spuriously.
 *
 * goto_terminationt's loop_is_is_unreliable predicate now sets
 * disable-inductive-step for the whole program when it sees a loop
 * whose modified set is non-empty but consists entirely of pointer-
 * typed entries. The expected verdict here is therefore UNKNOWN
 * (FC can't unwind a nondet-length string, IS is disabled), not
 * the wrong VERIFICATION FAILED IS used to produce. */

extern int __VERIFIER_nondet_int(void);

int main()
{
  int in_len = __VERIFIER_nondet_int();
  if (in_len < 1)
    return 1;
  char *in = (char *)__builtin_alloca(in_len);
  in[in_len - 1] = 0;

  /* Modifies only the pointer `t`; `*t` is read, never written. */
  char *t = in;
  while (*t != 0)
    ++t;

  return 0;
}
// Imported from ESBMC regression/termination/pointer_only_modified
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --no-assertions --termination --max-inductive-step 3 REQUIRES arch32
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --floatbv --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
