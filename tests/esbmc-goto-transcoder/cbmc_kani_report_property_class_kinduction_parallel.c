int main() { return 0; }
// Imported from ESBMC regression/goto-transcoder/cbmc_kani_report_property_class_kinduction_parallel
// Needs: --report-property-class cover --k-induction-parallel
// RUN: --report-property-class cover --k-induction-parallel
// MODE: FUTURE
// EXPECT: ^ERROR: --report-property-class does not support --k-induction-parallel$(?![\s\S]*does not support)(?![\s\S]*^VERIFICATION)
// Vereda: verdict none
