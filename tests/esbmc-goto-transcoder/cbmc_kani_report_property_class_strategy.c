int main() { return 0; }
// Imported from ESBMC regression/goto-transcoder/cbmc_kani_report_property_class_strategy
// Needs: --report-property-class cover --loop-invariant
// RUN: --report-property-class cover --loop-invariant
// MODE: FUTURE
// EXPECT: ^ERROR: --report-property-class needs a single BMC run, not a verification strategy$
// Vereda: verdict none
