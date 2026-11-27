// --fresh-solver renews the instance --incremental-solver keeps.
// RUN: --fresh-solver --incremental-solver
// EXPECT: --fresh-solver conflicts with --incremental-solver
// EXPECT-NOT: VERIFICATION
int main(void) { return 0; }
