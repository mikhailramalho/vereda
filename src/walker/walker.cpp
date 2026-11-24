#include "walker/walker.h"

#include "analysis/loops.h"
#include "ir/debug.h"
#include "ir/load.h"
#include "ir/sanitizer.h"
#include "ir/source.h"
#include "util/log.h"
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/MapVector.h>
#include <llvm/ADT/PostOrderIterator.h>
#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/Analysis/CaptureTracking.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/ScalarEvolutionExpressions.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Intrinsics.h>
#include <algorithm>
#include <memory>

using namespace llvm;
using camada::SMTExprRef;
using vereda::Unsupported;

namespace vereda::walker {

namespace {

// Per-function CFG facts, computed once.
struct FunctionInfo {
  FunctionInfo(Function &f, analysis::Counted counted) : dt(f), li(dt), loops(f, dt, li, counted) {
    for (BasicBlock *bb : ReversePostOrderTraversal<Function *>(&f))
      rpo.push_back(bb);
    DenseMap<const BasicBlock *, unsigned> index;
    for (unsigned i = 0; i < rpo.size(); ++i)
      index[rpo[i]] = i;
    // A retreating edge must be a loop back edge, or the CFG is irreducible.
    for (BasicBlock *bb : rpo)
      for (BasicBlock *succ : successors(bb))
        if (index.lookup(succ) <= index.lookup(bb) && !dt.dominates(succ, bb))
          throw Unsupported("irreducible control flow in " + f.getName().str());
    for (BasicBlock *bb : rpo)
      if (li.isLoopHeader(bb))
        loopNumber[li.getLoopFor(bb)] = loopNumber.size();
  }

  DominatorTree dt;
  LoopInfo li;
  analysis::Loops loops;
  std::vector<BasicBlock *> rpo;
  DenseMap<const Loop *, unsigned> loopNumber; // ESBMC's "loop N", per function
};

// One way into a block: the edge's guard and the values its phis take on it,
// captured when the edge is taken, since a later iteration rebinds them.
struct Arrival {
  SMTExprRef guard;
  SmallVector<SMTExprRef, 2> phiValues;
  memory::State mem;
};

struct Return {
  SMTExprRef guard, value;
  memory::State mem;
};

struct Frame {
  DenseMap<const BasicBlock *, SmallVector<Arrival, 2>> arrivals;
  SmallVector<Return, 2> returns;
};

// A conversion of a scanf format (C17 7.21.6.2), as glibc reads it: S and
// C are ls and lc, and m (POSIX) has the argument point to a pointer that
// the call sets to a buffer it allocates.
struct Conversion {
  char conv;
  StringRef length;
  uint64_t width = 0;
  bool suppressed = false, alloc = false;
};

// The conversions of format f in order. A printf format's flags and
// precision read as unknown conversions, which still finds each %n.
static SmallVector<Conversion, 4> conversions(StringRef f) {
  SmallVector<Conversion, 4> out;
  for (size_t i = 0; i < f.size(); ++i) {
    if (f[i] != '%' || ++i == f.size() || f[i] == '%')
      continue;
    Conversion cv;
    cv.suppressed = f[i] == '*';
    i += cv.suppressed;
    for (; i < f.size() && isDigit(f[i]); ++i)
      cv.width = cv.width * 10 + (f[i] - '0');
    size_t from = i;
    for (; i < f.size() && StringRef("hljztLqm").contains(f[i]); ++i)
      cv.alloc |= f[i] == 'm';
    if (i == f.size())
      break;
    cv.length = f.slice(from, i);
    cv.conv = f[i];
    if (cv.conv == 'S' || cv.conv == 'C') {
      cv.conv = toLower(cv.conv);
      cv.length = "l";
    }
    if (cv.conv == '[') // the set may start with ] or ^]
      i = std::min(f.find(']', i + (f.substr(i + 1).starts_with("^") ? 3 : 2)), f.size());
    out.push_back(cv);
  }
  return out;
}

// Where a call to a variadic function left its arguments, for va_start.
struct VarArgs {
  const Function *callee;
  SMTExprRef regs;  // x86-64: the register save area
  SMTExprRef stack; // the first variadic argument on the stack
  unsigned gp, fp;  // x86-64: the register save area offsets past the named arguments
};

class Walk {
public:
  Walk(const Options &opts, encode::Encoder &enc, memory::Memory &mem, checks::Checks &checks,
       Trace &trace)
      : opts(opts), enc(enc), mem(mem), t(enc.terms()), s(enc.solver()), checks(checks), trace(trace) {}

  void entry(Function &f) {
    for (Argument &arg : f.args())
      enc.bind(&arg, enc.fresh(arg.getType(), "arg_" + arg.getName().str()));
    const Module &m = *f.getParent();
    mem.allocateGlobals(m);
    memory::State st = mem.initial();
    for (const GlobalVariable &g : m.globals())
      if (unsigned bits = ownsArrays(&g, m.getDataLayout()))
        mem.own(st, mem.global(&g), bits);
    mem.initialise(st, m, [this](const Constant *c) { return enc.value(c); });
    function(f, t.boolean(true), st);
  }

  // The number of loops summarised, as ESBMC prints it.
  void report() const {
    if (!summarised.empty())
      outs() << "Summarised " << summarised.size() << " array loop(s)\n";
  }

private:
  // Whether the alloca or global v gets arrays of its own (memory.h, #71):
  // a getelementptr reaches it at a symbolic offset, which is where a short
  // chain and a pointer-width index pay, and no pointer to it is stored or
  // passed on (only its provenance counts; comparing its address does not),
  // so few accesses reach it through an unknown pointer, where each object
  // with arrays of its own is one more choice. Returns the bits of their
  // elements (elementBits), or 0 for none.
  unsigned ownsArrays(const Value *v, const DataLayout &dl) {
    auto [it, inserted] = owns.try_emplace(v, 0);
    if (!inserted || capturesAnything(PointerMayBeCaptured(v, /*ReturnCaptures=*/true,
                                                           CaptureComponents::Provenance)))
      return it->second;
    SmallVector<const Value *, 8> work{v};
    SmallPtrSet<const Value *, 8> seen;
    while (!work.empty())
      for (const User *u : work.pop_back_val()->users())
        if (auto *g = dyn_cast<GEPOperator>(u); g && seen.insert(g).second) {
          if (!g->hasAllConstantIndices())
            return it->second = elementBits(v, dl);
          work.push_back(g);
        }
    return 0;
  }

  // N for N = 16, 32 or 64 when every use of v and of the getelementptrs
  // from it is a load or store of iN at an offset that is a multiple of N/8,
  // a comparison or the no-op __vereda_uninitialised (issue #107): its own
  // arrays then hold whole elements. 8, for bytes, otherwise; so a call or
  // intrinsic taking v (memset, memcpy) keeps it on bytes.
  static unsigned elementBits(const Value *v, const DataLayout &dl) {
    unsigned bits = 0;
    auto access = [&](Type *ty) {
      if (!ty->isIntegerTy() || (bits && !ty->isIntegerTy(bits)))
        return false;
      bits = ty->getIntegerBitWidth();
      return true;
    };
    SmallVector<const Value *, 8> work{v};
    SmallVector<const GEPOperator *, 8> geps;
    while (!work.empty()) {
      const Value *p = work.pop_back_val();
      for (const User *u : p->users()) {
        bool ok = isa<ICmpInst>(u) || isa<GEPOperator>(u); // a pointer is no index
        if (auto *l = dyn_cast<LoadInst>(u))
          ok = access(l->getType());
        else if (auto *s = dyn_cast<StoreInst>(u))
          ok = s->getValueOperand() != p && access(s->getValueOperand()->getType());
        else if (auto *g = dyn_cast<GEPOperator>(u)) {
          geps.push_back(g);
          work.push_back(g);
        } else if (auto *c = dyn_cast<CallBase>(u))
          ok = c->getCalledFunction() &&
               c->getCalledFunction()->getName() == "__vereda_uninitialised";
        if (!ok)
          return 8;
      }
    }
    if (bits != 16 && bits != 32 && bits != 64)
      return 8;
    // Each getelementptr moves by a multiple of N/8: its constant offset
    // and the scale of each of its indices have that many low zero bits.
    unsigned shift = Log2_32(bits / 8);
    for (const GEPOperator *g : geps) {
      SmallMapVector<Value *, APInt, 4> scales;
      APInt constant(dl.getIndexTypeSizeInBits(g->getType()), 0);
      if (!g->collectOffset(dl, constant.getBitWidth(), scales, constant) ||
          constant.countr_zero() < shift ||
          any_of(scales, [&](const auto &s) { return s.second.countr_zero() < shift; }))
        return 8;
    }
    return bits;
  }

