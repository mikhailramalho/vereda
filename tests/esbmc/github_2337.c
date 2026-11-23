int a, b;
void main() { -(a < b); }
// Imported from ESBMC regression/esbmc/github_2337
// RUN: --overflow-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
