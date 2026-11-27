// One loop iteration as a transition relation (walker.h, Transition): scalar
// phis, an array write next to an object the loop leaves alone, a write in
// a callee, a nested loop from either side, and a write the pre-state cannot
// havoc.
#undef NDEBUG
#include "encode/encoder.h"
#include "ir/load.h"
#include "solver/solver.h"
#include "walker/walker.h"
#include <cassert>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <string>

using namespace vereda;
using encode::Term;

namespace {

const char *Header = R"(
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"
declare i32 @__VERIFIER_nondet_int()
declare void @__VERIFIER_assert(i32)
)";

// The transitions of the loop headed by block loop in main.
struct Run {
  llvm::LLVMContext ctx;
  std::unique_ptr<llvm::Module> m;
  camada::SMTSolverRef s;
  std::unique_ptr<encode::Terms> t;
  std::unique_ptr<memory::Memory> mem;
  std::unique_ptr<encode::Encoder> enc;
  checks::Checks checks;
  walker::Trace trace;
  std::vector<walker::Transition> out;
  const llvm::BasicBlock *header = nullptr;
  std::string error;

  Run(const std::string &ir, llvm::StringRef loop, unsigned unwind = 0) {
    llvm::SmallString<64> path;
    int fd;
    assert(!llvm::sys::fs::createTemporaryFile("loop_transition", "ll", fd, path));
    {
      llvm::raw_fd_ostream os(fd, /*shouldClose=*/true);
      os << Header << ir;
    }
    auto loaded = ir::load(path, ctx);
    llvm::sys::fs::remove(path);
    assert(loaded);
    m = std::move(*loaded);
    auto created = solver::create(camada::SolverKind::Bitwuzla);
    assert(created);
    s = std::move(*created);
    t = std::make_unique<encode::Terms>(*s);
    mem = std::make_unique<memory::Memory>(*t, m->getDataLayout());
    enc = std::make_unique<encode::Encoder>(*t, *mem);
    llvm::Function &main = *m->getFunction("main");
    for (const llvm::BasicBlock &bb : main)
      if (bb.getName() == loop)
        header = &bb;
    assert(header);
    walker::Options opts;
    opts.unwind = unwind;
    opts.showIterations = false;
    if (llvm::Error e = walker::transitions(main, *header, opts, *enc, *mem, checks, trace, out))
      error = llvm::toString(std::move(e));
  }

  // The position of header's phi name.
  unsigned phi(llvm::StringRef name) const {
    unsigned i = 0;
    for (const llvm::PHINode &p : header->phis()) {
      if (p.getName() == name)
        return i;
      ++i;
    }
    assert(false && "no such phi");
    return 0;
  }
  bool sat(const Term &f) {
    return s->checkSatAssuming({t->solverTerm(f)}) == camada::CheckResult::SAT;
  }
  bool valid(const Term &f) { return !sat(t->lnot(f)); }
  // The i32 at element k (an i64 term) of the global array name.
  Term element(memory::State st, llvm::StringRef name, const Term &k) {
    Term p = mem->advance(mem->global(m->getNamedGlobal(name)), t->mul(k, t->bv(4, 64)));
    return mem->load(st, p, llvm::Type::getInt32Ty(ctx));
  }
};

void scalars() {
  Run r(R"(
define i32 @main() {
entry:
  %n = call i32 @__VERIFIER_nondet_int()
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i1, %body ]
  %s = phi i32 [ 0, %entry ], [ %s1, %body ]
  %c = icmp slt i32 %i, %n
  br i1 %c, label %body, label %exit
body:
  %ok = icmp ne i32 %i, 7
  %okz = zext i1 %ok to i32
  call void @__VERIFIER_assert(i32 %okz)
  %s1 = add i32 %s, 2
  %i1 = add i32 %i, 1
  br label %loop
exit:
  ret i32 %s
}
)",
        "loop");
  assert(r.error.empty() && r.out.size() == 1);
  walker::Transition &tr = r.out[0];
  encode::Terms &t = *r.t;
  unsigned i = r.phi("i"), s = r.phi("s");
  assert(tr.pre.size() == 2 && tr.post.size() == 2);
  assert(r.valid(t.implies(tr.next, t.eq(tr.post[i], t.add(tr.pre[i], t.bv(1, 32))))));
  assert(r.valid(t.implies(tr.next, t.eq(tr.post[s], t.add(tr.pre[s], t.bv(2, 32))))));
  // Any state: not only the first iteration's.
  assert(r.sat(t.land(tr.next, t.eq(tr.pre[i], t.bv(1000, 32)))));
  // The assertion fails in this iteration exactly where i is 7.
  assert(tr.checks.size() == 1);
  const Term &violated = tr.checks[0].second;
  assert(r.checks[tr.checks[0].first].property == checks::Property::Assertion);
  assert(r.sat(violated));
  assert(r.valid(t.implies(violated, t.eq(tr.pre[i], t.bv(7, 32)))));
}

