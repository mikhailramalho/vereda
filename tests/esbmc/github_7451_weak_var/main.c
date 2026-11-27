int global = 1234;
// Imported from ESBMC regression/esbmc/github_7451_weak_var
// RUN: -DEXPECTED=1234 --no-incremental-bmc
// FILES: weak.c
// EXPECT: ^VERIFICATION SUCCESSFUL$
