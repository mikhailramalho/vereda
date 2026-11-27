int a, b;
void main() { -(a < b); }
// Imported from ESBMC regression/esbmc/github_2337
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