  FunctionInfo &info(Function &f) {
    std::unique_ptr<FunctionInfo> &fi = infos[&f];
    if (!fi)
      fi = std::make_unique<FunctionInfo>(
          f, analysis::Counted{counts(false), counts(true), opts.property == Property::UnreachCall});
    return *fi;
  }

  // Walks f's body from a call guarded by guard with memory st. Returns the
  // frame's return arrivals.
  Frame function(Function &f, SMTExprRef guard, const memory::State &st) {
    Frame frame;
    frame.arrivals[&f.getEntryBlock()].push_back({guard, {}, st});
    stack.push_back(&f);
    region(frame, info(f), nullptr);
    stack.pop_back();
    return frame;
  }

  // Walks the blocks of loop l, or of the whole function when l is null, in
  // reverse post-order with each inner loop collapsed to its header.
  void region(Frame &frame, FunctionInfo &fi, Loop *l) {
    for (BasicBlock *bb : fi.rpo) {
      if (l && !l->contains(bb))
        continue;
      Loop *inner = fi.li.getLoopFor(bb);
      if (inner == l) {
        block(frame, *bb);
        continue;
      }
      while (inner->getParentLoop() != l)
        inner = inner->getParentLoop();
      if (inner->getHeader() == bb)
        loop(frame, fi, inner);
    }
  }

  void loop(Frame &frame, FunctionInfo &fi, Loop *l) {
    if (opts.summarise && summarise(frame, fi, l))
      return;
    BasicBlock *header = l->getHeader();
    unsigned number = fi.loopNumber.lookup(l);
    for (unsigned i = 0;; ++i) {
      auto it = frame.arrivals.find(header);
      if (it == frame.arrivals.end() || it->second.empty())
        return;
      if (i > 0) {
        SMTExprRef reached = anyGuard(it->second);
        if (opts.unwind && i >= opts.unwind) {
          unwound(Unwound::Loop, number, loopLocation(l), reached, l);
          it->second.clear();
          return;
        }
        // One solver call per iteration without --unwind; term
        // folding will settle most of these without the solver.
        if (!opts.unwind && s.checkSatAssuming({reached}) == camada::CheckResult::UNSAT) {
          it->second.clear();
          return;
        }
        if (opts.showIterations)
          outs() << "Unwinding loop " << number << " iteration " << i << "   "
                 << loopLocation(l).str() << "\n";
      }
      region(frame, fi, l);
    }
  }

  // Walks loop l as one iteration at a nondet index k below its trip count,
  // which keeps every check of its body (ESBMC's array loop summaries), and
  // leaves it with the values its header has after the last iteration and
  // its store's elements written at once (memory.h). False, with nothing
  // walked, when l does not have the shape analysis/loops.h describes or a
  // value the summary needs has no term here: l is then unrolled.
  // Iterations up to which a loop with no store is unrolled when its count
  // is a constant.
  static constexpr uint64_t UnrolledChecks = 256;

  bool summarise(Frame &frame, FunctionInfo &fi, Loop *l) {
    BasicBlock *header = l->getHeader();
    auto in = frame.arrivals.find(header);
    const analysis::Summary *found = fi.loops.summary(l);
    // One summary at a time: a loop in a function the iteration calls is unrolled.
    if (summary || !found || in == frame.arrivals.end() || in->second.empty() ||
        (found->nondet && opts.witness))
      return false;
    analysis::Summary sum = *found;
    ScalarEvolution &se = fi.loops.scev();
    SMTExprRef trips = evaluate(se, sum.trips, l, {});
    if (!trips)
      return false;
    // A loop that only checks or counts, a few times, unrolls into reads at
    // constant indices that fold, where a summary leaves the solver a read
    // at a nondet index (survey 2026-11-23-array-loop-summaries).
    if (const APInt *n = t.constant(trips); !sum.store && n && n->ule(UnrolledChecks))
      return false;
    SMTExprRef k = t.symbol("loop_index#" + std::to_string(summaryCount),
                            s.mkBVSort(trips->getWidth()));
    // The header's phis in iteration k and after the last one, and the
    // values the exit takes from the header.
    SmallVector<SMTExprRef, 2> first;
    SmallVector<std::pair<Value *, SMTExprRef>, 4> last;
    auto at = [&](Value *v, const SMTExprRef &n) {
      return se.isSCEVable(v->getType()) ? evaluate(se, se.getSCEV(v), l, n) : SMTExprRef();
    };
    for (PHINode &phi : header->phis()) {
      first.push_back(at(&phi, k));
      last.push_back({&phi, at(&phi, trips)});
    }
    for (PHINode &phi : sum.exit->phis())
      if (auto *v = dyn_cast<Instruction>(phi.getIncomingValueForBlock(header)); v && l->contains(v))
        last.push_back({v, at(v, trips)});
    SMTExprRef start;
    if (sum.store) {
      start = evaluate(se, sum.address->getStart(), l, {});
      const APInt *object = start ? t.constant(mem.object(start)) : nullptr;
      if (!object || object->isZero())
        return false;
    }
    if (llvm::any_of(first, [](const SMTExprRef &v) { return !v; }) ||
        llvm::any_of(last, [](const auto &v) { return !v.second; }))
      return false;

    SmallVector<Arrival, 2> arrivals = std::move(in->second);
    frame.arrivals.erase(header);
    SMTExprRef guard = anyGuard(arrivals);
    memory::State st = merged(arrivals);
    SMTExprRef body = t.land(guard, t.ult(k, trips));
    if (!t.isFalse(body)) {
      frame.arrivals[header].push_back({body, first, st});
      summary = Summarising{l, k, sum.store, {}, 0};
      region(frame, fi, l);
      // Drop the back edge and the iteration's own exit.
      frame.arrivals.erase(header);
      frame.arrivals.erase(sum.exit);
      auto [stored, bits] = summary->stored;
      summary.reset();
      if (stored) {
        unsigned width = k->getWidth(), ptrBits = mem.pointerBits();
        mem.summarise(st, start, sum.stride, sum.store->getValueOperand()->getType(),
                      enc.resize(trips, ptrBits, false),
                      [&t = t, k, width, ptrBits, stored, bits](const SMTExprRef &n) {
                        SMTExprRef i = width < ptrBits   ? t.extract(n, width - 1, 0)
                                       : width > ptrBits ? t.zext(n, width - ptrBits)
                                                         : n;
                        return std::make_pair(t.substitute(stored, k, i), t.substitute(bits, k, i));
                      });
      }
    }
    for (auto &[v, term] : last)
      enc.bind(v, term);
    arrive(frame, header, sum.exit, guard, st);
    ++summaryCount;
    summarised.insert(l);
    return true;
  }

