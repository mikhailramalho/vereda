int my_assume(int n)
{
_EXIT: goto _EXIT;
}

int main() {
        int c = __VERIFIER_nondet_int();
        if(!c) my_assume(c);

        for(int i = 0; i < 5; i++);

        __VERIFIER_assert(0);
}
// Imported from ESBMC regression/esbmc/github_1411_false
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
