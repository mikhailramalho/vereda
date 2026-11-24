// Stage memory: (object, offset) pointers and byte-addressed memory (plan,
// section 4.4). A pointer is one bitvector: a 32-bit object id above a
// pointer-width offset. Memory is one byte array indexed by that pointer,
// plus a provenance array giving, per byte, the object of a stored pointer.
// Each object has a symbolic base address, above the previous object's end
// with a gap of at least one byte; addresses matter only for ptrtoint,
// inttoptr, ordered pointer comparison and comparison with a pointer outside
// the program. Layout constraints hold only on paths where the object
// exists.
//
// Object 0, at base 0, is null at offset 0 and the memory outside the
// program at any other (issue #137): its offset is the address. A pointer
// the program did not make (a call without a body returns one, main gets
// argv, a nondet or undef pointer) is an integer address converted to a
// pointer, and fromAddress gives an object only if the program exposed its
// address (stored it, passed it on, converted it to an integer, compared it:
// the walker decides); any other address is in object 0. So a store through
// such a pointer cannot reach an object the program kept to itself. Bytes
// nothing wrote (uninitialised memory, or havocked memory that held no
// pointer) hold integers, so a pointer read from them is converted the same
// way.
//
// A byte written at a constant pointer stays in the state's map of known
// bytes and reaches the arrays only when an access at a pointer that may
// alias it needs them (plan, section 4.2: a load from a known store). Most
// scalar globals and locals at a fixed address then never touch the arrays.
// At a join, an object whose two sides hold different values that multiply,
// divide or take a remainder goes to the arrays, so that the solver works on
// that arithmetic only where a read needs it.
//
// A fill that covers a whole object of at least MinSeparate bytes (memset,
// calloc, a zeroed global) gives the object arrays of its own, constant
// arrays of the fill byte, indexed like the shared ones (plan, section 4.4:
// one array per partition, here per object). A read or write at a pointer
// whose object is not constant chooses among them by object id.
//
// An object the walker picks with own() (an array indexed at a symbolic
// offset whose address does not escape, issue #71) has arrays of its own
// from the start, indexed by the offset alone: stores to other objects stay
// out of its chain, and the solver compares pointer-width indices, not the
// object id too. It starts from fixed nondet arrays of its own, which also
// stand for its bytes on a path that has not created it yet. One that the
// walker finds read and written only as whole iN at multiples of N/8 bytes
// (issue #107) has a data array of N-bit elements, indexed by offset / (N/8),
// and no provenance array, as no pointer is stored there: a load or store of
// iN at a symbolic offset is one select or store. Its known bytes move to the
// array an element at a time, and an access through an unknown pointer that
// may hit it reads or writes one byte of an element.
//
// A summarised loop (issue #36) leaves the object it wrote with arrays that
// are lambdas over the ones before (terms.h): a read of an element the loop
// wrote reduces to the value its iteration stored there.
#pragma once
#include "encode/terms.h"
#include "memory/cellmap.h"
#include <camada/camada.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/DenseSet.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Module.h>
#include <optional>
#include <functional>
#include <vector>

namespace vereda::memory {

struct State {
  encode::Term data; // pointer -> byte
  encode::Term prov; // pointer -> object id of a stored pointer's byte
  using Cell = memory::Cell;
  // Bytes at constant (object, offset), newer than data and prov. Copies of
  // the state share it (cellmap.h).
  CellMap known;
  // Objects with arrays of their own, at key (object id, 0): its data and
  // provenance arrays, indexed by the whole pointer or, for an object
  // given them by own(), by the offset. For an object of elements, its
  // element array and the constant provenance of an integer.
  CellMap separate;
};

class Memory {
public:
  Memory(encode::Terms &t, const llvm::DataLayout &dl);

  unsigned pointerBits() const { return ptrBits; }
  const llvm::DataLayout &layout() const { return dl; }
  camada::SMTSortRef pointerSort();
  encode::Term null();
  encode::Term object(encode::Term p);
  encode::Term offset(encode::Term p);
  encode::Term ptr(encode::Term obj, encode::Term off);
  // p moved by delta bytes, a pointer-width bitvector.
  encode::Term advance(encode::Term p, encode::Term delta);

