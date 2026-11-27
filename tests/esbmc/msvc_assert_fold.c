/* The folded MSVC assert holds where its condition does. Paired with
 * msvc_assert_fold_fail so the fold cannot pass by dropping the claim. */
void _wassert(const char *_Message, const char *_File, unsigned _Line);
#define ASSERT_MSVC(e) \
  (void)((!!(e)) || (_wassert(#e, __FILE__, (unsigned)__LINE__), 0))
int main(void)
{
  unsigned n;
  __VERIFIER_assume(n < 4);
  ASSERT_MSVC(n <= 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/msvc_assert_fold
// EXPECT: ^VERIFICATION SUCCESSFUL$
