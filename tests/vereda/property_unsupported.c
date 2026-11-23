// RUN: --property-file %S/properties/valid-memsafety.prp
// EXPECT: ^vereda\.py: unsupported property in \S*valid-memsafety\.prp; supported: unreach-call, no-overflow$
// EXPECT-NOT: ^VERIFICATION
// Vereda has no memory-safety checks yet, so it refuses the property.
int main(void) { return 0; }