  // The SCEV e in iteration k of l, counted from 0 as the header is
  // reached, or null for a form not handled here, an AddRec of another loop,
  // or one of l when k is null.
  SMTExprRef evaluate(ScalarEvolution &se, const SCEV *e, const Loop *l, const SMTExprRef &k) {
    unsigned w = se.getTypeSizeInBits(e->getType()), ptrBits = mem.pointerBits();
    bool pointer = e->getType()->isPointerTy();
    if (w == 1) // an i1 is a Bool term
      return {};
    SmallVector<SMTExprRef, 4> ops;
    for (const SCEV *op : e->operands()) {
      ops.push_back(evaluate(se, op, l, k));
      if (!ops.back())
        return {};
    }
    auto fold = [&](auto f) {
      SMTExprRef r = ops[0];
      for (unsigned i = 1; i < ops.size(); ++i)
        r = f(r, ops[i]);
      return r;
    };
    switch (e->getSCEVType()) {
    case scConstant:
      return t.bv(cast<SCEVConstant>(e)->getAPInt());
    case scUnknown: {
      Value *v = cast<SCEVUnknown>(e)->getValue();
      if (isa<UndefValue>(v))
        return {};
      if (auto *i = dyn_cast<Instruction>(v); i && l->contains(i))
        return {};
      return isa<Instruction>(v) || isa<Argument>(v) ? enc.bound(v) : enc.value(v);
    }
    case scPtrToInt:
      return enc.resize(mem.address(ops[0]), w, false);
    case scTruncate:
    case scZeroExtend:
      return enc.resize(ops[0], w, false);
    case scSignExtend:
      return enc.resize(ops[0], w, true);
    case scAddExpr: {
      if (!pointer)
        return fold([&](auto a, auto b) { return t.add(a, b); });
      SMTExprRef p, delta = t.bv(0, ptrBits);
      for (unsigned i = 0; i < ops.size(); ++i)
        if (e->operands()[i]->getType()->isPointerTy())
          p = ops[i];
        else
          delta = t.add(delta, enc.resize(ops[i], ptrBits, true));
      return mem.advance(p, delta);
    }
    case scMulExpr:
      return fold([&](auto a, auto b) { return t.mul(a, b); });
    case scUDivExpr:
      return t.udiv(ops[0], ops[1]);
    case scSMaxExpr:
      return pointer ? SMTExprRef() : fold([&](auto a, auto b) { return t.ite(t.sgt(a, b), a, b); });
    case scUMaxExpr:
      return pointer ? SMTExprRef() : fold([&](auto a, auto b) { return t.ite(t.ugt(a, b), a, b); });
    case scSMinExpr:
      return pointer ? SMTExprRef() : fold([&](auto a, auto b) { return t.ite(t.slt(a, b), a, b); });
    case scUMinExpr:
    case scSequentialUMinExpr:
      return pointer ? SMTExprRef() : fold([&](auto a, auto b) { return t.ite(t.ult(a, b), a, b); });
    case scAddRecExpr: {
      auto *ar = cast<SCEVAddRecExpr>(e);
      if (ar->getLoop() != l || !ar->isAffine() || !k)
        return {};
      SMTExprRef start = ops[0], step = ops[1];
      if (pointer)
        return mem.advance(start, t.mul(enc.resize(step, ptrBits, true), enc.resize(k, ptrBits, false)));
      return t.add(start, t.mul(step, enc.resize(k, w, false)));
    }
    default:
      return {};
    }
  }

  enum class Unwound { Loop, Recursion };

  // The bound was hit with guard reachable: an unwinding assertion, or an
  // assumption that cuts the path. key is the loop or the recursive call.
  void unwound(Unwound kind, unsigned number, const ir::SourceLoc &loc, SMTExprRef reached,
               const void *key) {
    if (!opts.unwindingAssertions)
      return;
    bool isLoop = kind == Unwound::Loop;
    std::string comment = isLoop ? "unwinding assertion loop " + std::to_string(number)
                                 : "recursion unwinding assertion";
    claim(isLoop ? checks::Property::Unwinding : checks::Property::Recursion, loc, comment, "",
          number, reached, key);
  }

  static ir::SourceLoc loopLocation(Loop *l) {
    DebugLoc loc = l->getStartLoc();
    return ir::location(loc, l->getHeader()->getParent()->getName());
  }

  SMTExprRef anyGuard(const SmallVectorImpl<Arrival> &arrivals) {
    SMTExprRef g = arrivals[0].guard;
    for (unsigned i = 1; i < arrivals.size(); ++i)
      g = t.lor(g, arrivals[i].guard);
    return g;
  }

  memory::State merged(const SmallVectorImpl<Arrival> &arrivals) {
    memory::State st = arrivals[0].mem;
    for (unsigned i = 1; i < arrivals.size(); ++i)
      st = mem.ite(arrivals[i].guard, arrivals[i].mem, st);
    return st;
  }

  void block(Frame &frame, BasicBlock &bb) {
    SmallVector<Arrival, 2> arrivals = std::move(frame.arrivals[&bb]);
    frame.arrivals.erase(&bb);
    if (arrivals.empty()) // no edge into bb was taken in this pass
      return;
    SMTExprRef guard = anyGuard(arrivals);
    memory::State st = merged(arrivals);

    unsigned p = 0;
    for (PHINode &phi : bb.phis()) {
      SMTExprRef r = arrivals[0].phiValues[p];
      for (unsigned i = 1; i < arrivals.size(); ++i)
        r = t.ite(arrivals[i].guard, arrivals[i].phiValues[p], r);
      enc.bind(&phi, r);
      ++p;
    }

    mem.setContext(guard);
    for (Instruction &inst : bb) {
      for (const DbgRecord &rec : inst.getDbgRecordRange())
        if (std::optional<ir::DbgAssign> a = ir::dbgAssign(rec))
          if (SMTExprRef v = traceValue(a->value))
            trace.push_back({Step::Assignment, guard, a->loc, a->name, v, a->isUnsigned});
      if (isa<PHINode>(inst))
        continue;
      if (inst.isTerminator())
        terminator(frame, inst, guard, st);
      else if (auto *c = dyn_cast<CallBase>(&inst)) {
        guard = call(*c, guard, st);
        mem.setContext(guard); // the callee's blocks set their own
      }
      else if (!memoryAccess(inst, guard, st))
        enc.encode(inst);
    }
  }

  // A debug record's value for the trace, or null when it has none yet.
  // LCSSA does not route debug uses, so a record can name a value from a
  // block not walked in this pass; the trace skips it.
  SMTExprRef traceValue(const Value *v) {
    if (!v->getType()->isIntegerTy())
      return {};
    if (isa<Instruction>(v) || isa<Argument>(v))
      return enc.bound(v);
    return isa<Constant>(v) ? enc.value(v) : SMTExprRef();
  }

  void arrive(Frame &frame, BasicBlock *pred, BasicBlock *succ, SMTExprRef guard,
              const memory::State &st) {
    if (t.isFalse(guard)) // an edge folded away is not walked
      return;
    Arrival a{guard, {}, st};
    for (PHINode &phi : succ->phis())
      a.phiValues.push_back(enc.value(phi.getIncomingValueForBlock(pred)));
    frame.arrivals[succ].push_back(std::move(a));
  }

  void terminator(Frame &frame, Instruction &term, SMTExprRef guard, const memory::State &st) {
    BasicBlock *bb = term.getParent();
    if (auto *br = dyn_cast<BranchInst>(&term)) {
      if (br->isUnconditional() || br->getSuccessor(0) == br->getSuccessor(1)) {
        arrive(frame, bb, br->getSuccessor(0), guard, st);
        return;
      }
      SMTExprRef c = enc.value(br->getCondition());
      arrive(frame, bb, br->getSuccessor(0), t.land(guard, c), st);
      arrive(frame, bb, br->getSuccessor(1), t.land(guard, t.lnot(c)), st);
    } else if (auto *sw = dyn_cast<SwitchInst>(&term)) {
      SMTExprRef cond = enc.value(sw->getCondition());
      SMTExprRef anyCase = t.boolean(false);
      MapVector<BasicBlock *, SMTExprRef> taken; // in case order, for a stable formula
      for (const auto &c : sw->cases()) {
        SMTExprRef eq = t.eq(cond, enc.value(c.getCaseValue()));
        anyCase = t.lor(anyCase, eq);
        SMTExprRef &to = taken[c.getCaseSuccessor()];
        to = to ? t.lor(to, eq) : eq;
      }
      SMTExprRef &d = taken[sw->getDefaultDest()];
      d = d ? t.lor(d, t.lnot(anyCase)) : t.lnot(anyCase);
      for (auto &[succ, to] : taken)
        arrive(frame, bb, succ, t.land(guard, to), st);
    } else if (auto *ret = dyn_cast<ReturnInst>(&term)) {
      Value *v = ret->getReturnValue();
      frame.returns.push_back({guard, v ? enc.value(v) : SMTExprRef(), st});
    } else if (isa<UnreachableInst>(term)) {
      // Clang ends a block with unreachable after a call that never returns:
      // the path ends there. Anywhere else it is __builtin_unreachable,
      // which ESBMC checks by default.
      auto *prev = dyn_cast_or_null<CallBase>(term.getPrevNode());
      if ((!prev || !prev->doesNotReturn()) && counts(false))
        assertion(term, guard, "__builtin_unreachable", "");
    } else {
      throw Unsupported(std::string("terminator ") + term.getOpcodeName());
    }
  }

