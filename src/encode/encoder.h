// Stage encode: LLVM values and instructions to camada terms. i1 values are
// Bool terms; every other integer is a bitvector of its width. Pointers are
// the memory stage's (object, offset) bitvectors. A struct of integers,
// pointers, floats and vectors is one bitvector with its fields' bits packed
// from the low bits up, so a pointer in a struct clang returns by value
// ({ptr, i32} on x86-64) keeps its object. A float in an
// IEEE format is a term of the solver's floating-point theory, which becomes
// its bits only where bits are needed: memory, bitcasts, vector elements and
// sign tests; a NaN's bits there are some NaN. With fp2bv a float is the
// bitvector of its bits everywhere, and an operation views it as camada's
// bit-vector encoded float, which propagates NaN payloads as hardware does.
// Operations round to nearest, ties to even, as C does by default, and on
// constant operands fold with APFloat under the same rounding, so a value
// computed from constants alone is a constant; a NaN result does not fold,
// since its bits are the encoding's. A vector of integers or
// floats is one bitvector with element 0 in the low bits, as memory lays it
// out; the scalarizer leaves vectors only where they cross calls.
#pragma once
#include "encode/terms.h"
#include "memory/memory.h"
#include "util/unsupported.h"
#include <camada/camada.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Operator.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <optional>
#include <string>

namespace vereda::encode {

using vereda::Unsupported;

class Encoder {
public:
  Encoder(Terms &t, memory::Memory &mem, bool fp2bv = false)
      : t(t), s(t.solver()), mem(mem),
        fpEncoding(fp2bv ? camada::FPEncoding::BV : camada::FPEncoding::Native) {
    mem.known = [this](const Term &x) { return known(facts, x); };
  }
  ~Encoder() { mem.known = {}; }
  camada::SMTSolver &solver() { return s; }
  Terms &terms() { return t; }
  unsigned symbolCount() const { return freshCount; }

