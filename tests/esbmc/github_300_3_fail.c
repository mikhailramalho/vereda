typedef union{
    unsigned int a;
    unsigned int b;
} U1;

U1 u1 = {1};

struct S1 {
    U1 a;
};

int main(void) {
    struct S1 s1;
    struct S1 *s1p = &s1;

    s1p->a = u1;

    assert(s1.a.a == 2); // change to 1 for verification successful
}
// Imported from ESBMC regression/esbmc/github_300_3_fail
// EXPECT: ^VERIFICATION FAILED$