  // Returns the path condition after the call: unchanged, strengthened by an
  // assume, or false when the callee does not return.
  SMTExprRef call(CallBase &c, SMTExprRef guard, memory::State &st) {
    // Inline asm that ir did not turn into IR. As GCC's contract for asm
    // has it, it writes its outputs and no other memory, unless it clobbers
    // "memory". Its register outputs get nondet values; one that may write
    // memory is refused when reached, as nothing yet gives memory nondet
    // contents.
    if (auto *ia = dyn_cast<InlineAsm>(c.getCalledOperand())) {
      for (const InlineAsm::ConstraintInfo &ci : ia->ParseConstraints())
        if ((ci.Type == InlineAsm::isOutput && ci.isIndirect) ||
            (ci.Type == InlineAsm::isClobber && ci.Codes[0] == "{memory}"))
          throw Unsupported("inline asm that may write memory at " + ir::location(c).str());
      if (!c.getType()->isVoidTy())
        enc.bind(&c, enc.fresh(c.getType(), "asm@" + ir::location(c).str()));
      return guard;
    }
    // getCalledFunction() is null for an indirect call, and also for a direct
    // one whose type disagrees with the function (K&R, or across files). A
    // bodyless callee is still handled by name, as SV-COMP tasks often
    // declare __VERIFIER_nondet_int() without a prototype.
    Function *callee = c.getCalledFunction();
    if (!callee) {
      callee = dyn_cast<Function>(c.getCalledOperand()->stripPointerCasts());
      if (!callee)
        throw Unsupported("indirect call at " + ir::location(c).str());
      if (!callee->isDeclaration())
        throw Unsupported("call to " + callee->getName().str() +
                          " with a mismatched signature at " + ir::location(c).str());
    }
    StringRef name = callee->getName();
    std::string tag = name.str() + "@" + ir::location(c).str();

    if (callee->isIntrinsic()) {
      if (enc.intrinsic(c) || memoryFunction(c, name, guard, st))
        return guard;
      if (name.starts_with("llvm.lifetime") || name.starts_with("llvm.dbg") ||
          name == "llvm.assume")
        return guard;
      if (name == "llvm.trap") // __builtin_trap: an abnormal end, not an error, as in ESBMC
        return t.boolean(false);
      switch (callee->getIntrinsicID()) {
      case Intrinsic::vastart: vaStart(c, st); return guard;
      case Intrinsic::vacopy: // a va_list holds no pointer into itself
        mem.copy(st, enc.value(c.getArgOperand(0)), enc.value(c.getArgOperand(1)),
                 isX86_64(c) ? 24 : 4);
        return guard;
      case Intrinsic::vaend: return guard;
      default: break;
      }
      if (name == "llvm.ubsantrap") { // a clang sanitizer check failed
        auto *code = cast<ConstantInt>(c.getArgOperand(0));
        if (counts(true))
          assertion(c, guard, ir::sanitizerCheck(code->getZExtValue()), "");
        return t.boolean(false);
      }
      throw Unsupported("intrinsic " + name.str());
    }
    if (opts.property == Property::UnreachCall && name == "reach_error") {
      assertion(c, guard, name.str(), "");
      return t.boolean(false);
    }
    // memcmp, bcmp and memchr from the models (which have no debug info) or
    // bodyless: the walker reads the range whole, which costs no unwinding
    // (#67), and the model's loop runs only when it cannot. A program's own
    // memcmp, with debug info, is walked as written.
    if ((name == "memcmp" || name == "bcmp" || name == "memchr") && !callee->getSubprogram() &&
        stringFunction(c, name == "memchr", guard, st))
      return guard;
    if (!callee->isDeclaration())
      return enter(c, *callee, guard, st);
    // A bodyless setjmp would return a nondet value and longjmp would return
    // normally, which neither under- nor over-approximates the jump (#10).
    // Only a reached call refuses, so dead code does not cost the verdict.
    if (name == "setjmp" || name == "_setjmp" || name == "__sigsetjmp" || name == "sigsetjmp" ||
        name == "longjmp" || name == "_longjmp" || name == "siglongjmp" ||
        name == "__longjmp_chk")
      throw Unsupported("call to " + name.str() + " at " + ir::location(c).str());
    // These write a formatted string, or what they scan, through a va_list,
    // which a bodyless call does not do (no model yet), so they refuse too.
    if (writesThroughVaList(name))
      throw Unsupported("call to " + name.str() + " at " + ir::location(c).str());
    // From ir's lowering of longjmp: a jump no frame on the stack resumes.
    // Inner frames return for their caller to try; the outermost has none.
    if (name == "__vereda_longjmp_unmatched") {
      if (stack.size() > 1)
        return guard;
      if (counts(false))
        assertion(c, guard, "longjmp to a jmp_buf whose setjmp is not active", "");
      return t.boolean(false);
    }
    if (name == "__vereda_longjmp_unsupported")
      throw Unsupported("longjmp back past a value its setjmp needs in " +
                        c.getFunction()->getName().str());
    if (name == "malloc" || name == "calloc") {
      allocateHeap(c, name, guard, st);
      return guard;
    }
    if (memoryFunction(c, name, guard, st))
      return guard;

    // Bodyless verifier functions, as ESBMC treats them by default: a defined
    // one is an ordinary call, except reach_error under unreach-call, above.
    if (name == "__VERIFIER_nondet_memory") {
      havoc(st, name, c, c.getArgOperand(0), constantSize(c.getArgOperand(1), guard, name),
            guard);
      return guard;
    }
    if (name.starts_with("__VERIFIER_nondet_")) {
      // In a summarised iteration, element k of values fresh per iteration,
      // an array per call: camada gives two symbols of one name one term, and
      // two calls can share a location (a macro, IR without debug info).
      SMTExprRef v =
          summary && summary->loop->contains(&c)
              ? t.select(t.symbol(tag + "#" + std::to_string(summaryCount) + "." +
                                      std::to_string(summary->nondets++),
                                  s.mkArraySort(s.mkBVSort(summary->index->getWidth()),
                                                enc.sort(c.getType()))),
                         summary->index)
              : enc.fresh(c.getType(), tag);
      enc.bind(&c, v);
      // For the witness, which reads the value's type from the callee: not
      // for a call through a mismatched signature.
      if (c.getCalledFunction())
        trace.push_back({Step::Call, guard, ir::location(c), name.str(), v});
      return guard;
    }
    if (name == "__VERIFIER_assume") {
      trace.push_back({Step::Assumption, guard, ir::location(c), ir::callArgumentText(c), {}});
      return t.land(guard, enc.isTrue(c.getArgOperand(0)));
    }
    if (name == "__assert_fail" || name == "reach_error" || name == "__VERIFIER_error") {
      if (counts(false))
        assertion(c, guard, name.str(), "");
      return t.boolean(false);
    }
    if (name == "__VERIFIER_assert" || (name == "assert" && c.arg_size() == 1)) {
      // assert without <assert.h> is an implicitly declared function; ESBMC's
      // front end treats it as the assertion it was meant to be.
      SMTExprRef holds = enc.isTrue(c.getArgOperand(0));
      if (!counts(false))
        return t.land(guard, holds);
      assertion(c, t.land(guard, t.lnot(holds)), name.str(), ir::callArgumentText(c));
      return guard;
    }
    if (name == "__vereda_object_size") { // for the realloc model
      SMTExprRef p = enc.value(c.getArgOperand(0));
      enc.bind(&c, enc.resize(t.sub(mem.size(mem.object(p)), mem.offset(p)),
                              c.getType()->getIntegerBitWidth(), false));
      return guard;
    }
    if (name == "__vereda_uninitialised") // keeps LLVM from folding reads of the local
      return guard;
    if (name == "abort" || name == "exit" || name == "_Exit")
      return t.boolean(false);
    // A struct result stays unsupported: the bodyless functions returning
    // one are mostly libgcc's complex arithmetic (__muldc3), whose result C
    // defines, so a nondet one would give wrong FAILEDs.
    if (c.getType()->isStructTy())
      throw Unsupported("call to " + name.str() + " returning a struct at " +
                        ir::location(c).str());
    // An unknown external returns an unconstrained value and may write all
    // of each object it gets a pointer to (read, fgets, ioctl), except a
    // constant and what its attributes, which LLVM infers for the C library,
    // say it only reads. With a constant format without positional
    // arguments, scanf writes its variadic arguments as scan says, and
    // printf none unless the format has a %n. It may also write through
    // pointers stored in those objects, or to globals, which this does not
    // cover. The pthread functions write nothing here, as threads are not
    // modelled yet.
    if (warned.insert(callee).second)
      util::warning("no body for function " + name);
    unsigned params = c.getFunctionType()->getNumParams(), end = c.arg_size();
    StringRef f;
    bool format = c.getFunctionType()->isVarArg() && params &&
                  getConstantStringInfo(c.getArgOperand(params - 1), f) && !f.contains('$');
    bool scanf = format && name.ends_with("scanf");
    if (scanf || (format && name.ends_with("printf") &&
                  none_of(conversions(f), [](const Conversion &cv) { return cv.conv == 'n'; })))
      end = params;
    if (!c.onlyReadsMemory() && !name.starts_with("pthread_"))
      for (unsigned i = 0; i < end; ++i) {
        Value *a = c.getArgOperand(i);
        auto *g = dyn_cast<GlobalVariable>(a->stripPointerCasts());
        if (a->getType()->isPointerTy() && !c.onlyReadsMemory(i) && !(g && g->isConstant()))
          havoc(st, name, c, a, std::nullopt, guard);
      }
    if (scanf)
      return scan(c, f, name, tag, guard, st);
    if (!c.getType()->isVoidTy())
      enc.bind(&c, enc.fresh(c.getType(), tag));
    return guard;
  }