void array() {
  Run r(R"(
@a = global [10 x i32] zeroinitializer
@g = global i32 5
define i32 @main() {
entry:
  br label %loop
loop:
  %i = phi i64 [ 0, %entry ], [ %i1, %body ]
  %c = icmp ult i64 %i, 10
  br i1 %c, label %body, label %exit
body:
  %p = getelementptr inbounds [10 x i32], ptr @a, i64 0, i64 %i
  %v = trunc i64 %i to i32
  store i32 %v, ptr %p
  %i1 = add i64 %i, 1
  br label %loop
exit:
  %x = load i32, ptr @g
  ret i32 %x
}
)",
        "loop");
  assert(r.error.empty() && r.out.size() == 1);
  walker::Transition &tr = r.out[0];
  encode::Terms &t = *r.t;
  Term i = tr.pre[0];
  // a is written, so any contents; g is not, so its own.
  assert(r.sat(t.lnot(t.eq(r.element(tr.preMem, "a", t.bv(3, 64)), t.bv(0, 32)))));
  Term g = r.mem->load(tr.preMem, r.mem->global(r.m->getNamedGlobal("g")),
                       llvm::Type::getInt32Ty(r.ctx));
  assert(r.valid(t.eq(g, t.bv(5, 32))));
  // a[i] = i, every other element as it was.
  assert(r.valid(t.implies(tr.next, t.eq(r.element(tr.postMem, "a", i), t.extract(i, 31, 0)))));
  Term j = t.symbol("j", r.s->mkBVSort(64));
  Term other = t.land(tr.next, t.land(t.ult(j, t.bv(10, 64)), t.lnot(t.eq(j, i))));
  assert(r.valid(t.implies(other, t.eq(r.element(tr.postMem, "a", j),
                                       r.element(tr.preMem, "a", j)))));
}

// A callee's store to a global is the loop's write too.
void call() {
  Run r(R"(
@count = global i32 0
define void @bump() {
  %v = load i32, ptr @count
  %w = add i32 %v, 1
  store i32 %w, ptr @count
  ret void
}
define i32 @main() {
entry:
  %n = call i32 @__VERIFIER_nondet_int()
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i1, %body ]
  %c = icmp slt i32 %i, %n
  br i1 %c, label %body, label %exit
body:
  call void @bump()
  %i1 = add i32 %i, 1
  br label %loop
exit:
  ret i32 0
}
)",
        "loop");
  assert(r.error.empty() && r.out.size() == 1);
  walker::Transition &tr = r.out[0];
  encode::Terms &t = *r.t;
  llvm::Type *i32 = llvm::Type::getInt32Ty(r.ctx);
  Term at = r.mem->global(r.m->getNamedGlobal("count"));
  Term before = r.mem->load(tr.preMem, at, i32), after = r.mem->load(tr.postMem, at, i32);
  assert(r.sat(t.land(tr.next, t.eq(before, t.bv(1000, 32)))));
  assert(r.valid(t.implies(tr.next, t.eq(after, t.add(before, t.bv(1, 32))))));
}

