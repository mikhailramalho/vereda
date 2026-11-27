#include "analysis/writes.h"
#include "util/assert.h"
#include "util/unsupported.h"

#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IntrinsicInst.h>

using namespace llvm;

namespace vereda::analysis {

namespace {

// The bodyless functions the walker models as writing no memory the loop
// had before: checks, assumptions, nondet values, allocation, and the
// intrinsics it ignores.
bool writesNothing(StringRef name, unsigned args) {
  return (name.starts_with("__VERIFIER_nondet_") && name != "__VERIFIER_nondet_memory") ||
         name == "__VERIFIER_assume" || name == "__VERIFIER_assert" ||
         (name == "assert" && args == 1) || name == "reach_error" ||
         name == "__VERIFIER_error" || name == "__vereda_error_label" || isAssertFail(name, args) || name == "abort" ||
         name == "exit" || name == "_Exit" || name == "malloc" || name == "calloc" ||
         name == "free" || name.starts_with("__vereda_check_") ||
         name == "__vereda_uninitialised" || name == "__vereda_object_size" ||
         name == "__ubsan_handle_divrem_overflow_abort" ||
         name == "__ubsan_handle_invalid_builtin_abort" ||
         name == "__ubsan_handle_float_cast_overflow_abort" || name.starts_with("llvm.lifetime") ||
         name.starts_with("llvm.stacksave") || name.starts_with("llvm.stackrestore");
}

struct Writes {
  const Loop &l;
  SmallSetVector<const Value *, 4> objects;
  SmallPtrSet<const Function *, 8> walked;

  void instruction(const Instruction &inst) {
    SmallVector<const Value *, 2> targets;
    if (auto *s = dyn_cast<StoreInst>(&inst))
      targets.push_back(s->getPointerOperand());
    else if (auto *a = dyn_cast<AtomicRMWInst>(&inst))
      targets.push_back(a->getPointerOperand());
    else if (auto *x = dyn_cast<AtomicCmpXchgInst>(&inst))
      targets.push_back(x->getPointerOperand());
    else if (auto *c = dyn_cast<CallBase>(&inst))
      call(*c, targets);
    else if (inst.mayWriteToMemory())
      throw Unsupported("a loop transition over " + std::string(inst.getOpcodeName()));
    for (const Value *p : targets) {
      SmallVector<const Value *, 4> under;
      getUnderlyingObjects(p, under);
      for (const Value *o : under) {
        auto *a = dyn_cast<AllocaInst>(o);
        if (isa<GlobalVariable>(o) || (a && a->getFunction() == l.getHeader()->getParent() &&
                                       !l.contains(a)))
          objects.insert(o);
        else if (!a)
          throw Unsupported("a loop transition writing through a pointer it cannot place");
      }
    }
  }

  void call(const CallBase &c, SmallVectorImpl<const Value *> &targets) {
    auto *f = dyn_cast<Function>(c.getCalledOperand()->stripPointerCasts());
    if (!f)
      throw Unsupported("a loop transition over an indirect call");
    if (!f->isDeclaration()) {
      if (walked.insert(f).second)
        for (const Instruction &inst : instructions(*f))
          instruction(inst);
      return;
    }
    StringRef name = f->getName();
    if (c.onlyReadsMemory() || writesNothing(name, c.arg_size()))
      return;
    if (isa<AnyMemIntrinsic>(c) || name == "memset" || name == "memcpy" || name == "memmove" ||
        name == "__VERIFIER_nondet_memory") {
      targets.push_back(c.getArgOperand(0));
      return;
    }
    if (!f->isIntrinsic())
      throw Unsupported("a loop transition over a call to " + name.str());
    for (unsigned i = 0; i < c.arg_size(); ++i)
      if (c.getArgOperand(i)->getType()->isPointerTy() && !c.onlyReadsMemory(i))
        targets.push_back(c.getArgOperand(i));
  }
};

} // namespace

SmallSetVector<const Value *, 4> writtenObjects(const Loop &l) {
  Writes w{l, {}, {}};
  for (const BasicBlock *bb : l.blocks())
    for (const Instruction &inst : *bb)
      w.instruction(inst);
  return std::move(w.objects);
}

} // namespace vereda::analysis