  camada::SMTSortRef sort(llvm::Type *ty);
  // What the path being walked knows of a term from its assumptions and
  // branches: a term it equals (Terms::equalities), a bitvector constant or
  // a leaf such as a symbol, which value() returns in its place so that
  // what is built from it folds, and bounds (Terms::narrow), by which a
  // comparison of it folds as it is encoded. A term equal to a leaf keeps
  // no bounds of its own: the leaf's, at the end of the chain, are both
  // terms'. Keyed by the term's id; terms live as long as the solver.
  struct Fact {
    Term equal;
    std::optional<Terms::Bounds> bounds;
    bool operator==(const Fact &o) const = default;
  };
  // The walker keeps them per path, in a persistent map (cellmap.h) that
  // each arrival copies as a pointer.
  using Facts = memory::Trie<Fact>;
  Facts facts;
  static Facts::Key key(const Term &x) {
    return {reinterpret_cast<uintptr_t>(x.id()), 0};
  }
  // Adds to into what cond, which holds on its path, says of its terms.
  void learn(const Term &cond, Facts &into);
  // What two paths' facts on one term both say, if anything: for a join.
  static std::optional<Fact> join(const Fact &a, const Fact &b);
  // The term facts makes x equal to at the end of its chain, else x.
  static Term root(const Facts &facts, Term x);
  // The bounds facts gives x, if any.
  static std::optional<Terms::Bounds> known(const Facts &facts, const Term &x);
  // e as the path being walked sees it (Terms::assuming).
  Term assuming(Term e);
  // The term bound to v, with facts applied, or a constant. Undef and
  // poison are a fresh value per use.
  Term value(const llvm::Value *v);
  void bind(const llvm::Value *v, Term term) { env[v] = std::move(term); }
  // The term bound to v, or a null term.
  Term bound(const llvm::Value *v) const { return env.lookup(v); }
  // A suspended loop's bindings, before walking a suffix that may rebind them.
  using Bindings = llvm::DenseMap<const llvm::Value *, Term>;
  Bindings saveBindings() const { return env; }
  void restoreBindings(Bindings bindings) { env = std::move(bindings); }
  // A value of type ty as the bits memory holds, and back: the identity
  // except for floats in the solver's theory.
  Term toBits(Term v, llvm::Type *ty);
  Term fromBits(Term bits, llvm::Type *ty);
  // a + b (opcode FAdd) or a - b (FSub) on two values of the floating-point
  // type ty, rounded to nearest even as the instructions round.
  Term fpBinary(unsigned opcode, Term a, Term b, llvm::Type *ty);
  // minnum/maxnum (a NaN operand gives the other; of equal operands, either)
  // or minimum/maximum (a NaN operand gives NaN; -0 below +0) of two values
  // of the floating-point type ty; id is the intrinsic's.
  Term fpMinMax(unsigned id, Term a, Term b, llvm::Type *ty);
  // A fresh unconstrained term of v's type, named after its origin (nondet
  // calls, arguments); for a pointer, a fresh address converted to one.
  Term fresh(llvm::Type *ty, const std::string &name);
  // Encodes arithmetic, comparison, cast, select, freeze, extractvalue,
  // insertvalue and getelementptr, binding the result.
  void encode(llvm::Instruction &inst);
  // Encodes the arithmetic intrinsics (llvm.*.with.overflow), binding the
  // result. Returns false for any other intrinsic.
  bool intrinsic(llvm::CallBase &c);
  // v != 0 as a Bool term, for assume and assert arguments and branch conditions.
  Term isTrue(const llvm::Value *v);
  // An integer term (Bool for i1) as a bitvector of width bits, extending by
  // sign or zero, or truncating.
  Term resize(Term v, unsigned width, bool isSigned);

private:
  unsigned packedWidth(llvm::Type *ty);
  Term toField(Term v, llvm::Type *ty);
  Term fromField(Term bits, llvm::Type *ty);
  Term replaceBits(Term v, unsigned low, Term part);
  Term toBool(Term bv);
  Term fromBool(Term b, unsigned width);
  Term binary(unsigned opcode, const llvm::Value *lhs, const llvm::Value *rhs);
  Term castTo(unsigned opcode, const llvm::Value *op, llvm::Type *dst);
  Term gep(const llvm::GEPOperator &g);
  camada::SMTSortRef fpSort(llvm::Type *ty);
  camada::SMTSortRef fpSort(llvm::Type *ty, camada::FPEncoding encoding);
  // v as a float of the theory or encoding, for camada's operations on floats.
  camada::SMTExprRef fp(const llvm::Value *v);
  camada::SMTExprRef fp(Term x, llvm::Type *ty); // as above, for a value's term
  Term bits(const camada::SMTExprRef &fp, llvm::Type *ty); // an operation's result as a value
  Term fp80ToIEEE79(Term bv80);
  Term ieee79ToFP80(Term bv79);
  bool native() const { return fpEncoding == camada::FPEncoding::Native; }
  // Bitwuzla's native FP theory supports only IEEE interchange formats
  // (half, float, double, fp128); x86_fp80 and bfloat always use camada's
  // bit-vector FP encoding.
  bool useNative(llvm::Type *ty) const {
    return native() && !ty->isX86_FP80Ty() && !ty->isBFloatTy();
  }
  camada::FPEncoding fpEncodingOf(llvm::Type *ty) const {
    return useNative(ty) ? camada::FPEncoding::Native : camada::FPEncoding::BV;
  }
  Term signBit(const llvm::Value *v); // true when v's sign bit is set
  Term signBit(Term x, llvm::Type *ty); // same, for a term
  camada::SMTExprRef rm(camada::RM mode, llvm::Type *ty);
  Term fcmp(llvm::CmpInst::Predicate p, const llvm::Value *a, const llvm::Value *b);
  // Whether the float with these bits is in one of the classes of mask, as
  // llvm.is.fpclass takes it.
  Term fpClass(const Term &x, llvm::Type *ty, unsigned mask);
  bool fpIntrinsic(llvm::CallBase &c);
  // v's value when its term is a constant float of an IEEE format.
  std::optional<llvm::APFloat> constFloat(const llvm::Value *v);
  // The constant float f as a value of type ty, and the same for the result
  // of an operation on constants, except null for a NaN, whose bits are the
  // encoding's (any NaN natively, the hardware's with fp2bv).
  Term floatValue(const llvm::APFloat &f, llvm::Type *ty);
  Term folded(const llvm::APFloat &r, llvm::Type *ty);

  Terms &t;
  camada::SMTSolver &s;
  memory::Memory &mem;
  camada::FPEncoding fpEncoding;
  Bindings env;
  // In the solver's theory, one term per constant float, keyed by its bits
  // (so -0 and +0 stay apart), and the bits of each such term.
  llvm::DenseMap<std::pair<const llvm::fltSemantics *, llvm::APInt>, Term> floatTerms;
  llvm::DenseMap<const void *, llvm::APInt> floatBits; // by Term id
  // The floating-point term a bit-vector from toBits() came from, keyed by
  // bit-vector Term id and IEEE format, so fromBits(toBits(v, ty), ty)
  // returns v without a round-trip through mkBVToIEEEFP.
  llvm::DenseMap<std::pair<const void *, const llvm::fltSemantics *>, Term> bitsFloat;
  unsigned freshCount = 0;
};

std::string describe(const llvm::Value *v);

} // namespace vereda::encode