const char *Nested = R"(
define i32 @main() {
entry:
  %n = call i32 @__VERIFIER_nondet_int()
  br label %outer
outer:
  %i = phi i32 [ 0, %entry ], [ %i1, %latch ]
  %s = phi i32 [ 0, %entry ], [ %t.lcssa, %latch ]
  %c = icmp slt i32 %i, %n
  br i1 %c, label %inner.pre, label %exit
inner.pre:
  br label %inner
inner:
  %j = phi i32 [ 0, %inner.pre ], [ %j1, %inner.body ]
  %t = phi i32 [ %s, %inner.pre ], [ %t1, %inner.body ]
  %d = icmp ult i32 %j, 3
  br i1 %d, label %inner.body, label %latch
inner.body:
  %t1 = add i32 %t, 1
  %j1 = add i32 %j, 1
  br label %inner
latch:
  %t.lcssa = phi i32 [ %t, %inner ]
  %i1 = add i32 %i, 1
  br label %outer
exit:
  ret i32 %s
}
)";

void nested() {
  // The outer loop's iteration unrolls the inner loop, three times.
  {
    Run r(Nested, "outer");
    assert(r.error.empty() && r.out.size() == 1);
    walker::Transition &tr = r.out[0];
    encode::Terms &t = *r.t;
    unsigned s = r.phi("s");
    assert(r.sat(tr.next));
    assert(r.valid(t.implies(tr.next, t.eq(tr.post[s], t.add(tr.pre[s], t.bv(3, 32))))));
    assert(tr.checks.empty());
  }
  // The inner loop, reached in each of two outer iterations.
  {
    Run r(Nested, "inner", 2);
    assert(r.error.empty() && r.out.size() == 2);
    encode::Terms &t = *r.t;
    unsigned j = r.phi("j"), k = r.phi("t");
    for (walker::Transition &tr : r.out) {
      assert(r.sat(tr.next));
      assert(r.valid(t.implies(tr.next, t.eq(tr.post[j], t.add(tr.pre[j], t.bv(1, 32))))));
      assert(r.valid(t.implies(tr.next, t.eq(tr.post[k], t.add(tr.pre[k], t.bv(1, 32))))));
      assert(tr.checks.empty());
    }
    assert(!(r.out[0].pre[j] == r.out[1].pre[j]));
  }
}

// Loops whose pre-state cannot be built, each with the reason given.
void unsupported() {
  auto loop = [](const char *globals, const char *body) {
    return std::string(globals) + R"(
define i32 @main() {
entry:
  %q = load ptr, ptr @pp
  br label %loop
loop:
  %i = phi i64 [ 0, %entry ], [ %i1, %body ]
  %c = icmp ult i64 %i, 4
  br i1 %c, label %body, label %exit
body:
)" + body + R"(
  %i1 = add i64 %i, 1
  br label %loop
exit:
  ret i32 0
}
)";
  };
  const char *globals = "@pp = global ptr null\n@x = global i32 0\n@ps = global [4 x ptr] "
                        "zeroinitializer\ndeclare void @unknown(i64)\n";
  // A pointer stored would come back from the havocked pre-state pointing
  // only to exposed objects.
  Run stored(loop(globals, "  %p = getelementptr inbounds [4 x ptr], ptr @ps, i64 0, i64 %i\n"
                           "  store ptr @x, ptr %p"),
             "loop");
  assert(stored.error.find("may hold pointers") != std::string::npos);
  // A pointer loaded from memory points to an object the analysis cannot name.
  Run loaded(loop(globals, "  %p = getelementptr inbounds i32, ptr %q, i64 %i\n"
                           "  store i32 0, ptr %p"),
             "loop");
  assert(loaded.error.find("cannot place") != std::string::npos);
  Run unknown(loop(globals, "  call void @unknown(i64 %i)"), "loop");
  assert(unknown.error.find("call to unknown") != std::string::npos);
  Run pointer(R"(
@a = global [4 x i32] zeroinitializer
define i32 @main() {
entry:
  br label %loop
loop:
  %p = phi ptr [ @a, %entry ], [ %p1, %loop ]
  %p1 = getelementptr inbounds i32, ptr %p, i64 1
  %c = icmp ne ptr %p1, getelementptr inbounds (i32, ptr @a, i64 4)
  br i1 %c, label %loop, label %exit
exit:
  ret i32 0
}
)",
              "loop");
  assert(pointer.error.find("pointer phi") != std::string::npos);
}

} // namespace

int main() {
  scalars();
  array();
  call();
  nested();
  unsupported();
}