  // The variadic arguments of scanf, fscanf and sscanf for a constant format
  // f (C17 7.21.6.2): each conversion writes the bytes its type takes, a
  // string up to its width or the end of the object, an unknown conversion
  // the whole object, and the result r says how many were assigned: the
  // first r, or none for EOF. A %n is written only when the conversions
  // before it were, if any.
  SMTExprRef scan(CallBase &c, StringRef f, StringRef name, const std::string &tag,
                  SMTExprRef guard, memory::State &st) {
    unsigned arg = c.getFunctionType()->getNumParams();
    SMTExprRef r = enc.fresh(c.getType(), tag);
    enc.bind(&c, r);
    const DataLayout &dl = c.getDataLayout();
    unsigned bits = r->getWidth(), ptr = dl.getPointerSize(), assigned = 0;
    for (const Conversion &cv : conversions(f)) {
      if (arg == c.arg_size())
        break;
      if (cv.suppressed)
        continue;
      Value *a = c.getArgOperand(arg++);
      SMTExprRef done = cv.conv == 'n' ? t.sge(r, t.bv(assigned ? int64_t(assigned) : -1, bits))
                                       : t.sgt(r, t.bv(assigned++, bits));
      if (!a->getType()->isPointerTy()) // undefined behaviour, not checked
        continue;
      StringRef l = cv.length;
      bool big = l == "ll" || l == "L" || l == "q"; // glibc: long long, or long double
      unsigned wide = l == "l" ? 4 : 1;
      std::optional<uint64_t> n; // none: the whole object
      if (cv.alloc)
        n = ptr;
      else if (StringRef("diouxXn").contains(cv.conv))
        n = l == "hh" ? 1 : l == "h" ? 2 : big || l == "j" ? 8
          : l.empty() ? 4 : ptr; // l, z and t: the pointer width on Linux
      else if (StringRef("aAeEfFgG").contains(cv.conv))
        n = big ? dl.getTypeAllocSize(Type::getX86_FP80Ty(c.getContext())) : l == "l" ? 8 : 4;
      else if (cv.conv == 'p')
        n = ptr;
      else if (cv.conv == 'c')
        n = (cv.width ? cv.width : 1) * wide;
      else if ((cv.conv == 's' || cv.conv == '[') && cv.width) // the characters and a null
        n = (cv.width + 1) * wide;
      else if (cv.conv == 's' || cv.conv == '[')
        n = single(t.sub(mem.size(mem.object(enc.value(a))), mem.offset(enc.value(a))), guard);
      memory::State written = st;
      havoc(written, name, c, a, n, guard);
      st = mem.ite(done, written, st);
    }
    SMTExprRef range = t.land(t.sge(r, t.bv(-1, bits)), t.sle(r, t.bv(assigned, bits)));
    return t.land(guard, range);
  }

  // Walks a defined callee in a new frame.
  SMTExprRef enter(CallBase &c, Function &callee, SMTExprRef guard, memory::State &st) {
    unsigned depth = std::count(stack.begin(), stack.end(), &callee);
    if (depth > 0) {
      bool cut = false;
      if (opts.unwind && depth >= opts.unwind) {
        unsigned &number = recursionNumber[&c];
        if (!number)
          number = ++recursionsIn[c.getFunction()];
        unwound(Unwound::Recursion, number, ir::location(c), guard, &c);
        cut = true;
      } else if (!opts.unwind && s.checkSatAssuming({guard}) == camada::CheckResult::UNSAT) {
        cut = true;
      }
      if (cut) { // no path continues; the result only has to be bound
        if (!c.getType()->isVoidTy())
          enc.bind(&c, enc.fresh(c.getType(), "unwound"));
        return t.boolean(false);
      }
    }

    SmallVector<SMTExprRef, 4> actuals;
    for (Value *arg : c.args())
      actuals.push_back(enc.value(arg));
    // A recursive call rebinds the caller's own values; keep them.
    SmallVector<std::pair<const Value *, SMTExprRef>, 0> saved;
    if (depth > 0) {
      for (Argument &arg : callee.args())
        if (SMTExprRef term = enc.bound(&arg))
          saved.push_back({&arg, term});
      for (Instruction &inst : instructions(callee))
        if (SMTExprRef term = enc.bound(&inst))
          saved.push_back({&inst, term});
    }
    for (Argument &arg : callee.args())
      enc.bind(&arg, actuals[arg.getArgNo()]);

    const CallBase *outerReachError = reachError;
    if (callee.getName() == "reach_error")
      reachError = &c;
    if (callee.isVarArg())
      varArgs.push_back(passVarArgs(c, callee, guard, st));
    Frame frame = function(callee, guard, st);
    if (callee.isVarArg())
      varArgs.pop_back();
    reachError = outerReachError;

    SMTExprRef after = t.boolean(false), result;
    for (unsigned i = 0; i < frame.returns.size(); ++i) {
      Return &r = frame.returns[i];
      after = frame.returns.size() == 1 ? r.guard : t.lor(after, r.guard);
      result = result ? t.ite(r.guard, r.value, result) : r.value;
      st = i ? mem.ite(r.guard, r.mem, st) : r.mem;
    }
    for (auto &[v, term] : saved)
      enc.bind(v, term);
    if (!c.getType()->isVoidTy())
      enc.bind(&c, result ? result : enc.fresh(c.getType(), "noreturn"));
    return after;
  }

  // vsprintf, vsnprintf, vasprintf and the v*scanf, under glibc's names too.
  static bool writesThroughVaList(StringRef name) {
    name.consume_front("__isoc99_") || name.consume_front("__isoc23_") ||
        (name.consume_front("__") && name.consume_back("_chk"));
    return is_contained<StringRef>({"vsprintf", "vsnprintf", "vasprintf", "vscanf", "vfscanf",
                                    "vsscanf", "vswprintf", "vwscanf", "vfwscanf", "vswscanf"},
                                   name);
  }

  // The va_list layouts known: x86-64 SysV (true) and i386 (false).
  static bool isX86_64(const CallBase &c) {
    const Module &m = *c.getModule();
    const Triple &tt = m.getTargetTriple();
    unsigned bits = m.getDataLayout().getPointerSizeInBits();
    if (tt.isOSWindows() || !((tt.getArch() == Triple::x86_64 && bits == 64) ||
                              (tt.getArch() == Triple::x86 && bits == 32)))
      throw Unsupported("variadic arguments on " + tt.str());
    return bits == 64;
  }

