#include "walker/walker.h"

#include "analysis/loops.h"
#include "ir/debug.h"
#include "ir/sanitizer.h"
#include "ir/source.h"
#include "util/log.h"
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/DenseSet.h>
#include <llvm/ADT/MapVector.h>
#include <llvm/ADT/PostOrderIterator.h>
#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/Analysis/CaptureTracking.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/ScalarEvolutionExpressions.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/Demangle/Demangle.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Intrinsics.h>
#include <algorithm>
#include <cstdlib>
#include <memory>

using namespace llvm;
using vereda::encode::Term;
using vereda::Unsupported;

namespace vereda::walker {

namespace {

// Per-function CFG facts, computed once.
struct FunctionInfo {
  explicit FunctionInfo(Function &f) : dt(f), li(dt) {
    SmallVector<BasicBlock *, 16> rpo;
    for (BasicBlock *bb : ReversePostOrderTraversal<Function *>(&f))
      rpo.push_back(bb);
    DenseMap<const BasicBlock *, unsigned> index;
    index.reserve(rpo.size());
    for (unsigned i = 0; i < rpo.size(); ++i)
      index[rpo[i]] = i;
    // A retreating edge must be a loop back edge, or the CFG is irreducible.
    for (BasicBlock *bb : rpo)
      for (BasicBlock *succ : successors(bb))
        if (index.lookup(succ) <= index.lookup(bb) && !dt.dominates(succ, bb))
          throw Unsupported("irreducible control flow in " + llvm::demangle(f.getName().str()));
    for (BasicBlock *bb : rpo) {
      Loop *in = li.getLoopFor(bb);
      regionBlocks[in].push_back(bb);
      if (in && in->getHeader() == bb) {
        loopNumber[in] = loopNumber.size();
        regionBlocks[in->getParentLoop()].push_back(bb);
      }
    }
  }

  DominatorTree dt;
  LoopInfo li;
  std::unique_ptr<analysis::Loops> loops;
  DenseMap<const Loop *, SmallVector<BasicBlock *, 8>> regionBlocks;
  DenseMap<const Loop *, unsigned> loopNumber; // ESBMC's "loop N", per function
};

// One way into a block: the edge's guard and the values its phis take on it,
// captured when the edge is taken, since a later iteration rebinds them.
struct Arrival {
  Term guard;
  SmallVector<Term, 2> phiValues;
  memory::State mem;
  encode::Encoder::Facts facts;
};

struct Return {
  Term guard, value;
  memory::State mem;
  encode::Encoder::Facts facts;
};

struct Frame {
  DenseMap<const BasicBlock *, SmallVector<Arrival, 2>> arrivals;
  SmallVector<Return, 2> returns;
  // The iteration being walked of each loop being unrolled.
  DenseMap<const Loop *, unsigned> iteration;
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
  Term regs;  // x86-64: the register save area
  Term stack; // the first variadic argument on the stack
  unsigned gp, fp;  // x86-64: the register save area offsets past the named arguments
};

class Walk {
public:
  Walk(const Module &m, const Options &opts, encode::Encoder &enc, memory::Memory &mem,
       checks::Checks &checks, Trace &trace, bool incremental = false)
      : opts(opts), enc(enc), mem(mem), t(enc.terms()), s(enc.solver()), checks(checks),
        trace(trace), tlii(m.getTargetTriple()), incremental(incremental) {
    // A summary writes the whole array at once, which another thread could
    // see half written.
    const Function *create = m.getFunction("pthread_create");
    threads = create && !create->use_empty();
  }

  void entry(Function &f) {
    for (Argument &arg : f.args())
      enc.bind(&arg, enc.fresh(arg.getType(), "arg_" + arg.getName().str()));
    Module &m = *f.getParent();
    // A global only declared is defined outside the program, which knows
    // its address.
    mem.allocateGlobals(m, [this](const GlobalValue *g) {
      return g->isDeclaration() || exposed(g);
    });
    memory::State st = mem.initial();
    for (const GlobalVariable &g : m.globals())
      if (unsigned bits = ownsArrays(&g, m.getDataLayout()))
        mem.own(st, mem.global(&g), bits);
    mem.initialise(st, m, [this](const Constant *c) { return enc.value(c); });
    errnoObject = m.getNamedGlobal("__vereda_errno");
    for (GlobalVariable &g : m.globals())
      if (g.isDeclaration() && !g.isConstant() && !g.getName().starts_with("__vereda_"))
        externs.push_back(&g);
    function(f, t.boolean(true), st);
  }

  // The number of loops summarised, as ESBMC prints it.
  void report() const {
    if (!summarised.empty())
      util::info() << "Summarised " << summarised.size() << " array loop(s)\n";
  }

  bool resumable() const { return stopped.has_value(); }

  void advance(Function &f, unsigned bound) {
    opts.unwind = bound;
    if (!stopped) {
      entry(f);
      return;
    }
    Stopped saved = std::move(*stopped);
    stopped.reset();
    checks.resize(saved.checks);
    trace.truncate(saved.trace);
    claims.erase({saved.loop, nullptr});
    stack.push_back(&f);
    FunctionInfo &fi = info(f);
    loop(saved.frame, fi, saved.loop, saved.iteration);
    if (!stopped) {
      bool after = false;
      for (BasicBlock *bb : fi.regionBlocks[nullptr]) {
        if (after) {
          if (Loop *l = fi.li.getLoopFor(bb))
            loop(saved.frame, fi, l);
          else
            block(saved.frame, *bb);
          if (stopped)
            break;
        }
        after |= bb == saved.loop->getHeader();
      }
    }
    stack.pop_back();
  }

private:
  // LLVM's default capture-tracking cap (100 uses) is too small for an array
  // written by an unrolled loop, where every store is a use of the alloca.
  static constexpr unsigned MaxCaptureUses = 16384;

  // Whether the address of v, an alloca, a global or a function, may reach
  // code outside the program or an integer, or be compared with one
  // (memory.h, #137): any capture but a test against null (stored, passed to
  // a call, converted to an integer, returned, compared), or it is in a
  // global's initialiser. A constant getelementptr or cast of v is followed
  // as an instruction would be.
  bool exposed(const Value *v) {
    struct Tracker : CaptureTracker {
      bool exposed = false;
      void tooManyUses() override { exposed = true; }
      Action captured(const Use *u, UseCaptureInfo ci) override {
        auto *ce = dyn_cast<ConstantExpr>(u->getUser());
        if (ce && (isa<GEPOperator>(ce) || ce->getOpcode() == Instruction::BitCast ||
                   ce->getOpcode() == Instruction::AddrSpaceCast)) {
          PointerMayBeCaptured(ce, this, MaxCaptureUses);
          return exposed ? Stop : ContinueIgnoringReturn;
        }
        exposed = !isa<Instruction>(u->getUser()) || !capturesAddressIsNullOnly(ci.UseCC);
        return exposed ? Stop : Continue;
      }
    };
    auto [it, inserted] = exposes.try_emplace(v, true);
    if (inserted) {
      Tracker tracker;
      PointerMayBeCaptured(v, &tracker, MaxCaptureUses);
      it->second = tracker.exposed;
    }
    return it->second;
  }

