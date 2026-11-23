unsigned ext(unsigned);
int main(void) { unsigned r = ext(1); return 0; }
// Imported from ESBMC regression/goto2lean/unsupported_entry_without_body
// Needs: --goto2lean
// RUN: --goto2lean --function ext
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: entry function ext not found$
// Vereda: verdict none
