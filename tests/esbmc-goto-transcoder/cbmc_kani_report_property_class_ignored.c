int main() { return 0; }
// Imported from ESBMC regression/goto-transcoder/cbmc_kani_report_property_class_ignored
// Needs: --report-property-class cover --ignore-property-class cover
// RUN: --report-property-class cover --ignore-property-class cover
// MODE: FUTURE
// EXPECT: ^ERROR: cover is both ignored and reported$
// Vereda: verdict none