  // Lays out the arguments of c, a call to the variadic callee, where the
  // caller leaves them, which is what clang's va_arg code in the callee
  // reads. On x86-64 the first six integers and pointers go in general
  // registers and the first eight floats and doubles in SSE registers, which
  // the callee's prologue saves to its register save area; the rest, and
  // byval aggregates, go on the stack in slots of 8 bytes. On i386 all go on
  // the stack in slots of 4. Clang has already split a small struct into such
  // pieces, or made it byval when the registers it needs are taken, so the IR
  // arguments can be placed one by one. Only the variadic ones are written:
  // va_arg never reads a named one.
  VarArgs passVarArgs(CallBase &c, Function &callee, SMTExprRef guard, memory::State &st) {
    bool x64 = isX86_64(c);
    if (c.getCallingConv() != CallingConv::C)
      throw Unsupported("variadic call with another calling convention at " +
                        ir::location(c).str());
    const DataLayout &dl = mem.layout();
    unsigned bits = mem.pointerBits(), slot = x64 ? 8 : 4, gp = 0, fp = 48;
    uint64_t onStack = 0, named = 0;
    VarArgs va{&callee, {}, {}, gp, fp};
    // Per argument: in the register save area or on the stack, and where.
    SmallVector<std::pair<bool, uint64_t>, 8> where;
    for (unsigned i = 0; i < c.arg_size(); ++i) {
      Type *ty = c.getArgOperand(i)->getType();
      bool byval = c.isByValArgument(i), isFP = ty->isFloatTy() || ty->isDoubleTy();
      if (c.paramHasAttr(i, Attribute::InReg) || c.paramHasAttr(i, Attribute::InAlloca) ||
          c.paramHasAttr(i, Attribute::Preallocated) || c.paramHasAttr(i, Attribute::Nest) ||
          !(byval || isFP || ty->isPointerTy() ||
            (ty->isIntegerTy() && ty->getIntegerBitWidth() <= 64)))
        throw Unsupported("variadic call with argument " + encode::describe(c.getArgOperand(i)) +
                          " at " + ir::location(c).str());
      if (x64 && !byval && isFP && fp < 176) {
        where.push_back({true, fp});
        fp += 16;
      } else if (x64 && !byval && !isFP && gp < 48) {
        where.push_back({true, gp});
        gp += 8;
      } else {
        uint64_t size =
            byval ? dl.getTypeAllocSize(c.getParamByValType(i)) : dl.getTypeStoreSize(ty);
        uint64_t align =
            byval ? std::max<uint64_t>(slot, c.getParamAlign(i).valueOrOne().value()) : slot;
        where.push_back({false, alignTo(onStack, align)});
        onStack = alignTo(where.back().second + size, slot);
      }
      if (i < callee.arg_size()) {
        va.gp = gp;
        va.fp = fp;
        named = onStack;
      }
    }
    // The stack starts 16-byte aligned at a call, which clang's va_arg code
    // relies on for an over-aligned byval argument.
    SMTExprRef stackArea = mem.allocate(t.bv(onStack, bits), 16, guard);
    va.stack = mem.advance(stackArea, t.bv(named, bits));
    if (x64)
      va.regs = mem.allocate(t.bv(176, bits), 16, guard);
    for (unsigned i = callee.arg_size(); i < c.arg_size(); ++i) {
      Value *arg = c.getArgOperand(i);
      SMTExprRef at =
          mem.advance(where[i].first ? va.regs : stackArea, t.bv(where[i].second, bits));
      if (c.isByValArgument(i))
        mem.copy(st, at, enc.value(arg), dl.getTypeAllocSize(c.getParamByValType(i)));
      else
        mem.store(st, at, enc.toBits(enc.value(arg), arg->getType()), arg->getType());
    }
    return va;
  }

  // va_start(ap) in the innermost variadic frame: on x86-64 ap is
  // {gp_offset, fp_offset, overflow_arg_area, reg_save_area}, the offsets
  // past the registers the named arguments took; on i386 it points to the
  // first variadic argument on the stack.
  void vaStart(CallBase &c, memory::State &st) {
    if (varArgs.empty() || varArgs.back().callee != stack.back())
      throw Unsupported("va_start outside a variadic call at " + ir::location(c).str());
    const VarArgs &va = varArgs.back();
    SMTExprRef ap = enc.value(c.getArgOperand(0));
    Type *ptr = c.getArgOperand(0)->getType(), *i32 = Type::getInt32Ty(c.getContext());
    if (!isX86_64(c)) {
      mem.store(st, ap, va.stack, ptr);
      return;
    }
    unsigned bits = mem.pointerBits();
    mem.store(st, ap, t.bv(va.gp, 32), i32);
    mem.store(st, mem.advance(ap, t.bv(4, bits)), t.bv(va.fp, 32), i32);
    mem.store(st, mem.advance(ap, t.bv(8, bits)), va.stack, ptr);
    mem.store(st, mem.advance(ap, t.bv(16, bits)), va.regs, ptr);
  }

  // Whether a failed assertion, or sanitizer check, violates the property
  // asked for. Under unreach-call only reach_error does, handled at the call.
  bool counts(bool sanitizer) const {
    return opts.property == Property::All || (opts.property == Property::NoOverflow && sanitizer);
  }

  // A check at inst, violated when violated holds; comment names it.
  void assertion(const Instruction &inst, SMTExprRef violated, std::string comment,
                 std::string expr) {
    unsigned &index = assertionNumber[&inst];
    if (!index)
      index = ++assertionsIn[inst.getFunction()];
    claim(checks::Property::Assertion, ir::location(inst), std::move(comment), std::move(expr),
          index, violated, &inst);
  }

  // One claim per source construct, keyed by its instruction or loop:
  // reaching it again, in another iteration or context, ORs into the check.
  void claim(checks::Property property, const ir::SourceLoc &loc, std::string comment,
             std::string expr, unsigned index, SMTExprRef violated, const void *key) {
    auto [it, inserted] = claims.try_emplace(key, checks.size());
    bool viaReachError = reachError || comment == "reach_error";
    trace.push_back({Step::Check, violated, reachError ? ir::location(*reachError) : loc,
                     viaReachError ? "reach_error" : "", {}, false, it->second});
    if (!inserted) {
      checks[it->second].violated = t.lor(checks[it->second].violated, violated);
      return;
    }
    checks.push_back({property, loc, std::move(comment), std::move(expr), index, violated});
  }

  // The one value v takes where guard holds (0 if it holds nowhere), which
  // the solver checks; folding terms as they are built settles most
  // without it. None when v may take several.
  std::optional<uint64_t> single(SMTExprRef v, SMTExprRef guard) {
    if (const APInt *c = t.constant(v))
      return c->getLimitedValue();
    camada::CheckResult feasible = s.checkSatAssuming({guard});
    if (feasible == camada::CheckResult::UNSAT)
      return 0; // the path is infeasible
    if (feasible != camada::CheckResult::SAT)
      return std::nullopt;
    auto model = s.getBVUnsigned(v);
    if (!model)
      return std::nullopt;
    SMTExprRef other = t.lnot(t.eq(v, t.bv(model.value(), v->getWidth())));
    if (s.checkSatAssuming({guard, other}) != camada::CheckResult::UNSAT)
      return std::nullopt;
    return model.value();
  }

  // Size operand of a memory function. A symbolic size is accepted when it
  // has one value on the path; one with several needs a loop or a
  // quantifier. Memory limits how many bytes go one at a time.
  uint64_t constantSize(const Value *v, SMTExprRef guard, StringRef what) {
    if (auto *ci = dyn_cast<ConstantInt>(v))
      return ci->getLimitedValue();
    std::optional<uint64_t> n = single(enc.value(v), guard);
    if (!n)
      throw Unsupported(what.str() + " with a symbolic size");
    return *n;
  }

  // Gives the n bytes at a, or its whole object, nondet contents (see
  // Memory::havoc), for a call to name. A pointer whose object the terms
  // leave open, as after a store at a symbolic address, is accepted when
  // the object has one value on the path.
  void havoc(memory::State &st, StringRef name, CallBase &c, Value *a,
             std::optional<uint64_t> n, SMTExprRef guard) {
    SMTExprRef p = enc.value(a);
    if (mem.havoc(st, p, n))
      return;
    std::optional<uint64_t> o = single(mem.object(p), guard);
    if (!o || !mem.havoc(st, mem.ptr(t.bv(*o, mem.object(p)->getWidth()), mem.offset(p)), n))
      throw Unsupported("call to " + name.str() + " without a body, which may write an "
                        "object chosen at run time, at " + ir::location(c).str());
  }

