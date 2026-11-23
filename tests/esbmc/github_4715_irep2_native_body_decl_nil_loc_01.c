// W1-loc spike Phase C (esbmc/esbmc#4715): a compound literal declares a
// synthesised variable with no source location. convert_decl reads the
// companion ASSIGN's location through codet's *mutable* location() accessor,
// which materialises an empty (id "", non-nil) #location, while code_decl2t
// carries a properly nil one -- so the native path emitted a nil location where
// legacy emitted an empty-but-present one. The GOTO dump renders the two
// differently ("no location" vs blank), which is how this surfaced as an
// IREP2-native byte-identity divergence. The DECL keeps the nil location
// in both paths: convert_decl emits it before that mutable access happens.
int my_func(int stat_loc)
{
  return ((union {
           __typeof(stat_loc) __in;
           int __i;
         }){.__in = (stat_loc)})
    .__i;
}

int main(void)
{
  int x = 7;
  return my_func(x) - 7;
}
// Imported from ESBMC regression/esbmc/github_4715_irep2_native_body_decl_nil_loc_01
// Needs: --goto-functions-only
// RUN: --goto-functions-only
// MODE: FUTURE
// EXPECT: // \d+ no location\n\s+DECL union [^\n]*compound-literal\$1;\n\s+// \d+ file [^\n]*github_4715_irep2_native_body_decl_nil_loc_01\.c line 12 [^\n]*\n\s+ASSIGN
// Vereda: verdict none
