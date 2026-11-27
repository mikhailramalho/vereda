int main()
{
    float r[1][1];
    (*(&r))[0][0] = nondet();
    assert(!(*(&r))[0][0]);
    return 0;
}

// Imported from ESBMC regression/esbmc/github_879
// EXPECT: ^VERIFICATION FAILED$
