// eca-rers2012 shape: while(1) reads validated input, calls an updater
// whose branches all transfer out via RETURN or exit(). The detector
// must find a (state, input) with NO branch firing — fall-through to
// `return -2` leaves state unchanged, demon picks that input forever,
// non-terminating. Period-1 fixpoint SMT discharge proves it.

int a = 1;
int b = 2;

extern int __VERIFIER_nondet_int(void);
extern void exit(int);

int update(int input)
{
    if (a == 99 && input == 1) {
        a = 0;
        return 0;
    }
    if (b == 99) {
        exit(0);
    }
    return -2;
}

int main(void)
{
    int sink = 0;
    while (1) {
        int input;
        input = __VERIFIER_nondet_int();
        if (input != 1 && input != 2)
            return -2;
        sink = update(input);
    }
    return sink;
}
// Imported from ESBMC regression/termination/lasso_eca_fixpoint
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --no-assertions --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2  --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^Recurrent set shows a non-terminating execution$
// Vereda: verdict none
