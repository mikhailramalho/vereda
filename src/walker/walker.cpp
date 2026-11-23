#include "walker/walker.h"

#include "ir/debug.h"
#include "ir/sanitizer.h"
#include "ir/source.h"
#include "util/log.h"
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/MapVector.h>
#include <llvm/ADT/PostOrderIterator.h>
#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/InstIterator.h>
#include <algorithm>
#include <memory>

using namespace llvm;
using camada::SMTExprRef;
using vereda::Unsupported;

namespace vereda::walker {

namespace {

// Per-function CFG facts, computed once.
struct FunctionInfo {
  explicit FunctionInfo(Function &f) : dt(f), li(dt) {
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
    mem.initialise(st, m, [this](const Constant *c) { return enc.value(c); });
    function(f, t.boolean(true), st);
  }

private:
  FunctionInfo &info(Function &f) {
    std::unique_ptr<FunctionInfo> &fi = infos[&f];
    if (!fi)
      fi = std::make_unique<FunctionInfo>(f);
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
        outs() << "Unwinding loop " << number << " iteration " << i << "   "
               << loopLocation(l).str() << "\n";
      }
      region(frame, fi, l);
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

  void block(Frame &frame, BasicBlock &bb) {
    SmallVector<Arrival, 2> arrivals = std::move(frame.arrivals[&bb]);
    frame.arrivals.erase(&bb);
    if (arrivals.empty()) // no edge into bb was taken in this pass
      return;
    SMTExprRef guard = anyGuard(arrivals);
    memory::State st = arrivals[0].mem;
    for (unsigned i = 1; i < arrivals.size(); ++i)
      st = mem.ite(arrivals[i].guard, arrivals[i].mem, st);

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
    if (!callee->isDeclaration())
      return enter(c, *callee, guard, st);
    // A bodyless setjmp would return a nondet value and longjmp would return
    // normally, which neither under- nor over-approximates the jump (#10).
    // Only a reached call refuses, so dead code does not cost the verdict.
    if (name == "setjmp" || name == "_setjmp" || name == "__sigsetjmp" || name == "sigsetjmp" ||
        name == "longjmp" || name == "_longjmp" || name == "siglongjmp" ||
        name == "__longjmp_chk")
      throw Unsupported("call to " + name.str() + " at " + ir::location(c).str());
    if (name == "malloc" || name == "calloc") {
      allocateHeap(c, name, guard, st);
      return guard;
    }
    if (memoryFunction(c, name, guard, st))
      return guard;

    // Bodyless verifier functions, as ESBMC treats them by default: a defined
    // one is an ordinary call, except reach_error under unreach-call, above.
    if (name.starts_with("__VERIFIER_nondet_")) {
      SMTExprRef v = enc.fresh(c.getType(), tag);
      enc.bind(&c, v);
      if (c.getType()->isIntegerTy()) // for the witness
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
    if (name == "abort" || name == "exit" || name == "_Exit")
      return t.boolean(false);
    // An unknown external returns an unconstrained value. Sound
    // while nothing has memory; revisit with the memory model.
    if (warned.insert(callee).second)
      util::warning("no body for function " + name);
    if (!c.getType()->isVoidTy())
      enc.bind(&c, enc.fresh(c.getType(), tag));
    return guard;
  }

  // Walks a defined callee in a new frame.
  SMTExprRef enter(CallBase &c, Function &callee, SMTExprRef guard, memory::State &st) {
    if (callee.isVarArg())
      throw Unsupported("variadic function " + callee.getName().str());
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
    Frame frame = function(callee, guard, st);
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

  // Size operand of a memory function. A symbolic size is accepted when it
  // has one value on the path, which the solver checks; folding terms as
  // they are built will settle most of these without it.
  // A size with several values needs a loop or a quantifier. Memory
  // limits how many bytes go one at a time.
  uint64_t constantSize(const Value *v, SMTExprRef guard, StringRef what) {
    if (auto *ci = dyn_cast<ConstantInt>(v))
      return ci->getLimitedValue();
    SMTExprRef size = enc.value(v);
    if (const APInt *c = t.constant(size))
      return c->getLimitedValue();
    if (s.checkSatAssuming({guard}) != camada::CheckResult::SAT)
      return 0; // the path is infeasible: no bytes to write
    auto model = s.getBVUnsigned(size);
    if (!model)
      throw Unsupported(what.str() + " with a symbolic size");
    uint64_t value = model.value();
    SMTExprRef other = t.lnot(t.eq(size, t.bv(value, size->getWidth())));
    if (s.checkSatAssuming({guard, other}) != camada::CheckResult::UNSAT)
      throw Unsupported(what.str() + " with a symbolic size");
    return value;
  }

  // alloca, load and store, and the atomics. Returns false for any other
  // instruction.
  bool memoryAccess(Instruction &inst, SMTExprRef guard, memory::State &st) {
    if (auto *a = dyn_cast<AllocaInst>(&inst)) {
      const DataLayout &dl = a->getDataLayout();
      unsigned bits = mem.pointerBits();
      SMTExprRef count = enc.resize(enc.value(a->getArraySize()), bits, false);
      SMTExprRef elem = t.bv(dl.getTypeAllocSize(a->getAllocatedType()), bits);
      // A stack object that does not fit is a stack overflow, which the
      // verifier assumes away: the path ends, like ESBMC.
      SMTExprRef ok = t.land(t.lnot(t.umulo(count, elem)),
                              mem.fits(t.mul(count, elem)));
      enc.bind(a, mem.allocate(t.mul(count, elem), a->getAlign().value(), guard));
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
      mem.store(st, p, enc.toBits(enc.value(v), v->getType()), v->getType());
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
      uint64_t n = constantSize(c.getArgOperand(0), guard, "calloc") *
                   constantSize(c.getArgOperand(1), guard, "calloc");
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
  SmallVector<const Function *, 8> stack;
  // The call to the reach_error being walked, if any: the witness puts a
  // violation inside it at the call, the violation unreach-call names.
  const CallBase *reachError = nullptr;
  DenseMap<const void *, unsigned> claims; // source construct -> index in checks
  DenseMap<const Instruction *, unsigned> assertionNumber;
  DenseMap<const Function *, unsigned> assertionsIn;
  DenseMap<const CallBase *, unsigned> recursionNumber;
  DenseMap<const Function *, unsigned> recursionsIn;
  SmallPtrSet<const Function *, 8> warned;
};

} // namespace

Error walk(Function &entry, const Options &opts, encode::Encoder &enc, memory::Memory &mem,
           checks::Checks &checks, Trace &trace) {
  try {
    Walk(opts, enc, mem, checks, trace).entry(entry);
  } catch (const Unsupported &e) {
    return createStringError(inconvertibleErrorCode(), "unsupported: %s", e.what());
  }
  return Error::success();
}

} // namespace vereda::walker
