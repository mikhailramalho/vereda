int main()
{
    goto ERROR;
    
    if(0) {
ERROR:
        assert(0);
    }
    
    return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_if3
// EXPECT: ^VERIFICATION FAILED$
