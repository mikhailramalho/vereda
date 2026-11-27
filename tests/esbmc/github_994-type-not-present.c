union U { unsigned raw : 32; struct { unsigned x : 16, y : 16; }; };

int f();

int main() { (union U)f(); }
// Imported from ESBMC regression/esbmc/github_994-type-not-present
// RUN: --no-incremental-bmc
// EXPECT: error: cast to union type from type( '.+')? not present in union
