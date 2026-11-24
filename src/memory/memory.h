// Stage memory: (object, offset) pointers and byte-addressed memory (plan,
// section 4.4). A pointer is one bitvector: a 32-bit object id above a
// pointer-width offset. Memory is one byte array indexed by that pointer,
// plus a provenance array giving, per byte, the object of a stored pointer.
// Object 0 is null. Each object has a symbolic base address, above the
// previous object's end with a gap of at least one byte; addresses matter
// only for ptrtoint, inttoptr and ordered pointer comparison. Layout
// constraints hold only on paths where the object exists.
//
// Object 1 is the memory outside the program (issue #137): at base 0, as
// large as the address space and outside the layout, so its offset is the
// address. A pointer the program did not make (a call without a body
// returns one, main gets argv, a nondet or undef pointer) is an integer
// address converted to a pointer, and fromAddress gives an object only if
// the program exposed its address (stored it, passed it on, converted it to
// an integer: the walker decides); any other address is in object 1. So a
// store through such a pointer cannot reach an object the program kept to
// itself. Bytes nothing wrote (uninitialised or havocked memory) hold
// integers, so a pointer read from them is converted the same way.
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
#pragma once
#include "encode/terms.h"
#include "memory/cellmap.h"
#include <camada/camada.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Module.h>
#include <optional>
#include <functional>
#include <vector>

namespace vereda::memory {

struct State {
  camada::SMTExprRef data; // pointer -> byte
  camada::SMTExprRef prov; // pointer -> object id of a stored pointer's byte
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
  camada::SMTExprRef null();
  camada::SMTExprRef object(camada::SMTExprRef p);
  camada::SMTExprRef offset(camada::SMTExprRef p);
  camada::SMTExprRef ptr(camada::SMTExprRef obj, camada::SMTExprRef off);
  // p moved by delta bytes, a pointer-width bitvector.
  camada::SMTExprRef advance(camada::SMTExprRef p, camada::SMTExprRef delta);

  // Whether an allocator accepts an object of size bytes: at most
  // PTRDIFF_MAX, as glibc's malloc. It depends on the size alone, so the
  // solver cannot make a small allocation fail by placing earlier objects.
  camada::SMTExprRef fits(camada::SMTExprRef size);
  // A new object of size bytes (pointer-width term) that exists when exists
  // holds; its layout constraints bind only then. Returns its start. An
  // address converted to a pointer may land in it only if exposed.
  camada::SMTExprRef allocate(camada::SMTExprRef size, uint64_t align,
                              camada::SMTExprRef exists, bool exposed = true);
  // Allocates every global of m, and every function whose address is taken,
  // exposed as the predicate says; the globals' initialisers go in with
  // initialise().
  void allocateGlobals(const llvm::Module &m,
                       const std::function<bool(const llvm::GlobalValue *)> &exposed);
  camada::SMTExprRef global(const llvm::GlobalValue *g) const;
  // Writes every global's initialiser into st; valueOf encodes constants
  // that are pointers into other globals.
  void initialise(State &st, const llvm::Module &m,
                  const std::function<camada::SMTExprRef(const llvm::Constant *)> &valueOf);

  // The size in bytes of the object with id obj, a pointer-width term.
  camada::SMTExprRef size(camada::SMTExprRef obj);
  camada::SMTExprRef address(camada::SMTExprRef p);   // ptrtoint
  // inttoptr: the exposed object that holds a, else null for 0 and the
  // memory outside the program for any other address.
  camada::SMTExprRef fromAddress(camada::SMTExprRef a);

  // The guard of the code being walked: an access at ite(c, p, q) whose c
  // or not c it implies goes to one side only.
  void setContext(camada::SMTExprRef guard) { context = std::move(guard); }