  // Whether an allocator accepts an object of size bytes: at most
  // PTRDIFF_MAX, as glibc's malloc. It depends on the size alone, so the
  // solver cannot make a small allocation fail by placing earlier objects.
  encode::Term fits(encode::Term size);
  // A new object of size bytes (pointer-width term) that exists when exists
  // holds; its layout constraints bind only then. Returns its start. An
  // address converted to a pointer may land in it only if exposed.
  encode::Term allocate(encode::Term size, uint64_t align,
                              encode::Term exists, bool exposed = true);
  // Allocates every global of m, and every function whose address is taken,
  // exposed as the predicate says; the globals' initialisers go in with
  // initialise().
  void allocateGlobals(const llvm::Module &m,
                       const std::function<bool(const llvm::GlobalValue *)> &exposed);
  encode::Term global(const llvm::GlobalValue *g) const;
  // Writes every global's initialiser into st; valueOf encodes constants
  // that are pointers into other globals.
  void initialise(State &st, const llvm::Module &m,
                  const std::function<encode::Term(const llvm::Constant *)> &valueOf);

  // The size in bytes of the object with id obj, a pointer-width term.
  encode::Term size(encode::Term obj);
  encode::Term address(encode::Term p);   // ptrtoint
  // p == q: object and offset equal, or, when one is outside the program,
  // the addresses: it may hold the address of an object allocated since it
  // was converted.
  encode::Term equal(encode::Term p, encode::Term q);
  // inttoptr: the exposed object that holds a, else object 0 at offset a
  // (null, or the memory outside the program).
  encode::Term fromAddress(encode::Term a);

  // The guard of the code being walked: an access at ite(c, p, q) whose c
  // or not c it implies goes to one side only.
  void setContext(encode::Term guard) { context = std::move(guard); }

  State initial(); // nondet contents
  State ite(encode::Term c, const State &a, const State &b);
  // Loads and stores of integers (i1 as a byte) and pointers.
  encode::Term load(State &st, encode::Term p, llvm::Type *ty);
  void store(State &st, encode::Term p, encode::Term v, llvm::Type *ty);
  // Writes byte to the n bytes at p. A fill of a whole object of at least
  // MinSeparate bytes is one constant array; anything else goes byte by
  // byte, which past MaxBytes is unsupported.
  void set(State &st, encode::Term p, encode::Term byte, uint64_t n);
  // Writes byte to every byte of the object p starts, whatever its size,
  // as one constant array: calloc with a size that is not constant.
  void fill(State &st, encode::Term p, encode::Term byte);
  void copy(State &st, encode::Term dst, encode::Term src, uint64_t n);
  // Gives the object of p, a pointer to a new object, arrays of its own
  // indexed by offset, before anything is written to it; with bits 16, 32
  // or 64, arrays of elements of that many bits.
  void own(State &st, encode::Term p, unsigned bits = 8);
  // Iteration n, a pointer-width term: whether it stored, and the value.
  using Iteration = std::function<std::pair<encode::Term, encode::Term>(
      const encode::Term &n)>;
  // The effect of a loop whose iterations n < count stored a value of type
  // ty at first + n * stride, as iteration(n) says, where first's object is
  // a constant and the stride at least the value's size.
  void summarise(State &st, encode::Term first, uint64_t stride, llvm::Type *ty,
                 encode::Term count, Iteration iteration);
  // Gives the n bytes at p nondet contents (__VERIFIER_nondet_memory), or
  // without n the whole object at p, as a call to a function without a body
  // may write any of it; for p into one of several objects, each where p
  // points into it.
  void havoc(State &st, encode::Term p, std::optional<uint64_t> n = std::nullopt);
  // The constant globals p may point into, each with where it does: a write
  // there modifies a string literal or an object defined const, which C11
  // leaves undefined (6.4.5p7, 6.7.3p6). A pointer made from an integer is
  // not counted, as it points into an object only by the addresses the
  // solver picks.
  std::vector<std::pair<const llvm::GlobalVariable *, encode::Term>>
  readOnly(encode::Term p);
  // What a call without a body writes, given the pointers it may write
  // through and those it only reads through: the whole object at each of
  // written, as havoc, but a constant global or a function, which the call
  // may not write. With reach, as such a call may follow the pointers
  // it finds (#114), also each object but a constant that a pointer stored
  // where any of them points may point to, transitively, where that pointer
  // points into an object that reaches it; a pointer made from an integer
  // (an address, or bytes nothing wrote) is not followed. Each object
  // written may then hold a pointer to any exposed object it got a pointer
  // into and to any object reached.
  void havocCall(State &st, llvm::ArrayRef<encode::Term> written,
                 llvm::ArrayRef<encode::Term> read, bool reach);
  // A pointer into the memory outside the program, for havoc.
  encode::Term outside() { return advance(null(), t.bv(1, ptrBits)); }
  static constexpr uint64_t MaxBytes = 1 << 20;
  static constexpr uint64_t MinSeparate = 1024;

private:
  encode::Term bytePtr(encode::Term p, uint64_t i);
  encode::Term base(encode::Term obj);
  bool hasPadding(llvm::Type *ty) const;
  using Key = std::pair<uint64_t, uint64_t>;
  std::optional<Key> key(encode::Term p) const;
  encode::Term ptr(const Key &k);
  // Moves the known bytes an access at p may alias into the arrays: those
  // of the objects p may point into.
  void prepare(State &st, encode::Term p);
  // The objects p may point into: the constants its object chooses among,
  // and, where that object was read from memory, object 0 and every exposed
  // object. Without integers, none for an object fromAddress made.
  std::vector<uint64_t> candidates(encode::Term p, bool integers = true);
  // havoc, with prov the provenance of the bytes of a whole object written;
  // a constant object is left as it is unless constants.
  void havocWith(State &st, encode::Term p, std::optional<uint64_t> n,
                 encode::Term prov, bool constants);
  // Where obj, an object id or a pointer, is object o, but by an address
  // converted to a pointer.
  encode::Term into(encode::Term obj, uint64_t o);
  void havocObject(State &st, uint64_t o, encode::Term prov);
  // For each object of st that may hold a pointer, the objects other than
  // null it may point to: from its known bytes' provenance and the stores in
  // the arrays that hold its bytes.
  llvm::DenseMap<uint64_t, llvm::DenseSet<uint64_t>> pointers(const State &st);
  // Moves the known bytes of object, or all known bytes, into the arrays.
  void flush(State &st, std::optional<uint64_t> object = std::nullopt);
  State::Cell readByte(const State &st, encode::Term at);
  // The byte at at from object's own arrays own, and those arrays with c
  // stored at at.
  State::Cell readOwn(uint64_t object, const State::Cell &own, encode::Term at);
  State::Cell storeOwn(uint64_t object, const State::Cell &own, encode::Term at,
                       const State::Cell &c);
  // The object of p when its elements are of type ty and p is at a symbolic
  // offset, where an access is one select or store once prepare(st, p) ran.
  std::optional<uint64_t> element(encode::Term p, llvm::Type *ty);
  // The arrays' bytes at a pointer that is not a known byte.
  State::Cell readArrays(const State &st, encode::Term at);
  // Stores c at at in the arrays that hold object's bytes in st: its own or
  // the shared ones.
  void storeArrays(State &st, uint64_t object, encode::Term at, const State::Cell &c);
  CellMap mergeSeparate(encode::Term c, const State &a, const State &b);
  // Where object's own arrays index the pointer at: by offset for an object
  // given them by own(), divided by the element's bytes for one of
  // elements, by the whole pointer otherwise.
  encode::Term index(uint64_t object, encode::Term at);
  // A fill of the whole object at p, if p is one: true when done.
  bool fillObject(State &st, encode::Term p, encode::Term byte, uint64_t n);
  void fill(State &st, uint64_t object, encode::Term byte);
  void setBytes(State &st, encode::Term p, encode::Term byte, uint64_t n);
  // set for initialisers: no limit, as they were written byte by byte before.
  void fillOrSet(State &st, encode::Term p, encode::Term byte, uint64_t n) {
    if (!fillObject(st, p, byte, n))
      setBytes(st, p, byte, n);
  }
  void writeByte(State &st, encode::Term at, const State::Cell &c);
  void storeBytes(State &st, encode::Term p, encode::Term bits,
                  encode::Term prov);
  void storeConstant(State &st, encode::Term p, const llvm::Constant *c,
                     const std::function<encode::Term(const llvm::Constant *)> &valueOf);