  // alloca, load and store, and the atomics. Returns false for any other
  // instruction.
  bool memoryAccess(Instruction &inst, SMTExprRef guard, memory::State &st) {
    if (auto *a = dyn_cast<AllocaInst>(&inst)) {
      const DataLayout &dl = a->getDataLayout();
      unsigned bits = mem.pointerBits();
      // A stack object that does not fit is a stack overflow, which the
      // verifier assumes away: the path ends, like ESBMC. An element that
      // does not fit even the pointer width (a constant-size VLA turned into
      // one array by instcombine, #59) ends it before any term is built.
      uint64_t elemSize = dl.getTypeAllocSize(a->getAllocatedType());
      if (!isUIntN(bits, elemSize)) {
        s.addConstraint(t.lnot(guard));
        enc.bind(a, mem.null());
        return true;
      }
      SMTExprRef count = enc.resize(enc.value(a->getArraySize()), bits, false);
      SMTExprRef elem = t.bv(elemSize, bits);
      SMTExprRef ok = t.land(t.lnot(t.umulo(count, elem)),
                              mem.fits(t.mul(count, elem)));
      SMTExprRef p = mem.allocate(t.mul(count, elem), a->getAlign().value(), guard);
      if (unsigned bits = ownsArrays(a, dl))
        mem.own(st, p, bits);
      enc.bind(a, p);
      s.addConstraint(t.implies(guard, ok));
      return true;
    }
    // Atomics: one thread, so each is a plain load and store and a fence
    // orders nothing.
    if (auto *l = dyn_cast<LoadInst>(&inst)) {
      enc.bind(l, enc.fromBits(mem.load(st, enc.value(l->getPointerOperand()), l->getType()),
                               l->getType()));
      return true;
    }
    if (auto *w = dyn_cast<StoreInst>(&inst)) {
      Value *v = w->getValueOperand();
      SMTExprRef p = enc.value(w->getPointerOperand());
      asmTarget(inst, p);
      mem.store(st, p, enc.toBits(enc.value(v), v->getType()), v->getType());
      if (summary && w == summary->store)
        summary->stored = {guard, enc.toBits(enc.value(v), v->getType())};
      // A source store into a named variable kept in memory (a global, an
      // array, a local whose address is taken) is an assignment to show.
      Type *ty = v->getType();
      if (w->getDebugLoc() && ty->isIntegerTy() && !ty->isIntegerTy(1))
        if (auto var = namedObject(w->getPointerOperand()))
          trace.push_back({Step::Assignment, guard, ir::location(*w), var->first, enc.value(v),
                           false, 0, var->second, mem.offset(p)});
      return true;
    }
    if (auto *rmw = dyn_cast<AtomicRMWInst>(&inst)) {
      atomicRMW(*rmw, st);
      return true;
    }
    if (auto *cx = dyn_cast<AtomicCmpXchgInst>(&inst)) {
      Type *ty = cx->getNewValOperand()->getType();
      if (!ty->isIntegerTy() || ty->isIntegerTy(1))
        throw Unsupported("instruction " + encode::describe(&inst));
      SMTExprRef p = enc.value(cx->getPointerOperand());
      asmTarget(inst, p);
      SMTExprRef old = mem.load(st, p, ty);
      SMTExprRef ok = t.eq(old, enc.value(cx->getCompareOperand()));
      if (cx->isWeak()) // may fail even when equal
        ok = t.land(ok, enc.fresh(Type::getInt1Ty(inst.getContext()), "cmpxchg_weak"));
      mem.store(st, p, t.ite(ok, enc.value(cx->getNewValOperand()), old), ty);
      // {old, success}: a struct of integers packs field 0 into the low bits.
      enc.bind(cx, t.concat(t.ite(ok, t.bv(1, 1), t.bv(0, 1)), old));
      return true;
    }
    return isa<FenceInst>(inst);
  }

  // A write an inline asm became (ir marks it with AsmMD) goes only through
  // a pointer to a known object, or one of two: any other pointer, such as
  // one the program read but never set, may point to any object, and a
  // write there could reach state the asm never touches.
  void asmTarget(Instruction &inst, const SMTExprRef &p) {
    MDNode *md = inst.getMetadata(ir::AsmMD);
    if (!md)
      return;
    SMTExprRef obj = mem.object(p), c, x, y;
    if (!t.constant(obj) && !t.split(obj, c, x, y))
      throw Unsupported("inline asm `" + cast<MDString>(md->getOperand(0))->getString().str() +
                        "` through a pointer to an unknown object at " +
                        ir::location(inst).str());
  }

  // The source variable whose memory p points into, when it is a global or
  // a local kept in memory with a name in the debug info: name and type.
  static std::optional<std::pair<std::string, const DIType *>> namedObject(Value *p) {
    while (auto *g = dyn_cast<GEPOperator>(p))
      p = g->getPointerOperand();
    p = p->stripPointerCasts();
    if (auto *gv = dyn_cast<GlobalVariable>(p)) {
      SmallVector<DIGlobalVariableExpression *, 1> vars;
      gv->getDebugInfo(vars);
      if (!vars.empty())
        return std::make_pair(vars[0]->getVariable()->getName().str(),
                              vars[0]->getVariable()->getType());
    }
    if (auto *a = dyn_cast<AllocaInst>(p))
      for (DbgVariableRecord *d : findDVRDeclares(a))
        if (!d->getExpression()->getNumElements())
          return std::make_pair(d->getVariable()->getName().str(), d->getVariable()->getType());
    return std::nullopt;
  }

  // Loads the old value, stores the operation's result and binds the old
  // value, as one thread sees an atomic read-modify-write.
  void atomicRMW(AtomicRMWInst &rmw, memory::State &st) {
    Type *ty = rmw.getType();
    AtomicRMWInst::BinOp op = rmw.getOperation();
    if (!(ty->isIntegerTy() && !ty->isIntegerTy(1)) &&
        !(ty->isPointerTy() && op == AtomicRMWInst::Xchg))
      throw Unsupported("instruction " + encode::describe(&rmw));
    SMTExprRef p = enc.value(rmw.getPointerOperand());
    asmTarget(rmw, p);
    SMTExprRef old = mem.load(st, p, ty), v = enc.value(rmw.getValOperand());
    unsigned w = ty->isIntegerTy() ? ty->getIntegerBitWidth() : 0;
    auto one = [&] { return t.bv(1, w); };
    SMTExprRef r;
    switch (op) {
    case AtomicRMWInst::Xchg: r = v; break;
    case AtomicRMWInst::Add: r = t.add(old, v); break;
    case AtomicRMWInst::Sub: r = t.sub(old, v); break;
    case AtomicRMWInst::And: r = t.bvand(old, v); break;
    case AtomicRMWInst::Nand: r = t.bvnot(t.bvand(old, v)); break;
    case AtomicRMWInst::Or: r = t.bvor(old, v); break;
    case AtomicRMWInst::Xor: r = t.bvxor(old, v); break;
    case AtomicRMWInst::Max: r = t.ite(t.sgt(old, v), old, v); break;
    case AtomicRMWInst::Min: r = t.ite(t.slt(old, v), old, v); break;
    case AtomicRMWInst::UMax: r = t.ite(t.ugt(old, v), old, v); break;
    case AtomicRMWInst::UMin: r = t.ite(t.ult(old, v), old, v); break;
    case AtomicRMWInst::UIncWrap: r = t.ite(t.uge(old, v), t.bv(0, w), t.add(old, one())); break;
    case AtomicRMWInst::UDecWrap:
      r = t.ite(t.lor(t.eq(old, t.bv(0, w)), t.ugt(old, v)), v, t.sub(old, one()));
      break;
    case AtomicRMWInst::USubCond: r = t.ite(t.uge(old, v), t.sub(old, v), old); break;
    case AtomicRMWInst::USubSat: r = t.ite(t.uge(old, v), t.sub(old, v), t.bv(0, w)); break;
    default: throw Unsupported("instruction " + encode::describe(&rmw));
    }
    mem.store(st, p, r, ty);
    enc.bind(&rmw, old);
  }

