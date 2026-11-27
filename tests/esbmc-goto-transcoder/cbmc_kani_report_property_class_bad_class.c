int main() { return 0; }
// Imported from ESBMC regression/goto-transcoder/cbmc_kani_report_property_class_bad_class
// Needs: --report-property-class assertion
// RUN: --report-property-class assertion
// MODE: FUTURE
// EXPECT: ^ERROR: --report-property-class takes reachability_check or cover, not 'assertion'$
// Vereda: verdict none
