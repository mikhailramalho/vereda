// Stage walker: the trace, one step per source-level event in walk order.
// The witness stage reads it back under the solver's model.
#pragma once
#include "encode/terms.h"
#include "ir/debug.h"
#include <cstdint>
#include <deque>
#include <llvm/ADT/Hashing.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace vereda::walker {

struct Step {
  // Call: the value a bodyless __VERIFIER_nondet_ function returned, for
  // the witness. Branch: a branch that decides a source condition (see
  // ir::branchSite) taken one way, for the witness.
  enum Kind : uint8_t { Assignment, Assumption, Check, Call, Branch } kind;
  bool isUnsigned = false; // from the debug type; bitvectors carry no sign
  bool isFloat = false;    // a float or double, printed from its IEEE bits
  // Interned indices into Trace's location and string tables: a long unrolled
  // loop repeats the same few source locations and variable names hundreds of
  // thousands of times.
  uint32_t loc = 0;
  uint32_t text = 0;
  uint32_t check = 0; // for Check, the index of the claim in the checks
  // Path condition at the step; for Check, the violation; for Branch, the
  // path condition with the branch taken.
  encode::Term guard;
  encode::Term value; // assignments and calls only
  // A store into a named variable kept in memory: the variable's debug type
  // and the byte offset written, from which the witness spells "a[2].x".
  const llvm::DIType *type = nullptr;
  encode::Term offset;
};

class Trace {
public:
  void add(Step::Kind kind, encode::Term guard, const ir::SourceLoc &loc, llvm::StringRef text,
           encode::Term value = {}, bool isUnsigned = false, unsigned check = 0,
           const llvm::DIType *type = nullptr, encode::Term offset = {}, bool isFloat = false) {
    steps.push_back({kind, isUnsigned, isFloat, internLoc(loc), internText(text), check, guard,
                     value, type, offset});
  }

  const ir::SourceLoc &loc(const Step &s) const { return locs[s.loc]; }
  const std::string &text(const Step &s) const { return texts[s.text]; }

  auto begin() const { return steps.begin(); }
  auto end() const { return steps.end(); }
  size_t size() const { return steps.size(); }
  void truncate(size_t n) { steps.resize(n); }
  bool empty() const { return steps.empty(); }

private:
  struct LocEq {
    bool operator()(const ir::SourceLoc &a, const ir::SourceLoc &b) const {
      return a.line == b.line && a.col == b.col && a.file == b.file && a.function == b.function;
    }
  };
  struct LocHash {
    size_t operator()(const ir::SourceLoc &l) const {
      return llvm::hash_combine(l.line, l.col, llvm::StringRef(l.file),
                                llvm::StringRef(l.function));
    }
  };

  uint32_t internLoc(const ir::SourceLoc &l) {
    auto [it, fresh] = locIds.try_emplace(l, static_cast<uint32_t>(locs.size()));
    if (fresh)
      locs.push_back(l);
    return it->second;
  }

  uint32_t internText(llvm::StringRef s) {
    auto [it, fresh] = textIds.try_emplace(s, static_cast<uint32_t>(texts.size()));
    if (fresh)
      texts.emplace_back(s);
    return it->second;
  }

  std::deque<Step> steps;
  std::vector<ir::SourceLoc> locs;
  std::unordered_map<ir::SourceLoc, uint32_t, LocHash, LocEq> locIds;
  std::vector<std::string> texts;
  llvm::StringMap<uint32_t> textIds;
};

} // namespace vereda::walker