  struct Object {
    encode::Term base, size, exists;
    bool exposed;
    // The constant global or the function the object is, if either, which
    // the program may not write.
    const llvm::GlobalValue *constant = nullptr;
  };

  encode::Terms &t;
  camada::SMTSolver &s;
  const llvm::DataLayout &dl;
  unsigned ptrBits;
  std::vector<Object> objects; // index = object id; 0 is null
  encode::Term bases;    // object id -> base address, as an uninterpreted array
  encode::Term end;      // first address after the last object
  llvm::DenseMap<const llvm::GlobalValue *, encode::Term> globals;
  llvm::DenseMap<uint64_t, State::Cell> owned; // object id -> its initial arrays
  // Object id -> log2 of its element's bytes, for an object of elements.
  llvm::DenseMap<uint64_t, unsigned> shifts;
  // A summarised loop's provenance array (kept alive) -> the one it reads
  // below, which holds every pointer it does.
  llvm::DenseMap<const void *, std::pair<encode::Term, encode::Term>> provBelow;
  // The objects of the pointers fromAddress made, kept alive.
  llvm::DenseMap<const void *, encode::Term> fromInteger;
  unsigned fresh = 0;
  encode::Term context;
  // Which side of a split pointer the context picks: 1, 0, or -1 for both.
  int side(const encode::Term &c);
};

} // namespace vereda::memory
