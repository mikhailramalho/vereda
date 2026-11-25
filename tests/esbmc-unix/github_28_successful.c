#include <limits.h>

struct aws_string
{
  const unsigned long len;
  const unsigned char bytes[];
};

void *bounded_malloc(unsigned long size)
{
  __VERIFIER_assume(size <= ((18446744073709551615UL) >> (8 + 1)));
  return malloc(size);
}

static inline const unsigned char *
aws_string_bytes(const struct aws_string *str)
{
  return str->bytes;
}

int main()
{
  unsigned long len;
  __VERIFIER_assume(
    len < ((18446744073709551615UL) - 1 - sizeof(struct aws_string)));

  struct aws_string *str = bounded_malloc(sizeof(struct aws_string) + len + 1);
  *(unsigned long *)(&str->len) = len;
  *(unsigned char *)&str->bytes[len] = '\0';

  __VERIFIER_assert(aws_string_bytes(str) == str->bytes); // This should never fail

  __VERIFIER_assert(
    ((((str->len + 1)) == 0) || ((&str->bytes[0])))); // This should never fail
  __VERIFIER_assert(str->bytes[str->len] == 0); // This should never fail
}
// Imported from ESBMC regression/esbmc-unix/github_28_successful
// Needs: --state-hashing --context-bound 2 --k-induction --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --state-hashing --no-align-check --k-step 2  --unlimited-k-steps --context-bound 2 --64 --k-induction --max-inductive-step 3 --no-pointer-check --no-bounds-check -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
