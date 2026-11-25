// Stage memory: verified compact functions for fully written finite tables.
#pragma once
#include "encode/terms.h"
#include <span>

namespace vereda::memory {

// A function matching every element, or empty when no bounded pattern fits.
// The caller preserves the array's default outside [0, values.size()).
// Values have equal bit widths; the index can represent the table length.
encode::Terms::Def tableFunction(encode::Terms &t, std::span<const encode::Term> values);

} // namespace vereda::memory