  State initial(); // nondet contents
  State ite(camada::SMTExprRef c, const State &a, const State &b);
  // Loads and stores of integers (i1 as a byte) and pointers.
  camada::SMTExprRef load(State &st, camada::SMTExprRef p, llvm::Type *ty);
  void store(State &st, camada::SMTExprRef p, camada::SMTExprRef v, llvm::Type *ty);
  // Writes byte to the n bytes at p. A fill of a whole object of at least
  // MinSeparate bytes is one constant array; anything else goes byte by
  // byte, which past MaxBytes is unsupported.
  void set(State &st, camada::SMTExprRef p, camada::SMTExprRef byte, uint64_t n);
  void copy(State &st, camada::SMTExprRef dst, camada::SMTExprRef src, uint64_t n);
  // Gives the object of p, a pointer to a new object, arrays of its own
  // indexed by offset, before anything is written to it; with bits 16, 32
  // or 64, arrays of elements of that many bits.
  void own(State &st, camada::SMTExprRef p, unsigned bits = 8);
  // Gives the n bytes at p nondet contents (__VERIFIER_nondet_memory), or
  // without n the whole object at p, as a call to a function without a body
  // may write any of it; for p into one of several objects, each where p
  // points into it.
  void havoc(State &st, camada::SMTExprRef p, std::optional<uint64_t> n = std::nullopt);
  static constexpr uint64_t MaxBytes = 1 << 20;
  static constexpr uint64_t MinSeparate = 1024;

private:
  camada::SMTExprRef bytePtr(camada::SMTExprRef p, uint64_t i);
  camada::SMTExprRef base(camada::SMTExprRef obj);
  bool hasPadding(llvm::Type *ty) const;
  using Key = std::pair<uint64_t, uint64_t>;
  std::optional<Key> key(camada::SMTExprRef p) const;
  camada::SMTExprRef ptr(const Key &k);
  // Moves the known bytes an access at p may alias into the arrays: those
  // of the objects p may point into.
  void prepare(State &st, camada::SMTExprRef p);
  // The objects p may point into: the constants its object chooses among,
  // and, where that object was read from memory, null, the memory outside
  // the program and every exposed object.
  std::vector<uint64_t> candidates(camada::SMTExprRef p);
  // Moves the known bytes of object, or all known bytes, into the arrays.
  void flush(State &st, std::optional<uint64_t> object = std::nullopt);
  State::Cell readByte(const State &st, camada::SMTExprRef at);
  // The byte at at from object's own arrays own, and those arrays with c
  // stored at at.
  State::Cell readOwn(uint64_t object, const State::Cell &own, camada::SMTExprRef at);
  State::Cell storeOwn(uint64_t object, const State::Cell &own, camada::SMTExprRef at,
                       const State::Cell &c);
  // The object of p when its elements are of type ty and p is at a symbolic
  // offset, where an access is one select or store once prepare(st, p) ran.
  std::optional<uint64_t> element(camada::SMTExprRef p, llvm::Type *ty);
  // The arrays' bytes at a pointer that is not a known byte.
  State::Cell readArrays(const State &st, camada::SMTExprRef at);
  // Stores c at at in the arrays that hold object's bytes in st: its own or
  // the shared ones.
  void storeArrays(State &st, uint64_t object, camada::SMTExprRef at, const State::Cell &c);
  CellMap mergeSeparate(camada::SMTExprRef c, const State &a, const State &b);
  // Where object's own arrays index the pointer at: by offset for an object
  // given them by own(), divided by the element's bytes for one of
  // elements, by the whole pointer otherwise.
  camada::SMTExprRef index(uint64_t object, camada::SMTExprRef at);
  // A fill of the whole object at p, if p is one: true when done.
  bool fillObject(State &st, camada::SMTExprRef p, camada::SMTExprRef byte, uint64_t n);
  void setBytes(State &st, camada::SMTExprRef p, camada::SMTExprRef byte, uint64_t n);
  // set for initialisers: no limit, as they were written byte by byte before.
  void fillOrSet(State &st, camada::SMTExprRef p, camada::SMTExprRef byte, uint64_t n) {
    if (!fillObject(st, p, byte, n))
      setBytes(st, p, byte, n);
  }
  void writeByte(State &st, camada::SMTExprRef at, const State::Cell &c);
  void storeBytes(State &st, camada::SMTExprRef p, camada::SMTExprRef bits,
                  camada::SMTExprRef prov);
  void storeConstant(State &st, camada::SMTExprRef p, const llvm::Constant *c,
                     const std::function<camada::SMTExprRef(const llvm::Constant *)> &valueOf);

  struct Object {
    camada::SMTExprRef base, size;
    bool exposed;
  };

  encode::Terms &t;
  camada::SMTSolver &s;
  const llvm::DataLayout &dl;
  unsigned ptrBits;
  std::vector<Object> objects; // index = object id; 0 is null
  camada::SMTExprRef bases;    // object id -> base address, as an uninterpreted array
  camada::SMTExprRef end;      // first address after the last object
  llvm::DenseMap<const llvm::GlobalValue *, camada::SMTExprRef> globals;
  llvm::DenseMap<uint64_t, State::Cell> owned; // object id -> its initial arrays
  // Object id -> log2 of its element's bytes, for an object of elements.
  llvm::DenseMap<uint64_t, unsigned> shifts;
  unsigned fresh = 0;
  camada::SMTExprRef context;
  // Which side of a split pointer the context picks: 1, 0, or -1 for both.
  int side(const camada::SMTExprRef &c);
};

} // namespace vereda::memory