  // Whether the alloca or global v gets arrays of its own (memory.h, #71):
  // a getelementptr reaches it at a symbolic offset, which is where a short
  // chain and a pointer-width index pay, and no pointer to it is stored or
  // passed on (only its provenance counts; comparing its address does not),
  // so few accesses reach it through an unknown pointer, where each object
  // with arrays of its own is one more choice. Returns the bits of their
  // elements (elementBits), or 0 for none.
  unsigned ownsArrays(const Value *v, const DataLayout &dl) {
    auto [it, inserted] = owns.try_emplace(v, 0);
    if (!inserted ||
        capturesAnything(
            PointerMayBeCaptured(v, CaptureComponents::Provenance, capturesAnything, MaxCaptureUses)
                .WithRet))
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

  static bool containsPointer(const Type *ty) {
    if (ty->isPointerTy())
      return true;
    if (auto *st = dyn_cast<StructType>(ty))
      return any_of(st->elements(), containsPointer);
    if (auto *at = dyn_cast<ArrayType>(ty))
      return containsPointer(at->getElementType());
    if (auto *vt = dyn_cast<VectorType>(ty))
      return containsPointer(vt->getElementType());
    return false;
  }

  static bool isMemoryCopyOrSet(const CallBase *c, bool &isCopy) {
    const Function *callee = c->getCalledFunction();
    if (!callee || c->arg_size() < 3 || !c->use_empty())
      return false;
    StringRef name = callee->getName();
    isCopy = name.starts_with("llvm.memcpy") || name.starts_with("llvm.memmove") ||
             name == "memcpy" || name == "memmove";
    return isCopy || name.starts_with("llvm.memset") || name == "memset";
  }

  // Whether v, an alloca or global, can never hold a pointer with provenance:
  // its address is not exposed and does not escape, any global initialiser has
  // no pointer type, and every write reachable through getelementptrs from it
  // stores a non-pointer value, is a memset, or copies from another such object.
  bool holdsNoPointers(const Value *v, SmallPtrSetImpl<const Value *> &visiting) {
    if (!isa<AllocaInst>(v) && !isa<GlobalVariable>(v))
      return false;
    if (!visiting.insert(v).second)
      return true;
    if (auto *gv = dyn_cast<GlobalVariable>(v))
      if (!gv->hasInitializer() || containsPointer(gv->getInitializer()->getType()))
        return false;
    if (exposed(v) ||
        capturesAnything(
            PointerMayBeCaptured(v, CaptureComponents::Provenance, capturesAnything, MaxCaptureUses)
                .WithRet))
      return false;
    SmallVector<const Value *, 8> work{v};
    SmallPtrSet<const Value *, 8> seen{v};
    while (!work.empty()) {
      const Value *p = work.pop_back_val();
      for (const User *u : p->users()) {
        if (isa<ICmpInst>(u) || isa<LoadInst>(u))
          continue;
        if (auto *g = dyn_cast<GEPOperator>(u)) {
          if (seen.insert(g).second)
            work.push_back(g);
          continue;
        }
        if (auto *s = dyn_cast<StoreInst>(u)) {
          if (s->getValueOperand() == p || containsPointer(s->getValueOperand()->getType()))
            return false;
          continue;
        }
        if (auto *c = dyn_cast<CallBase>(u)) {
          if (c->getCalledFunction() &&
              c->getCalledFunction()->getName() == "__vereda_uninitialised")
            continue;
          bool isCopy = false;
          if (isMemoryCopyOrSet(c, isCopy)) {
            if (!isCopy && c->getArgOperand(0) == p)
              continue;
            if (isCopy && (c->getArgOperand(0) == p || c->getArgOperand(1) == p)) {
              if (c->getArgOperand(0) == p &&
                  !holdsNoPointers(getUnderlyingObject(c->getArgOperand(1)), visiting))
                return false;
              continue;
            }
          }
        }
        return false;
      }
    }
    return true;
  }

  // N for N = 16, 32 or 64 when every use of v and of the getelementptrs
  // from it is a load or store of iN at an offset that is a multiple of N/8,
  // a comparison, the no-op __vereda_uninitialised, or a memcpy/memmove/memset
  // of a constant multiple of N/8 bytes (issue #107), with any copy into v
  // coming from an object that holds no pointers: its own arrays then hold
  // whole elements. 8, for bytes, otherwise.
  unsigned elementBits(const Value *v, const DataLayout &dl) {
    if (auto *gv = dyn_cast<GlobalVariable>(v);
        gv && gv->hasInitializer() && containsPointer(gv->getInitializer()->getType()))
      return 8;
    unsigned bits = 0;
    auto access = [&](Type *ty) {
      if (!ty->isIntegerTy() || (bits && !ty->isIntegerTy(bits)))
        return false;
      bits = ty->getIntegerBitWidth();
      return true;
    };
    SmallVector<const Value *, 8> work{v};
    SmallPtrSet<const Value *, 8> seen{v};
    SmallVector<const GEPOperator *, 8> geps;
    SmallVector<const ConstantInt *, 4> lengths;
    while (!work.empty()) {
      const Value *p = work.pop_back_val();
      for (const User *u : p->users()) {
        bool ok = isa<ICmpInst>(u) || isa<GEPOperator>(u); // a pointer is no index
        if (auto *l = dyn_cast<LoadInst>(u))
          ok = access(l->getType());
        else if (auto *s = dyn_cast<StoreInst>(u))
          ok = s->getValueOperand() != p && access(s->getValueOperand()->getType());
        else if (auto *g = dyn_cast<GEPOperator>(u)) {
          if (seen.insert(g).second) {
            geps.push_back(g);
            work.push_back(g);
          }
        } else if (auto *c = dyn_cast<CallBase>(u)) {
          bool isCopy = false;
          if (c->getCalledFunction() &&
              c->getCalledFunction()->getName() == "__vereda_uninitialised") {
            ok = true;
          } else if (isMemoryCopyOrSet(c, isCopy)) {
            auto *len = dyn_cast<ConstantInt>(c->getArgOperand(2));
            bool isDst = c->getArgOperand(0) == p;
            bool isSrc = isCopy && c->getArgOperand(1) == p;
            if (len && (isDst || isSrc)) {
              ok = true;
              if (isCopy && isDst) {
                SmallPtrSet<const Value *, 8> visiting{v};
                ok = holdsNoPointers(getUnderlyingObject(c->getArgOperand(1)), visiting);
              }
              if (ok)
                lengths.push_back(len);
            }
          }
        }
        if (!ok)
          return 8;
      }
    }
    if (bits != 16 && bits != 32 && bits != 64)
      return 8;
    // Each getelementptr moves by a multiple of N/8: its constant offset
    // and the scale of each of its indices have that many low zero bits.
    unsigned shift = Log2_32(bits / 8);
    for (const ConstantInt *len : lengths)
      if (len->getValue().countr_zero() < shift)
        return 8;
    for (const GEPOperator *g : geps) {
      SmallMapVector<Value *, APInt, 4> scales;
      APInt constant(dl.getIndexTypeSizeInBits(g->getType()), 0);
      if (!g->collectOffset(dl, constant.getBitWidth(), scales, constant) ||
          constant.countr_zero() < shift || any_of(scales, [&](const auto &s) {
            return s.second.countr_zero() + computeKnownBits(s.first, dl).countMinTrailingZeros() <
                   shift;
          }))
        return 8;
    }
    return bits;
  }

  FunctionInfo &info(Function &f) {
    std::unique_ptr<FunctionInfo> &fi = infos[&f];
    if (!fi)
      fi = std::make_unique<FunctionInfo>(f);
    return *fi;
  }

  analysis::Loops &loopsFor(FunctionInfo &fi, Function &f) {
    if (!fi.loops)
      fi.loops = std::make_unique<analysis::Loops>(
          f, fi.dt, fi.li, tlii,
          analysis::Counted{opts.assertions && counts(false), counts(true),
                            opts.property == Property::UnreachCall, counts(false)},
          threads);
    return *fi.loops;
  }

  // Walks f's body from a call guarded by guard with memory st. Returns the
  // frame's return arrivals.
  Frame function(Function &f, Term guard, const memory::State &st) {
    if (t.isFalse(guard))
      return Frame();
    Frame frame;
    frame.arrivals[&f.getEntryBlock()].push_back({guard, {}, st, enc.facts});
    stack.push_back(&f);
    region(frame, info(f), nullptr);
    stack.pop_back();
    return frame;
  }

  // Walks the blocks of loop l, or of the whole function when l is null, in
  // reverse post-order with each inner loop collapsed to its header.
  void region(Frame &frame, FunctionInfo &fi, Loop *l) {
    auto it = fi.regionBlocks.find(l);
    if (it == fi.regionBlocks.end())
      return;
    for (BasicBlock *bb : it->second) {
      Loop *inner = fi.li.getLoopFor(bb);
      if (inner == l)
        block(frame, *bb);
      else
        loop(frame, fi, inner);
      if (stopped)
        return;
    }
  }

  // Whether no path leaves l and nothing it does can be seen: each edge out
  // of it is a branch on a constant that never takes it, and no instruction
  // in it has a side effect or calls anything but an intrinsic without one.
  // A check is a call or an edge out of l (to an assert's, a trap's or an
  // unreachable's block), so l has none. ESBMC turns such a loop,
  // `while (1);`, into assume(0).
  static bool endless(const Loop *l) {
    for (BasicBlock *bb : l->blocks()) {
      auto *br = dyn_cast<CondBrInst>(bb->getTerminator());
      auto *c = br ? dyn_cast<ConstantInt>(br->getCondition()) : nullptr;
      for (BasicBlock *succ : successors(bb))
        if (!l->contains(succ) && (!c || br->getSuccessor(c->isZero()) == succ))
          return false;
      for (Instruction &i : *bb)
        if (i.mayHaveSideEffects() || (isa<CallBase>(i) && !isa<IntrinsicInst>(i)))
          return false;
    }
    return true;
  }

  // Back-edge count up to which a constant-count loop skips unreachable()
  // during unrolling without --unwind, letting term folding end it instead.
  static constexpr uint64_t SmallConstantLoop = 64;

  void loop(Frame &frame, FunctionInfo &fi, Loop *l, unsigned start = 0) {
    BasicBlock *header = l->getHeader();
    // The paths that reach an endless loop end there, as at unreachable,
    // and its header keeps the IR, debug info and loop numbers as they were.
    if (endless(l)) {
      frame.arrivals.erase(header);
      return;
    }
    if (!start && opts.summarise && summarise(frame, fi, l))
      return;
    unsigned number = fi.loopNumber.lookup(l);
    // When SCEV knows the loop takes its back edge a small constant number of
    // times, folding alone normally ends the unrolling; asking the solver on
    // each iteration up to that bound only wastes queries when the entry guard
    // is symbolic. Past the bound, unreachable() still runs in case folding
    // did not settle the exit condition.
    uint64_t knownBackedges = 0;
    if (!opts.unwind)
      if (auto *n = dyn_cast<SCEVConstant>(
              loopsFor(fi, *header->getParent()).scev().getBackedgeTakenCount(l)))
        if (n->getAPInt().ule(SmallConstantLoop))
          knownBackedges = n->getAPInt().getZExtValue();
    std::string locDisplay;
    if (opts.showIterations)
      locDisplay = loopLocation(l).display();
    for (unsigned i = start;; ++i) {
      auto it = frame.arrivals.find(header);
      if (it == frame.arrivals.end() || it->second.empty())
        return;
      if (i > 0) {
        Term reached = anyGuard(it->second);
        if (opts.unwind && i >= opts.unwind) {
          size_t beforeChecks = checks.size(), beforeTrace = trace.size();
          unwound(Unwound::Loop, number, loopLocation(l), reached, l);
          // Every path runs l past the bound: until its last iteration, a
          // larger bound only adds iterations of l.
          if (auto c = claims.find({l, nullptr}); c != claims.end() && t.isTrue(reached))
            if (auto *n = dyn_cast<SCEVConstant>(
                    loopsFor(fi, *header->getParent()).scev().getBackedgeTakenCount(l)))
              checks[c->second].unwindsAt = n->getAPInt().getLimitedValue(UINT_MAX - 1) + 1;
          // Nothing after this prefix has been walked. Keep its live header
          // instead of rebuilding it at the next bound; exits need fresh walks.
          if (incremental && !l->getParentLoop() && stack.size() == 1 &&
              frame.arrivals.size() == 1 && frame.returns.empty() && t.isTrue(reached) &&
              checks.size() == beforeChecks + 1) {
            stopped = Stopped{std::move(frame), l, i, beforeChecks, beforeTrace};
            return;
          }
          it->second.clear();
          return;
        }
        // Without --unwind, whether another iteration is reachable; term
        // folding settles most of these without the solver.
        if (!opts.unwind && i > knownBackedges && unreachable(reached)) {
          it->second.clear();
          return;
        }
        if (opts.showIterations)
          util::progress(Twine("Unwinding loop ") + Twine(number) + " iteration " + Twine(i) +
                         "   " + locDisplay);
      }
      frame.iteration[l] = i;
      region(frame, fi, l);
      frame.iteration.erase(l);
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
    analysis::Loops &loops = loopsFor(fi, *header->getParent());
    const analysis::Summary *found = loops.summary(l);
    // One summary at a time: a loop in a function the iteration calls is unrolled.
    if (summary || !found || in == frame.arrivals.end() || in->second.empty() ||
        (found->nondet && opts.witness))
      return false;
    analysis::Summary sum = *found;
    ScalarEvolution &se = loops.scev();
    // SCEV sees through the LCSSA phi that carries a value out of an
    // earlier loop, to an AddRec of that loop or a value in it: the phi's
    // term is what either holds as l is entered. A constant keeps its own
    // term, which folds.
    inputs.clear();
    for (BasicBlock *bb : l->blocks())
      for (Instruction &inst : *bb)
        for (Value *op : inst.operands())
          if (auto *i = dyn_cast<Instruction>(op);
              i && current(fi.li, i, l) && se.isSCEVable(i->getType()))
            if (const SCEV *e = se.getSCEV(i); !isa<SCEVConstant>(e))
              if (Term v = enc.bound(i))
                inputs.try_emplace(e, v);
    Term trips = evaluate(frame, fi, sum.trips, l, {});
    if (!trips)
      return false;
    // A loop that only checks or counts, a few times, unrolls into reads at
    // constant indices that fold, where a summary leaves the solver a read
    // at a nondet index (survey 2026-11-23-array-loop-summaries).
    if (const APInt *n = t.constant(trips); !sum.store && n && n->ule(UnrolledChecks))
      return false;
    Term k = t.symbol("loop_index#" + std::to_string(summaryCount),
                            s.mkBVSort(trips.width()));
    // The header's phis in iteration k and after the last one, and the
    // values the exit takes from the header.
    SmallVector<Term, 2> first;
    SmallVector<std::pair<Value *, Term>, 4> last;
    auto at = [&](Value *v, const Term &n) {
      return se.isSCEVable(v->getType()) ? evaluate(frame, fi, se.getSCEV(v), l, n) : Term();
    };
    for (PHINode &phi : header->phis()) {
      first.push_back(at(&phi, k));
      last.push_back({&phi, at(&phi, trips)});
    }
    for (PHINode &phi : sum.exit->phis())
      if (auto *v = dyn_cast<Instruction>(phi.getIncomingValueForBlock(header)); v && l->contains(v))
        last.push_back({v, at(v, trips)});
    Term start;
    if (sum.store) {
      start = evaluate(frame, fi, sum.address->getStart(), l, {});
      const APInt *object = start ? t.constant(mem.object(start)) : nullptr;
      if (!object || object->isZero())
        return false;
    }
    if (llvm::any_of(first, [](const Term &v) { return !v; }) ||
        llvm::any_of(last, [](const auto &v) { return !v.second; }))
      return false;

    SmallVector<Arrival, 2> arrivals = std::move(in->second);
    frame.arrivals.erase(header);
    Term guard = anyGuard(arrivals);
    memory::State st = merged(arrivals);
    // The iteration's facts hold only in it, and the loop may not run.
    encode::Encoder::Facts facts = common(arrivals), inside = facts;
    Term inRange = t.ult(k, trips), body = t.land(guard, inRange);
    enc.learn(inRange, inside); // k < trips holds in the iteration
    // Built after body, which keeps k < trips: every term with k counts only
    // under body, or as an element the loop wrote, n < trips, once
    // substitute puts n for k; there, terms fold as if it held.
    if (const APInt *n = t.constant(trips); n && !n->isZero() && n->getActiveBits() <= 64)
      t.bound(k, n->getZExtValue());
    if (!t.isFalse(body)) {
      frame.arrivals[header].push_back({body, first, st, inside});
      summary = Summarising{l, k, sum.store, {}, 0};
      region(frame, fi, l);
      // Drop the back edge and the iteration's own exit.
      frame.arrivals.erase(header);
      frame.arrivals.erase(sum.exit);
      auto [stored, bits] = summary->stored;
      summary.reset();
      if (stored) {
        unsigned width = k.width(), ptrBits = mem.pointerBits();
        mem.summarise(st, start, sum.stride, sum.store->getValueOperand()->getType(),
                      enc.resize(trips, ptrBits, false),
                      [&t = t, k, width, ptrBits, stored, bits](const Term &n) {
                        Term i = width < ptrBits   ? t.extract(n, width - 1, 0)
                                       : width > ptrBits ? t.zext(n, width - ptrBits)
                                                         : n;
                        return std::make_pair(t.substitute(stored, k, i), t.substitute(bits, k, i));
                      });
      }
    }
    for (auto &[v, term] : last)
      enc.bind(v, term);
    enc.facts = facts;
    arrive(frame, header, sum.exit, guard, st);
    ++summaryCount;
    summarised.insert(l);
    return true;
  }

  // Whether the term bound to i is its value as l is entered: i is outside
  // l and in no loop l is not in, whose iterations rebind it.
  static bool current(const LoopInfo &li, const Instruction *i, const Loop *l) {
    const Loop *in = li.getLoopFor(i->getParent());
    return !l->contains(i) && (!in || in->contains(l));
  }

  // The SCEV e in iteration k of l, counted from 0 as the header is
  // reached, or null for a form not handled here, a value of another loop
  // that is not one of l's inputs, or an AddRec of l when k is null.
  Term evaluate(const Frame &frame, FunctionInfo &fi, const SCEV *e, const Loop *l,
                      const Term &k) {
    ScalarEvolution &se = fi.loops->scev();
    unsigned w = se.getTypeSizeInBits(e->getType()), ptrBits = mem.pointerBits();
    bool pointer = e->getType()->isPointerTy();
    if (w == 1) // an i1 is a Bool term
      return {};
    if (Term v = inputs.lookup(e))
      return v;
    SmallVector<Term, 4> ops;
    for (const SCEV *op : e->operands()) {
      ops.push_back(evaluate(frame, fi, op, l, k));
      if (!ops.back())
        return {};
    }
    auto fold = [&](auto f) {
      Term r = ops[0];
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
      if (auto *i = dyn_cast<Instruction>(v); i && !current(fi.li, i, l))
        return {};
      return isa<Instruction>(v) || isa<Argument>(v) ? enc.bound(v) : enc.value(v);
    }
    case scPtrToInt:
    case scPtrToAddr:
      return enc.resize(mem.address(ops[0]), w, false);
    case scTruncate:
    case scZeroExtend:
      return enc.resize(ops[0], w, false);
    case scSignExtend:
      return enc.resize(ops[0], w, true);
    case scAddExpr: {
      if (!pointer)
        return fold([&](auto a, auto b) { return t.add(a, b); });
      Term p, delta = t.bv(0, ptrBits);
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
      return pointer ? Term() : fold([&](auto a, auto b) { return t.ite(t.sgt(a, b), a, b); });
    case scUMaxExpr:
      return pointer ? Term() : fold([&](auto a, auto b) { return t.ite(t.ugt(a, b), a, b); });
    case scSMinExpr:
      return pointer ? Term() : fold([&](auto a, auto b) { return t.ite(t.slt(a, b), a, b); });
    case scUMinExpr:
    case scSequentialUMinExpr:
      return pointer ? Term() : fold([&](auto a, auto b) { return t.ite(t.ult(a, b), a, b); });
    case scAddRecExpr: {
      auto *ar = cast<SCEVAddRecExpr>(e);
      // An enclosing loop's AddRec takes the iteration of it being walked.
      Term n = k;
      if (ar->getLoop() != l) {
        auto it = frame.iteration.find(ar->getLoop());
        n = it == frame.iteration.end() ? Term() : t.bv(APInt(64, it->second));
      }
      if (!ar->isAffine() || !n)
        return {};
      Term start = ops[0], step = ops[1];
      if (pointer)
        return mem.advance(start, t.mul(enc.resize(step, ptrBits, true), enc.resize(n, ptrBits, false)));
      return t.add(start, t.mul(step, enc.resize(n, w, false)));
    }
    default:
      return {};
    }
  }

  enum class Unwound { Loop, Recursion };

  // The bound was hit with guard reachable: an unwinding assertion, or an
  // assumption that cuts the path. key is the loop or the recursive call.
  void unwound(Unwound kind, unsigned number, const ir::SourceLoc &loc, Term reached,
               const void *key) {
    if (!opts.unwindingAssertions)
      return;
    bool isLoop = kind == Unwound::Loop;
    std::string comment = isLoop ? "unwinding assertion loop " + std::to_string(number)
                                 : "recursion unwinding assertion";
    claim(isLoop ? checks::Property::Unwinding : checks::Property::Recursion, loc, comment, "",
          number, reached, {key, nullptr});
  }

  static ir::SourceLoc loopLocation(Loop *l) {
    DebugLoc loc = l->getStartLoc();
    return ir::location(loc, l->getHeader()->getParent()->getName());
  }

  // Whether no path satisfies guard. A constant needs no solver call; a
  // counting loop whose condition folds would otherwise ask one per
  // iteration. Answers are memoised by guard id: UNSAT stays valid as
  // constraints are only added, while SAT is dropped whenever a new global
  // constraint advances Terms::constraintsEpoch().
  bool unreachable(Term guard) {
    if (t.isTrue(guard))
      return false;
    if (t.isFalse(guard))
      return true;
    const void *id = guard.id();
    if (unsatGuards.contains(id))
      return true;
    uint64_t epoch = t.constraintsEpoch();
    if (epoch != satEpoch) {
      satGuards.clear();
      satEpoch = epoch;
    } else if (satGuards.contains(id)) {
      return false;
    }
    bool unsat = s.checkSatAssuming({t.solverTerm(guard)}) == camada::CheckResult::UNSAT;
    if (unsat)
      unsatGuards.insert(id);
    else
      satGuards.insert(id);
    return unsat;
  }

  Term anyGuard(const SmallVectorImpl<Arrival> &arrivals) {
    Term g = arrivals[0].guard;
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
    auto it = frame.arrivals.find(&bb);
    if (it == frame.arrivals.end() || it->second.empty()) // no edge into bb was taken in this pass
      return;
    SmallVector<Arrival, 2> arrivals = std::move(it->second);
    frame.arrivals.erase(it);
    Term guard = anyGuard(arrivals);
    enc.facts = common(arrivals);
    memory::State st = merged(arrivals);

    unsigned p = 0;
    for (PHINode &phi : bb.phis()) {
      Term r = arrivals[0].phiValues[p];
      for (unsigned i = 1; i < arrivals.size(); ++i)
        r = t.ite(arrivals[i].guard, arrivals[i].phiValues[p], r);
      enc.bind(&phi, r);
      ++p;
    }

    mem.setContext(guard);
    for (Instruction &inst : bb) {
      for (const DbgRecord &rec : inst.getDbgRecordRange())
        if (std::optional<ir::DbgAssign> a = ir::dbgAssign(rec))
          if (Term v = traceValue(a->value))
            trace.add(Step::Assignment, guard, a->loc, a->name, v, a->isUnsigned);
      if (isa<PHINode>(inst))
        continue;
      if (inst.isTerminator())
        terminator(frame, inst, guard, st);
      else if (auto *c = dyn_cast<CallBase>(&inst)) {
        guard = call(*c, guard, st);
        if (t.isFalse(guard))
          break;
        mem.setContext(guard); // the callee's blocks set their own
      }
      else {
        if (!memoryAccess(inst, guard, st))
          enc.encode(inst);
        if (t.isFalse(guard))
          break;
      }
    }
  }

  // A debug record's value for the trace, or null when it has none yet.
  // LCSSA does not route debug uses, so a record can name a value from a
  // block not walked in this pass; the trace skips it.
  Term traceValue(const Value *v) {
    if (!v->getType()->isIntegerTy())
      return {};
    if (isa<Instruction>(v) || isa<Argument>(v))
      return enc.bound(v);
    return isa<Constant>(v) ? enc.value(v) : Term();
  }

  // The facts (encoder.h) that hold on every one of paths: what a join keeps.
  template <typename Paths> static encode::Encoder::Facts common(const Paths &paths) {
    using encode::Encoder;
    Encoder::Facts f = paths[0].facts;
    for (const auto &p : drop_begin(paths)) {
      Encoder::Facts was = f; // keeps the nodes diff walks while f changes
      Encoder::Facts::diff(was, p.facts, [&](auto k, const Encoder::Fact *a, const Encoder::Fact *b) {
        if (!a)
          return;
        if (std::optional<Encoder::Fact> both = b ? Encoder::join(*a, *b) : std::nullopt)
          f.set(k, *both);
        else
          f.eraseObject(k.first);
      });
    }
    return f;
  }

  // Takes the edge from pred to succ, on which edge holds when it is given.
  void arrive(Frame &frame, BasicBlock *pred, BasicBlock *succ, Term guard,
              const memory::State &st, Term edge = {}) {
    if (t.isFalse(guard)) // an edge folded away is not walked
      return;
    Arrival a{guard, {}, st, enc.facts};
    if (edge)
      enc.learn(edge, a.facts);
    std::swap(enc.facts, a.facts); // the phis take their values on the edge
    for (PHINode &phi : succ->phis())
      a.phiValues.push_back(enc.value(phi.getIncomingValueForBlock(pred)));
    std::swap(enc.facts, a.facts);
    frame.arrivals[succ].push_back(std::move(a));
  }

  void terminator(Frame &frame, Instruction &term, Term guard, const memory::State &st) {
    BasicBlock *bb = term.getParent();
    if (auto *br = dyn_cast<UncondBrInst>(&term)) {
      arrive(frame, bb, br->getSuccessor(), guard, st);
    } else if (auto *br = dyn_cast<CondBrInst>(&term)) {
      if (br->getSuccessor(0) == br->getSuccessor(1)) {
        arrive(frame, bb, br->getSuccessor(0), guard, st);
        return;
      }
      Term c = enc.value(br->getCondition());
      Term taken[2] = {t.land(guard, c), t.land(guard, t.lnot(c))};
      if (opts.witness)
        if (std::optional<ir::BranchSite> site = ir::branchSite(*br))
          for (unsigned i = 0; i < 2; ++i)
            if (site->taken[i] && !t.isFalse(taken[i]))
              trace.add(Step::Branch, taken[i], site->loc, *site->taken[i] ? "true" : "false");
      arrive(frame, bb, br->getSuccessor(0), taken[0], st, c);
      arrive(frame, bb, br->getSuccessor(1), taken[1], st, t.lnot(c));
    } else if (auto *sw = dyn_cast<SwitchInst>(&term)) {
      Term cond = enc.value(sw->getCondition());
      if (const APInt *v = t.constant(cond)) {
        auto *ci = ConstantInt::get(sw->getContext(), *v);
        arrive(frame, bb, sw->findCaseValue(ci)->getCaseSuccessor(), guard, st);
        return;
      }
      Term anyCase = t.boolean(false);
      MapVector<BasicBlock *, Term> taken; // in case order, for a stable formula
      for (const auto &c : sw->cases()) {
        Term eq = t.eq(cond, enc.value(c.getCaseValue()));
        anyCase = t.lor(anyCase, eq);
        Term &to = taken[c.getCaseSuccessor()];
        to = to ? t.lor(to, eq) : eq;
      }
      Term &d = taken[sw->getDefaultDest()];
      d = d ? t.lor(d, t.lnot(anyCase)) : t.lnot(anyCase);
      for (auto &[succ, to] : taken)
        arrive(frame, bb, succ, t.land(guard, to), st, to);
    } else if (auto *ret = dyn_cast<ReturnInst>(&term)) {
      Value *v = ret->getReturnValue();
      frame.returns.push_back({guard, v ? enc.value(v) : Term(), st, enc.facts});
    } else if (isa<UnreachableInst>(term)) {
      // Clang ends a block with unreachable after a call that never returns:
      // the path ends there. Anywhere else it is __builtin_unreachable,
      // which ESBMC checks by default.
      auto *prev = dyn_cast_or_null<CallBase>(term.getPrevNode());
      if ((!prev || !prev->doesNotReturn()) && opts.assertions && counts(false))
        assertion(term, guard, "__builtin_unreachable", "");
    } else if (isa<InvokeInst>(term)) {
      throw Unsupported("exception unwinding (invoke) is not supported at " + ir::location(term).str() +
                        "; for Rust, compile with -C panic=abort");
    } else {
      throw Unsupported(std::string("terminator ") + term.getOpcodeName());
    }
  }

  // Returns the path condition after the call: unchanged, strengthened by an
  // assume, or false when the callee does not return.
  Term call(CallBase &c, Term guard, memory::State &st) {
    // Inline asm that ir did not turn into IR. As GCC's contract for asm
    // has it, it writes its outputs and no other memory, unless it clobbers
    // "memory". Its register outputs get nondet values; one that may write
    // memory is refused when reached, as nothing yet gives memory nondet
    // contents.
    if (auto *ia = dyn_cast<InlineAsm>(c.getCalledOperand())) {
      for (const InlineAsm::ConstraintInfo &ci : ia->ParseConstraints())
        if ((ci.Type == InlineAsm::isOutput && ci.isIndirect) ||
            (ci.Type == InlineAsm::isClobber && ci.Codes[0] == "{memory}"))
          throw Unsupported("inline asm that may write memory at " + ir::location(c).display());
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
        throw Unsupported("indirect call at " + ir::location(c).display());
      if (!callee->isDeclaration())
        throw Unsupported("call to " + llvm::demangle(callee->getName().str()) +
                          " with a mismatched signature at " + ir::location(c).display());
    }
    StringRef name = callee->getName();
    auto tag = [&] { return name.str() + "@" + ir::location(c).str(); };

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
    if (name == "__ubsan_handle_divrem_overflow_abort") {
      Term divisor = enc.value(c.getArgOperand(2));
      unsigned width = ir::divremOperandBits(c);
      if (!width)
        throw Unsupported("integer division sanitizer with an unknown type descriptor");
      if (width > mem.pointerBits())
        divisor = mem.load(st, mem.fromAddress(divisor), IntegerType::get(c.getContext(), width));
      Term zero = t.eq(divisor, t.bv(0, divisor.width()));
      if (counts(false) && !t.isFalse(zero))
        assertion(c, t.land(guard, zero), "division by zero", "", c.getArgOperand(2),
                  checks::Property::DivisionByZero);
      if (counts(true) && !t.isTrue(zero))
        assertion(c, t.land(guard, t.lnot(zero)), "arithmetic overflow on division", "", nullptr,
                  checks::Property::ArithmeticOverflow);
      return t.boolean(false);
    }
    // Rust lowers assertions and checked arithmetic and indexing to panic calls.
    // Recognise Rust symbols only, including when their bodies are linked in.
    if (name.starts_with("_R") || name.starts_with("_ZN")) {
      char *raw = llvm::rustDemangle(name.str());
      std::string demangled = raw ? raw : "";
      std::free(raw);
      // Older rustc uses Itanium-style symbols ending in a Rust hash.
      if (demangled.empty() && name.starts_with("_ZN") && name.size() >= 20 &&
          name.substr(name.size() - 20, 3) == "17h" && name.ends_with("E") &&
          name.substr(name.size() - 17, 16).find_first_not_of("0123456789abcdef") == StringRef::npos)
        demangled = llvm::demangle(name.str());
      StringRef symbol(demangled);
      bool library = symbol.starts_with("core::") || symbol.starts_with("alloc::") ||
                     symbol.starts_with("std::");
      StringRef entry = symbol.split("::h").first.split("::<").first;
      bool exits = entry == "std::process::exit" || entry == "std::process::abort" ||
                   entry == "core::intrinsics::abort";
      bool stops = c.doesNotReturn() || isa_and_nonnull<UnreachableInst>(c.getNextNode());
      if (exits)
        return t.boolean(false);
      if ((library && callee->isDeclaration() && stops) ||
          symbol.starts_with("core::panicking::panic") ||
          symbol.starts_with("core::panicking::assert_failed") ||
          symbol.starts_with("std::panicking::begin_panic") ||
          symbol.starts_with("std::panicking::rust_panic_with_hook") ||
          symbol == "std::panicking::panic" || symbol.starts_with("std::panicking::panic::")) {
        bool overflow = symbol.contains("overflow");
        bool bounds = symbol.contains("bounds_check") ||
                      (symbol.contains("index") && symbol.contains("fail")) ||
                      symbol.contains("slice_error_fail");
        const char *message = overflow ? "arithmetic overflow"
                              : bounds ? "array bounds violated" : "Rust panic";
        checks::Property kind = overflow ? checks::Property::ArithmeticOverflow
                                : bounds ? checks::Property::ArrayBounds
                                         : checks::Property::Assertion;
        if (counts(overflow) && (overflow || bounds || opts.assertions))
          assertion(c, guard, message, "", nullptr, kind);
        return t.boolean(false);
      }
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
      throw Unsupported("call to " + llvm::demangle(name.str()) + " at " +
                        ir::location(c).display());
    // Threads are not modelled yet (#49): the thread function would never
    // run, so a violation only it reaches would be missed.
    if (name == "pthread_create")
      throw Unsupported("call to " + llvm::demangle(name.str()) + " at " +
                        ir::location(c).display());
    // These write a formatted string, or what they scan, through a va_list,
    // which a bodyless call does not do (models/printf.c has the ones that
    // print to memory), so they refuse too.
    if (writesThroughVaList(name))
      throw Unsupported("call to " + llvm::demangle(name.str()) + " at " +
                        ir::location(c).display());
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
                        llvm::demangle(c.getFunction()->getName().str()));
    if (name == "malloc" || name == "calloc") {
      allocateHeap(c, name, guard, st);
      return guard;
    }
    if (memoryFunction(c, name, guard, st))
      return guard;

    // Bodyless verifier functions, as ESBMC treats them by default: a defined
    // one is an ordinary call, except reach_error under unreach-call, above.
    if (name == "__VERIFIER_nondet_memory") {
      Term p = enc.value(c.getArgOperand(0));
      uint64_t n = constantSize(c.getArgOperand(1), guard, name);
      if (n)
        writes(c, guard, p);
      mem.havoc(st, p, n);
      return guard;
    }
    if (name.starts_with("__VERIFIER_nondet_")) {
      // In a summarised iteration, element k of values fresh per iteration,
      // an array per call: camada gives two symbols of one name one term, and
      // two calls can share a location (a macro, IR without debug info).
      Term v =
          summary && summary->loop->contains(&c)
              ? t.select(t.symbol(tag() + "#" + std::to_string(summaryCount) + "." +
                                      std::to_string(summary->nondets++),
                                  s.mkArraySort(s.mkBVSort(summary->index.width()),
                                                enc.sort(c.getType()))),
                         summary->index)
              : enc.fresh(c.getType(), tag());
      enc.bind(&c, v);
      // For the witness, which reads the value's type from the callee: not
      // for a call whose result type differs from it. One declared without
      // a prototype, as in many SV-COMP tasks, is variadic and called
      // without the "...", but returns the same type.
      if (opts.witness && c.getType() == callee->getReturnType())
        trace.add(Step::Call, guard, ir::location(c), name, v);
      return guard;
    }
    if (name == "__VERIFIER_assume") {
      trace.add(Step::Assumption, guard, ir::location(c), ir::callArgumentText(c));
      Term holds = enc.isTrue(c.getArgOperand(0));
      enc.learn(holds, enc.facts);
      return t.land(guard, holds);
    }
    if (name == "__vereda_check_divisor") {
      Term holds = enc.isTrue(c.getArgOperand(0));
      if (t.isTrue(holds))
        return guard;
      if (counts(false))
        assertion(c, t.land(guard, t.lnot(holds)), "division by zero", "", nullptr,
                  checks::Property::DivisionByZero);
      enc.learn(holds, enc.facts);
      return t.land(guard, holds);
    }
    if (name == "__assert_fail" || name == "reach_error" || name == "__VERIFIER_error") {
      if (opts.assertions && counts(false))
        assertion(c, guard, name.str(), "");
      return t.boolean(false);
    }
    if (name == "__VERIFIER_assert" || (name == "assert" && c.arg_size() == 1)) {
      if (!opts.assertions)
        return guard;
      // assert without <assert.h> is an implicitly declared function; ESBMC's
      // front end treats it as the assertion it was meant to be.
      Term holds = enc.isTrue(c.getArgOperand(0));
      if (!counts(false)) {
        enc.learn(holds, enc.facts);
        return t.land(guard, holds);
      }
      assertion(c, t.land(guard, t.lnot(holds)), name.str(), ir::callArgumentText(c));
      return guard;
    }
    if (name == "__vereda_object_size") { // for the realloc model
      Term p = enc.value(c.getArgOperand(0));
      enc.bind(&c, enc.resize(t.sub(mem.size(mem.object(p)), mem.offset(p)),
                              c.getType()->getIntegerBitWidth(), false));
      return guard;
    }
    if (name == "__vereda_uninitialised") // keeps LLVM from folding reads of the local
      return guard;
    if (name == "__vereda_havoc") { // for the models: the object p points into is unknown
      writes(c, guard, enc.value(c.getArgOperand(0)));
      mem.havoc(st, enc.value(c.getArgOperand(0)));
      return guard;
    }
    if (name == "abort" || name == "exit" || name == "_Exit")
      return t.boolean(false);
    // A struct result stays unsupported: the bodyless functions returning
    // one are mostly libgcc's complex arithmetic (__muldc3), whose result C
    // defines, so a nondet one would give wrong FAILEDs.
    if (c.getType()->isStructTy())
      throw Unsupported("call to " + llvm::demangle(name.str()) + " returning a struct at " +
                        ir::location(c).display());
    // An unknown external returns an unconstrained value and may write all
    // of each object it gets a pointer to (read, fgets, ioctl), except a
    // constant and what its attributes, which LLVM infers for the C library,
    // say it only reads. With a constant format without positional
    // arguments, scanf writes its variadic arguments as scan says, and
    // printf none unless the format has a %n. Unless its attributes say it
    // writes only its arguments' memory, it may also write errno, every
    // global the program declares without defining (optind, environ, a
    // variable of a file not given) and the memory outside the program, as
    // they belong to code Vereda does not see, and the objects reached
    // through pointers stored in an object it gets (#114), which LLVM
    // counts as such other memory too. The mutex and condition-variable
    // calls write only the lock or condition they get, which the program
    // does not read, so they write nothing here and keep the rest of an
    // object holding one.
    if (warned.insert(callee).second)
      util::warning("no body for function " + llvm::demangle(name.str()));
    unsigned params = c.getFunctionType()->getNumParams(), end = c.arg_size();
    StringRef f;
    bool format = c.getFunctionType()->isVarArg() && params &&
                  getConstantStringInfo(c.getArgOperand(params - 1), f) && !f.contains('$');
    bool scanf = format && name.ends_with("scanf");
    if (scanf || (format && name.ends_with("printf") &&
                  none_of(conversions(f), [](const Conversion &cv) { return cv.conv == 'n'; })))
      end = params;
    if (!c.onlyReadsMemory() && !writesOnlyLock(name)) {
      MemoryEffects effects = c.getMemoryEffects();
      bool other = isModSet(effects.getModRef(IRMemLocation::Other));
      // A pointer it only reads through may still hold pointers it follows.
      SmallVector<Term, 8> written, read;
      for (unsigned i = 0; i < end; ++i) {
        Value *a = c.getArgOperand(i);
        auto *g = dyn_cast<GlobalVariable>(a->stripPointerCasts());
        if (a->getType()->isPointerTy())
          (c.onlyReadsMemory(i) || (g && g->isConstant()) ? read : written)
              .push_back(enc.value(a));
      }
      if (other) {
        for (GlobalVariable *g : externs)
          written.push_back(enc.value(g));
        written.push_back(mem.outside());
      }
      mem.havocCall(st, written, read, other);
      if (errnoObject && isModSet(effects.getModRef(IRMemLocation::ErrnoMem)))
        mem.havoc(st, enc.value(errnoObject));
    }
    if (scanf)
      return scan(c, f, name, tag(), guard, st);
    if (!c.getType()->isVoidTy())
      enc.bind(&c, enc.fresh(c.getType(), tag()));
    return guard;
  }

  // The variadic arguments of scanf, fscanf and sscanf for a constant format
  // f (C17 7.21.6.2): each conversion writes the bytes its type takes, a
  // string up to its width or the end of the object, an unknown conversion
  // the whole object, and the result r says how many were assigned: the
  // first r, or none for EOF. A %n is written only when the conversions
  // before it were, if any.
  Term scan(CallBase &c, StringRef f, StringRef name, const std::string &tag,
                  Term guard, memory::State &st) {
    unsigned arg = c.getFunctionType()->getNumParams();
    Term r = enc.fresh(c.getType(), tag);
    enc.bind(&c, r);
    const DataLayout &dl = c.getDataLayout();
    unsigned bits = r.width(), ptr = dl.getPointerSize(), assigned = 0;
    for (const Conversion &cv : conversions(f)) {
      if (arg == c.arg_size())
        break;
      if (cv.suppressed)
        continue;
      Value *a = c.getArgOperand(arg++);
      Term done = cv.conv == 'n' ? t.sge(r, t.bv(assigned ? int64_t(assigned) : -1, bits))
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
      writes(c, t.land(guard, done), enc.value(a));
      memory::State written = st;
      mem.havoc(written, enc.value(a), n);
      st = mem.ite(done, written, st);
    }
    Term range = t.land(t.sge(r, t.bv(-1, bits)), t.sle(r, t.bv(assigned, bits)));
    return t.land(guard, range);
  }

  // Walks a defined callee in a new frame.
  Term enter(CallBase &c, Function &callee, Term guard, memory::State &st) {
    unsigned depth = std::count(stack.begin(), stack.end(), &callee);
    if (depth > 0) {
      bool cut = false;
      if (opts.unwind && depth >= opts.unwind) {
        unsigned &number = recursionNumber[&c];
        if (!number)
          number = ++recursionsIn[c.getFunction()];
        unwound(Unwound::Recursion, number, ir::location(c), guard, &c);
        cut = true;
      } else if (!opts.unwind && unreachable(guard)) {
        cut = true;
      }
      if (cut) { // no path continues; the result only has to be bound
        if (!c.getType()->isVoidTy())
          enc.bind(&c, enc.fresh(c.getType(), "unwound"));
        return t.boolean(false);
      }
    }

    SmallVector<Term, 4> actuals;
    for (Value *arg : c.args())
      actuals.push_back(enc.value(arg));
    // A recursive call rebinds the caller's own values; keep them.
    SmallVector<std::pair<const Value *, Term>, 0> saved;
    if (depth > 0) {
      for (Argument &arg : callee.args())
        if (Term term = enc.bound(&arg))
          saved.push_back({&arg, term});
      for (Instruction &inst : instructions(callee))
        if (Term term = enc.bound(&inst))
          saved.push_back({&inst, term});
    }
    for (Argument &arg : callee.args())
      enc.bind(&arg, actuals[arg.getArgNo()]);

    const CallBase *outerReachError = reachError;
    if (callee.getName() == "reach_error")
      reachError = &c;
    if (callee.isVarArg())
      varArgs.push_back(passVarArgs(c, callee, guard, st));
    encode::Encoder::Facts outer = enc.facts; // the callee's blocks set their own
    Frame frame = function(callee, guard, st);
    enc.facts = frame.returns.empty() ? std::move(outer) : common(frame.returns);
    if (callee.isVarArg())
      varArgs.pop_back();
    reachError = outerReachError;

    Term after = t.boolean(false), result;
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

  // The v*scanf and vswprintf, under glibc's names too.
  static bool writesThroughVaList(StringRef name) {
    name.consume_front("__isoc99_") || name.consume_front("__isoc23_") ||
        (name.consume_front("__") && name.consume_back("_chk"));
    return is_contained<StringRef>(
        {"vscanf", "vfscanf", "vsscanf", "vswprintf", "vwscanf", "vfwscanf", "vswscanf"}, name);
  }

  static bool writesOnlyLock(StringRef name) {
    return is_contained<StringRef>({"pthread_mutex_init", "pthread_mutex_lock",
                                    "pthread_mutex_trylock", "pthread_mutex_unlock",
                                    "pthread_mutex_destroy", "pthread_cond_init",
                                    "pthread_cond_wait", "pthread_cond_signal",
                                    "pthread_cond_broadcast", "pthread_cond_destroy"},
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
  VarArgs passVarArgs(CallBase &c, Function &callee, Term guard, memory::State &st) {
    bool x64 = isX86_64(c);
    if (c.getCallingConv() != CallingConv::C)
      throw Unsupported("variadic call with another calling convention at " +
                        ir::location(c).display());
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
                          " at " + ir::location(c).display());
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
    Term stackArea = mem.allocate(t.bv(onStack, bits), 16, guard);
    va.stack = mem.advance(stackArea, t.bv(named, bits));
    if (x64)
      va.regs = mem.allocate(t.bv(176, bits), 16, guard);
    for (unsigned i = callee.arg_size(); i < c.arg_size(); ++i) {
      Value *arg = c.getArgOperand(i);
      Term at =
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
      throw Unsupported("va_start outside a variadic call at " + ir::location(c).display());
    const VarArgs &va = varArgs.back();
    Term ap = enc.value(c.getArgOperand(0));
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

  // A check at inst, violated when violated holds; comment names it. An
  // instruction with several checks tells them apart by what.
  void assertion(const Instruction &inst, Term violated, std::string comment,
                 std::string expr, const void *what = nullptr,
                 checks::Property kind = checks::Property::Assertion) {
    unsigned &index = assertionNumber[{&inst, what}];
    if (!index)
      index = ++assertionsIn[{inst.getFunction(), unsigned(kind)}];
    claim(kind, ir::location(inst), std::move(comment), std::move(expr),
          index, violated, {&inst, what});
  }

  // A write at inst through p where guard holds, which fails where p points
  // into a string literal or an object defined const: one assertion for
  // the literals and one per such object, worded as ESBMC's pointer check.
  void writes(const Instruction &inst, Term guard, Term p) {
    if (!opts.pointerCheck)
      return;
    for (auto &[g, into] : mem.readOnly(p)) {
      Term violated = t.land(guard, into);
      if (t.isFalse(violated))
        continue;
      if (!counts(false)) {
        t.addConstraint(t.lnot(violated));
        continue;
      }
      // Clang makes a string literal, and __func__, an unnamed_addr global.
      if (g->hasGlobalUnnamedAddr()) {
        assertion(inst, violated, "dereference failure: write access to string constant", "",
                  &literals);
        continue;
      }
      SmallVector<DIGlobalVariableExpression *, 1> vars;
      g->getDebugInfo(vars);
      StringRef name = vars.empty() ? g->getName() : vars[0]->getVariable()->getName();
      assertion(inst, violated,
                "dereference failure: write access to const object `" + name.str() + "'", "", g);
    }
  }

  using ClaimKey = std::pair<const void *, const void *>;

  // One claim per source construct, keyed by its instruction or loop and
  // what it checks there: reaching it again, in another iteration or
  // context, ORs into the check.
  void claim(checks::Property property, const ir::SourceLoc &loc, std::string comment,
             std::string expr, unsigned index, Term violated, ClaimKey key) {
    auto [it, inserted] = claims.try_emplace(key, checks.size());
    bool viaReachError = reachError || comment == "reach_error";
    trace.add(Step::Check, violated, reachError ? ir::location(*reachError) : loc,
              viaReachError ? "reach_error" : "", {}, false, it->second);
    if (!inserted) {
      checks[it->second].violated = t.lor(checks[it->second].violated, violated);
      return;
    }
    checks.push_back({property, loc, std::move(comment), std::move(expr), index, violated});
  }

  // The one value v takes where guard holds (0 if it holds nowhere), which
  // the solver checks; folding terms as they are built settles most
  // without it. None when v may take several.
  std::optional<uint64_t> single(Term v, Term guard) {
    if (const APInt *c = t.constant(v))
      return c->getLimitedValue();
    camada::CheckResult feasible = s.checkSatAssuming({t.solverTerm(guard)});
    if (feasible == camada::CheckResult::UNSAT)
      return 0; // the path is infeasible
    if (feasible != camada::CheckResult::SAT)
      return std::nullopt;
    auto model = s.getBVUnsigned(t.solverTerm(v));
    if (!model)
      return std::nullopt;
    Term other = t.lnot(t.eq(v, t.bv(model.value(), v.width())));
    if (s.checkSatAssuming({t.solverTerm(guard), t.solverTerm(other)}) !=
        camada::CheckResult::UNSAT)
      return std::nullopt;
    return model.value();
  }

  // Size operand of a memory function. A symbolic size is accepted when it
  // has one value on the path; one with several needs a loop or a
  // quantifier. Memory limits how many bytes go one at a time.
  uint64_t constantSize(const Value *v, Term guard, StringRef what) {
    if (auto *ci = dyn_cast<ConstantInt>(v))
      return ci->getLimitedValue();
    std::optional<uint64_t> n = single(enc.value(v), guard);
    if (!n)
      throw Unsupported(what.str() + " with a symbolic size");
    return *n;
  }

  // alloca, load and store, and the atomics. Returns false for any other
  // instruction.
  bool memoryAccess(Instruction &inst, Term &guard, memory::State &st) {
    if (auto *a = dyn_cast<AllocaInst>(&inst)) {
      const DataLayout &dl = a->getDataLayout();
      unsigned bits = mem.pointerBits();
      uint64_t elemSize = dl.getTypeAllocSize(a->getAllocatedType());
      Term count = enc.value(a->getArraySize());
      Term ok = t.boolean(true);
      bool zeroSize = elemSize == 0;
      if (elemSize) {
        // Compare before truncating or multiplying, so allocation-size
        // arithmetic cannot wrap an oversized request into a small one.
        unsigned width = std::max(64u, std::max(bits, count.width()));
        APInt limit = APInt::getSignedMaxValue(bits).zextOrTrunc(width);
        limit = limit.udiv(APInt(width, elemSize));
        zeroSize = limit.isZero();
        ok = t.ule(enc.resize(count, width, false), t.bv(limit));
      }
      Term violated = t.land(guard, t.lnot(ok));
      if (opts.vlaSizeCheck && counts(false) && !t.isFalse(violated))
        assertion(inst, violated, "alloca: size exceeds PTRDIFF_MAX", "");
      // Do not assume ok globally: that would erase this violation and
      // checks reached earlier on the same path.
      guard = t.land(guard, ok);
      mem.setContext(guard);
      if (t.isFalse(guard)) {
        enc.bind(a, mem.null());
        return true;
      }
      Term size = zeroSize ? t.bv(0, bits)
                           : t.mul(enc.resize(count, bits, false), t.bv(elemSize, bits));
      Term p = mem.allocate(size, a->getAlign().value(), guard, exposed(a));
      if (unsigned bits = ownsArrays(a, dl))
        mem.own(st, p, bits);
      enc.bind(a, p);
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
      Term p = enc.value(w->getPointerOperand());
      writes(*w, guard, p);
      mem.store(st, p, enc.toBits(enc.value(v), v->getType()), v->getType());
      if (summary && w == summary->store)
        summary->stored = {guard, enc.toBits(enc.value(v), v->getType())};
      // A source store into a named variable kept in memory (a global, an
      // array, a local whose address is taken) is an assignment to show.
      Type *ty = v->getType();
      if (w->getDebugLoc() && ty->isIntegerTy() && !ty->isIntegerTy(1))
        if (auto *var = namedObject(w->getPointerOperand()))
          trace.add(Step::Assignment, guard, ir::location(*w), var->first, enc.value(v), false, 0,
                    var->second, mem.offset(p));
      return true;
    }
    if (auto *rmw = dyn_cast<AtomicRMWInst>(&inst)) {
      writes(inst, guard, enc.value(rmw->getPointerOperand()));
      atomicRMW(*rmw, st);
      return true;
    }
    if (auto *cx = dyn_cast<AtomicCmpXchgInst>(&inst)) {
      Type *ty = cx->getNewValOperand()->getType();
      if (!(ty->isIntegerTy() && !ty->isIntegerTy(1)) && !ty->isPointerTy())
        throw Unsupported("instruction " + encode::describe(&inst));
      Term p = enc.value(cx->getPointerOperand());
      Term old = mem.load(st, p, ty), expected = enc.value(cx->getCompareOperand());
      // Pointers compare as icmp eq compares them.
      Term ok = ty->isPointerTy() ? mem.equal(old, expected) : t.eq(old, expected);
      if (cx->isWeak()) // may fail even when equal
        ok = t.land(ok, enc.fresh(Type::getInt1Ty(inst.getContext()), "cmpxchg_weak"));
      writes(inst, t.land(guard, ok), p);
      mem.store(st, p, t.ite(ok, enc.value(cx->getNewValOperand()), old), ty);
      // {old, success}: a packed struct holds field 0 in the low bits.
      enc.bind(cx, t.concat(t.ite(ok, t.bv(1, 1), t.bv(0, 1)), old));
      return true;
    }
    return isa<FenceInst>(inst);
  }

  // The source variable whose memory p points into, when it is a global or
  // a local kept in memory with a name in the debug info: name and type.
  const std::pair<std::string, const DIType *> *namedObject(Value *p) {
    while (auto *g = dyn_cast<GEPOperator>(p))
      p = g->getPointerOperand();
    p = p->stripPointerCasts();
    auto [it, inserted] = namedObjects.try_emplace(p);
    if (inserted) {
      if (auto *gv = dyn_cast<GlobalVariable>(p)) {
        SmallVector<DIGlobalVariableExpression *, 1> vars;
        gv->getDebugInfo(vars);
        if (!vars.empty())
          it->second.emplace(vars[0]->getVariable()->getName().str(),
                             vars[0]->getVariable()->getType());
      } else if (auto *a = dyn_cast<AllocaInst>(p)) {
        for (DbgVariableRecord *d : findDVRDeclares(a))
          if (!d->getExpression()->getNumElements()) {
            it->second.emplace(d->getVariable()->getName().str(), d->getVariable()->getType());
            break;
          }
      }
    }
    return it->second ? &*it->second : nullptr;
  }

  // Loads the old value, stores the operation's result and binds the old
  // value, as one thread sees an atomic read-modify-write.
  void atomicRMW(AtomicRMWInst &rmw, memory::State &st) {
    Type *ty = rmw.getType();
    AtomicRMWInst::BinOp op = rmw.getOperation();
    if (!(ty->isIntegerTy() && !ty->isIntegerTy(1)) &&
        !(ty->isPointerTy() && op == AtomicRMWInst::Xchg))
      throw Unsupported("instruction " + encode::describe(&rmw));
    Term p = enc.value(rmw.getPointerOperand());
    Term old = mem.load(st, p, ty), v = enc.value(rmw.getValOperand());
    unsigned w = ty->isIntegerTy() ? ty->getIntegerBitWidth() : 0;
    auto one = [&] { return t.bv(1, w); };
    Term r;
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
  bool memoryFunction(CallBase &c, StringRef name, Term guard, memory::State &st) {
    bool copy = name.starts_with("llvm.memcpy") || name.starts_with("llvm.memmove") ||
                name == "memcpy" || name == "memmove";
    if (copy || name.starts_with("llvm.memset") || name == "memset") {
      Term dst = enc.value(c.getArgOperand(0)), v = enc.value(c.getArgOperand(1));
      uint64_t n = constantSize(c.getArgOperand(2), guard, name);
      if (n)
        writes(c, guard, dst);
      if (copy)
        mem.copy(st, dst, v, n);
      else
        mem.set(st, dst, enc.resize(v, 8, false), n);
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
  bool stringFunction(CallBase &c, bool find, Term guard, memory::State &st) {
    if (c.arg_size() != 3 || !c.getArgOperand(0)->getType()->isPointerTy() ||
        !c.getArgOperand(2)->getType()->isIntegerTy() ||
        !(find ? c.getArgOperand(1)->getType()->isIntegerTy() && c.getType()->isPointerTy()
               : c.getArgOperand(1)->getType()->isPointerTy() && c.getType()->isIntegerTy(32)))
      return false;
    Term a = enc.value(c.getArgOperand(0)), other = enc.value(c.getArgOperand(1));
    Term n = enc.value(c.getArgOperand(2));
    std::optional<uint64_t> bytes =
        find ? rangeBound(n, guard, {a}) : rangeBound(n, guard, {a, other});
    if (!bytes)
      return false;
    Type *i8 = Type::getInt8Ty(c.getContext());
    Term ch;
    if (find)
      ch = enc.resize(other, 8, false);
    Term r = find ? mem.null() : t.bv(0, 32);
    // From the last byte back, so that the first one that decides is outermost.
    for (uint64_t i = *bytes; i-- > 0;) {
      Term in = t.ult(t.bv(i, n.width()), n);
      Term at = mem.advance(a, t.bv(i, mem.pointerBits()));
      Term x = mem.load(st, at, i8);
      if (find) {
        r = t.ite(t.land(in, t.eq(x, ch)), at, r);
        continue;
      }
      Term y = mem.load(st, mem.advance(other, t.bv(i, mem.pointerBits())), i8);
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
  std::optional<uint64_t> rangeBound(Term n, Term guard,
                                     std::initializer_list<Term> ptrs) {
    if (const APInt *k = t.constant(n))
      return k->ule(memory::Memory::MaxBytes) ? std::optional(k->getZExtValue()) : std::nullopt;
    uint64_t bound = memory::Memory::MaxBytes;
    for (const Term &p : ptrs) {
      const APInt *size = t.constant(mem.size(mem.object(p))), *off = t.constant(mem.offset(p));
      if (!size || !off || off->ugt(*size))
        return std::nullopt;
      bound = std::min(bound, (*size - *off).getZExtValue());
    }
    Term over = t.ugt(n, t.bv(bound, n.width()));
    if (s.checkSatAssuming({t.solverTerm(guard), t.solverTerm(over)}) != camada::CheckResult::UNSAT)
      return std::nullopt;
    return bound;
  }

  // malloc and calloc return null when the object does not fit in the
  // address space, or calloc's size overflows (C23 7.24.3.2). Like ESBMC by
  // default, malloc may also fail anyway unless --force-malloc-success.
  void allocateHeap(CallBase &c, StringRef name, Term guard, memory::State &st) {
    unsigned bits = mem.pointerBits();
    bool isCalloc = name == "calloc";
    Term size = enc.resize(enc.value(c.getArgOperand(0)), bits, false);
    Term ok = t.boolean(true);
    if (isCalloc) {
      Term count = size;
      size = enc.resize(enc.value(c.getArgOperand(1)), bits, false);
      ok = t.lnot(t.umulo(count, size));
      size = t.mul(count, size);
    }
    ok = t.land(ok, mem.fits(size));
    if (!opts.forceMallocSuccess)
      ok = t.land(ok, enc.fresh(Type::getInt1Ty(c.getContext()), "malloc_succeeds"));
    Term p = mem.allocate(size, 16, t.land(guard, ok));
    if (isCalloc) {
      // A size that is not constant gets a constant array of zeros, whatever
      // it is on the path. A product beyond the largest object makes ok
      // false: nothing to zero.
      const APInt *n = t.constant(size);
      if (!n)
        mem.fill(st, p, t.bv(0, 8));
      else if (!t.isFalse(ok))
        mem.set(st, p, t.bv(0, 8), n->getZExtValue());
    }
    enc.bind(&c, t.ite(ok, p, mem.null()));
  }

  Options opts;
  encode::Encoder &enc;
  memory::Memory &mem;
  encode::Terms &t;
  camada::SMTSolver &s;
  checks::Checks &checks;
  Trace &trace;
  TargetLibraryInfoImpl tlii;
  bool incremental;
  struct Stopped {
    Frame frame;
    Loop *loop;
    unsigned iteration;
    size_t checks, trace;
  };
  std::optional<Stopped> stopped;
  bool threads = false;
  DenseMap<const Function *, std::unique_ptr<FunctionInfo>> infos;
  DenseMap<const Value *, unsigned> owns; // ownsArrays, per alloca and global
  DenseMap<const Value *, bool> exposes;  // exposed, per alloca, global and function
  DenseMap<const Value *, std::optional<std::pair<std::string, const DIType *>>> namedObjects;
  SmallVector<const Function *, 8> stack;
  SmallVector<VarArgs, 2> varArgs; // of the variadic frames on the stack, innermost last
  // The call to the reach_error being walked, if any: the witness puts a
  // violation inside it at the call, the violation unreach-call names.
  const CallBase *reachError = nullptr;
  DenseMap<ClaimKey, unsigned> claims; // source construct -> index in checks
  DenseMap<ClaimKey, unsigned> assertionNumber;
  const char literals = 0; // what the checks of writes into string literals check
  DenseMap<std::pair<const Function *, unsigned>, unsigned> assertionsIn;
  DenseMap<const CallBase *, unsigned> recursionNumber;
  DenseMap<const Function *, unsigned> recursionsIn;
  SmallPtrSet<const Function *, 8> warned;
  DenseSet<const void *> unsatGuards;
  DenseSet<const void *> satGuards;
  uint64_t satEpoch = 0;
  // The iteration of a summarised loop being walked: its index, its store,
  // and once walked, the store's guard and bits.
  struct Summarising {
    const Loop *loop;
    Term index;
    const StoreInst *store;
    std::pair<Term, Term> stored;
    unsigned nondets = 0; // nondet calls walked, which names their arrays
  };
  std::optional<Summarising> summary;
  // The terms of the values the loop being summarised takes from outside
  // it, by their SCEVs.
  DenseMap<const SCEV *, Term> inputs;
  unsigned summaryCount = 0; // names each summary's fresh symbols
  SmallPtrSet<const Loop *, 8> summarised;
  // What a call without a body may write besides its arguments' objects:
  // errno, and the writable globals the program declares without defining,
  // except the models' stdin, stdout and stderr objects, whose contents no
  // program reads and which a call that gets the stream writes anyway.
  GlobalVariable *errnoObject = nullptr;
  SmallVector<GlobalVariable *, 4> externs;
};

} // namespace

struct Incremental::Impl {
  Function &entry;
  Walk walk;
  Impl(Function &entry, const Options &opts, encode::Encoder &enc, memory::Memory &mem,
       checks::Checks &checks, Trace &trace)
      : entry(entry), walk(*entry.getParent(), opts, enc, mem, checks, trace, true) {}
};

Incremental::Incremental(Function &entry, const Options &opts, encode::Encoder &enc,
                         memory::Memory &mem, checks::Checks &checks, Trace &trace)
    : impl(std::make_unique<Impl>(entry, opts, enc, mem, checks, trace)) {}

Incremental::~Incremental() = default;

bool Incremental::resumable() const { return impl->walk.resumable(); }

Error Incremental::advance(unsigned bound) {
  try {
    impl->walk.advance(impl->entry, bound);
    impl->walk.report();
  } catch (const Unsupported &e) {
    return createStringError(inconvertibleErrorCode(), "unsupported: %s", e.what());
  }
  return Error::success();
}

Error walk(Function &entry, const Options &opts, encode::Encoder &enc, memory::Memory &mem,
           checks::Checks &checks, Trace &trace) {
  try {
    Walk w(*entry.getParent(), opts, enc, mem, checks, trace);
    w.entry(entry);
    w.report();
  } catch (const Unsupported &e) {
    return createStringError(inconvertibleErrorCode(), "unsupported: %s", e.what());
  }
  return Error::success();
}

} // namespace vereda::walker