  // memcpy, memmove and memset, as intrinsics or library calls, and free.
  // Returns false for any other function.
  bool memoryFunction(CallBase &c, StringRef name, SMTExprRef guard, memory::State &st) {
    if (name.starts_with("llvm.memcpy") || name.starts_with("llvm.memmove") ||
        name == "memcpy" || name == "memmove") {
      mem.copy(st, enc.value(c.getArgOperand(0)), enc.value(c.getArgOperand(1)),
               constantSize(c.getArgOperand(2), guard, name));
    } else if (name.starts_with("llvm.memset") || name == "memset") {
      mem.set(st, enc.value(c.getArgOperand(0)),
              enc.resize(enc.value(c.getArgOperand(1)), 8, false),
              constantSize(c.getArgOperand(2), guard, name));
    } else if (name == "free" || name.starts_with("llvm.stackrestore")) {
      // Free changes nothing until memory safety tracks liveness.
    } else if (name.starts_with("llvm.stacksave")) {
      enc.bind(&c, enc.fresh(c.getType(), "stacksave"));
      return true;
    } else {
      return false;
    }
    // memcpy, memmove and memset return their destination.
    if (!c.getType()->isVoidTy())
      enc.bind(&c, enc.value(c.getArgOperand(0)));
    return true;
  }

  // memcmp and bcmp (find false) or memchr (find true) over their n bytes as
  // one term: the difference of the first differing bytes as unsigned
  // chars, or a pointer to the first byte equal to (unsigned char)c, else
  // null. False when the call has another shape or n has no usable bound.
  bool stringFunction(CallBase &c, bool find, SMTExprRef guard, memory::State &st) {
    if (c.arg_size() != 3 || !c.getArgOperand(0)->getType()->isPointerTy() ||
        !c.getArgOperand(2)->getType()->isIntegerTy() ||
        !(find ? c.getArgOperand(1)->getType()->isIntegerTy() && c.getType()->isPointerTy()
               : c.getArgOperand(1)->getType()->isPointerTy() && c.getType()->isIntegerTy(32)))
      return false;
    SMTExprRef a = enc.value(c.getArgOperand(0)), other = enc.value(c.getArgOperand(1));
    SMTExprRef n = enc.value(c.getArgOperand(2));
    std::optional<uint64_t> bytes =
        find ? rangeBound(n, guard, {a}) : rangeBound(n, guard, {a, other});
    if (!bytes)
      return false;
    Type *i8 = Type::getInt8Ty(c.getContext());
    SMTExprRef ch;
    if (find)
      ch = enc.resize(other, 8, false);
    SMTExprRef r = find ? mem.null() : t.bv(0, 32);
    // From the last byte back, so that the first one that decides is outermost.
    for (uint64_t i = *bytes; i-- > 0;) {
      SMTExprRef in = t.ult(t.bv(i, n->getWidth()), n);
      SMTExprRef at = mem.advance(a, t.bv(i, mem.pointerBits()));
      SMTExprRef x = mem.load(st, at, i8);
      if (find) {
        r = t.ite(t.land(in, t.eq(x, ch)), at, r);
        continue;
      }
      SMTExprRef y = mem.load(st, mem.advance(other, t.bv(i, mem.pointerBits())), i8);
      r = t.ite(t.land(in, t.lnot(t.eq(x, y))),
                t.sub(enc.resize(x, 32, false), enc.resize(y, 32, false)), r);
    }
    enc.bind(&c, r);
    return true;
  }

  // How many bytes a range of n bytes at the pointers ptrs can cover: n when
  // it is a constant, else the fewest bytes left in their objects, when
  // these are constants and the solver shows n stays within them on this
  // path (a larger n reads past an object, which the model's loop does byte
  // by byte). None past Memory::MaxBytes.
  std::optional<uint64_t> rangeBound(SMTExprRef n, SMTExprRef guard,
                                     std::initializer_list<SMTExprRef> ptrs) {
    if (const APInt *k = t.constant(n))
      return k->ule(memory::Memory::MaxBytes) ? std::optional(k->getZExtValue()) : std::nullopt;
    uint64_t bound = memory::Memory::MaxBytes;
    for (const SMTExprRef &p : ptrs) {
      const APInt *size = t.constant(mem.size(mem.object(p))), *off = t.constant(mem.offset(p));
      if (!size || !off || off->ugt(*size))
        return std::nullopt;
      bound = std::min(bound, (*size - *off).getZExtValue());
    }
    if (s.checkSatAssuming({guard, t.ugt(n, t.bv(bound, n->getWidth()))}) !=
        camada::CheckResult::UNSAT)
      return std::nullopt;
    return bound;
  }

  // malloc and calloc return null when the object does not fit in the
  // address space, or calloc's size overflows (C23 7.24.3.2). Like ESBMC by
  // default, malloc may also fail anyway unless --force-malloc-success.
  void allocateHeap(CallBase &c, StringRef name, SMTExprRef guard, memory::State &st) {
    unsigned bits = mem.pointerBits();
    bool isCalloc = name == "calloc";
    SMTExprRef size = enc.resize(enc.value(c.getArgOperand(0)), bits, false);
    SMTExprRef ok = t.boolean(true);
    if (isCalloc) {
      SMTExprRef count = size;
      size = enc.resize(enc.value(c.getArgOperand(1)), bits, false);
      ok = t.lnot(t.umulo(count, size));
      size = t.mul(count, size);
    }
    ok = t.land(ok, mem.fits(size));
    if (!opts.forceMallocSuccess)
      ok = t.land(ok, enc.fresh(Type::getInt1Ty(c.getContext()), "malloc_succeeds"));
    SMTExprRef p = mem.allocate(size, 16, t.land(guard, ok));
    if (isCalloc) {
      // A product beyond the largest object makes ok false: nothing to zero.
      uint64_t n;
      if (!__builtin_mul_overflow(constantSize(c.getArgOperand(0), guard, "calloc"),
                                  constantSize(c.getArgOperand(1), guard, "calloc"), &n) &&
          isUIntN(bits - 1, n))
        mem.set(st, p, t.bv(0, 8), n);
    }
    enc.bind(&c, t.ite(ok, p, mem.null()));
  }

  const Options &opts;
  encode::Encoder &enc;
  memory::Memory &mem;
  encode::Terms &t;
  camada::SMTSolver &s;
  checks::Checks &checks;
  Trace &trace;
  DenseMap<const Function *, std::unique_ptr<FunctionInfo>> infos;
  DenseMap<const Value *, unsigned> owns; // ownsArrays, per alloca and global
  SmallVector<const Function *, 8> stack;
  SmallVector<VarArgs, 2> varArgs; // of the variadic frames on the stack, innermost last
  // The call to the reach_error being walked, if any: the witness puts a
  // violation inside it at the call, the violation unreach-call names.
  const CallBase *reachError = nullptr;
  DenseMap<const void *, unsigned> claims; // source construct -> index in checks
  DenseMap<const Instruction *, unsigned> assertionNumber;
  DenseMap<const Function *, unsigned> assertionsIn;
  DenseMap<const CallBase *, unsigned> recursionNumber;
  DenseMap<const Function *, unsigned> recursionsIn;
  SmallPtrSet<const Function *, 8> warned;
  // The iteration of a summarised loop being walked: its index, its store,
  // and once walked, the store's guard and bits.
  struct Summarising {
    const Loop *loop;
    SMTExprRef index;
    const StoreInst *store;
    std::pair<SMTExprRef, SMTExprRef> stored;
    unsigned nondets = 0; // nondet calls walked, which names their arrays
  };
  std::optional<Summarising> summary;
  unsigned summaryCount = 0; // names each summary's fresh symbols
  SmallPtrSet<const Loop *, 8> summarised;
};

} // namespace

Error walk(Function &entry, const Options &opts, encode::Encoder &enc, memory::Memory &mem,
           checks::Checks &checks, Trace &trace) {
  try {
    Walk w(opts, enc, mem, checks, trace);
    w.entry(entry);
    w.report();
  } catch (const Unsupported &e) {
    return createStringError(inconvertibleErrorCode(), "unsupported: %s", e.what());
  }
  return Error::success();
}

} // namespace vereda::walker
