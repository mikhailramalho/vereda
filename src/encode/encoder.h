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
// Operations round to nearest, ties to even, as C does by default. A vector of integers or
// floats is one bitvector with element 0 in the low bits, as memory lays it
// out; the scalarizer leaves vectors only where they cross calls.
#pragma once
#include "encode/terms.h"
#include "memory/memory.h"
#include "util/unsupported.h"
#include <camada/camada.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Operator.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <string>

namespace vereda::encode {

using vereda::Unsupported;

class Encoder {
public:
  Encoder(Terms &t, memory::Memory &mem, bool fp2bv = false)
      : t(t), s(t.solver()), mem(mem),
        fpEncoding(fp2bv ? camada::FPEncoding::BV : camada::FPEncoding::Native) {}
  camada::SMTSolver &solver() { return s; }
  Terms &terms() { return t; }
  unsigned symbolCount() const { return freshCount; }

  camada::SMTSortRef sort(llvm::Type *ty);
  // Terms the path being walked makes equal to a bitvector constant, from
  // its assumptions and branches (Terms::equalities): value() returns the
  // constant in their place, so that what is built from them folds. The
  // walker keeps them per path, so they are a persistent map (cellmap.h)
  // that each arrival copies as a pointer: the key is the term's address,
  // the cell's data the constant and its provenance the term, which keeps
  // the address from being reused.
  using Facts = memory::CellMap;
  Facts facts;
  static Facts::Key key(const camada::SMTExprRef &x) {
    return {reinterpret_cast<uintptr_t>(x.get()), 0};
  }
  // The term bound to v, with facts applied, or a constant. Undef and
  // poison are a fresh value per use.
  camada::SMTExprRef value(const llvm::Value *v);
  void bind(const llvm::Value *v, camada::SMTExprRef term) { env[v] = std::move(term); }
  // The term bound to v, or a null term.
  camada::SMTExprRef bound(const llvm::Value *v) const { return env.lookup(v); }
  // A value of type ty as the bits memory holds, and back: the identity
  // except for floats in the solver's theory.
  camada::SMTExprRef toBits(camada::SMTExprRef v, llvm::Type *ty);
  camada::SMTExprRef fromBits(camada::SMTExprRef bits, llvm::Type *ty);
  // A fresh unconstrained term of v's type, named after its origin (nondet calls, arguments).
  camada::SMTExprRef fresh(llvm::Type *ty, const std::string &name);
  // Encodes arithmetic, comparison, cast, select, freeze, extractvalue,
  // insertvalue and getelementptr, binding the result.
  void encode(llvm::Instruction &inst);
  // Encodes the arithmetic intrinsics (llvm.*.with.overflow), binding the
  // result. Returns false for any other intrinsic.
  bool intrinsic(llvm::CallBase &c);
  // v != 0 as a Bool term, for assume and assert arguments and branch conditions.
  camada::SMTExprRef isTrue(const llvm::Value *v);
  // An integer term (Bool for i1) as a bitvector of width bits, extending by
  // sign or zero, or truncating.
  camada::SMTExprRef resize(camada::SMTExprRef v, unsigned width, bool isSigned);

private:
  unsigned packedWidth(llvm::Type *ty);
  camada::SMTExprRef toField(camada::SMTExprRef v, llvm::Type *ty);
  camada::SMTExprRef fromField(camada::SMTExprRef bits, llvm::Type *ty);
  camada::SMTExprRef replaceBits(camada::SMTExprRef v, unsigned low, camada::SMTExprRef part);
  camada::SMTExprRef toBool(camada::SMTExprRef bv);
  camada::SMTExprRef fromBool(camada::SMTExprRef b, unsigned width);
  camada::SMTExprRef binary(unsigned opcode, const llvm::Value *lhs, const llvm::Value *rhs);
  camada::SMTExprRef castTo(unsigned opcode, const llvm::Value *op, llvm::Type *dst);
  camada::SMTExprRef gep(const llvm::GEPOperator &g);
  camada::SMTSortRef fpSort(llvm::Type *ty);
  camada::SMTExprRef fp(const llvm::Value *v); // v as a float of the theory or encoding
  camada::SMTExprRef bits(const camada::SMTExprRef &fp); // an operation's result as a value
  bool native() const { return fpEncoding == camada::FPEncoding::Native; }
  camada::SMTExprRef signBit(const llvm::Value *v); // true when v's sign bit is set
  camada::SMTExprRef rm(camada::RM mode);
  camada::SMTExprRef fcmp(llvm::CmpInst::Predicate p, const llvm::Value *a,
                          const llvm::Value *b);
  // Whether the float with these bits is in one of the classes of mask, as
  // llvm.is.fpclass takes it.
  camada::SMTExprRef fpClass(const camada::SMTExprRef &x, llvm::Type *ty, unsigned mask);
  bool fpIntrinsic(llvm::CallBase &c);

  Terms &t;
  camada::SMTSolver &s;
  memory::Memory &mem;
  camada::FPEncoding fpEncoding;
  llvm::DenseMap<const llvm::Value *, camada::SMTExprRef> env;
  unsigned freshCount = 0;
};

std::string describe(const llvm::Value *v);

} // namespace vereda::encode
