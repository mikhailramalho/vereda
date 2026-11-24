#include "memory/memory.h"

#include "util/unsupported.h"
#include <llvm/ADT/DenseSet.h>
#include <llvm/ADT/MapVector.h>
#include <llvm/ADT/SetVector.h>
#include <llvm/IR/GlobalVariable.h>
#include <set>
#include <vector>

using namespace llvm;
using camada::SMTExprRef;
using camada::SMTSortRef;
using vereda::Unsupported;

namespace vereda::memory {

static const unsigned ObjBits = 32;
static const int64_t IntProvenance = -1; // a byte that holds no pointer

static unsigned storeBits(const DataLayout &dl, Type *ty) {
  return 8 * dl.getTypeStoreSize(ty).getFixedValue();
}

Memory::Memory(encode::Terms &t, const DataLayout &dl)
    : t(t), s(t.solver()), dl(dl), ptrBits(dl.getPointerSizeInBits(0)) {
  if (dl.isBigEndian())
    throw Unsupported("big-endian targets");
  bases = t.symbol("bases", s.mkArraySort(s.mkBVSort(ObjBits), s.mkBVSort(ptrBits)));
  // Constrained before the object is recorded, as allocate() does: after it,
  // base() of the constant 0 folds to objects[0].base and the constraint to
  // true, which left bases[0] free and a stored null read back non-null.
  s.addConstraint(t.eq(base(t.bv(0, ObjBits)), t.bv(0, ptrBits)));
  objects.push_back({t.bv(0, ptrBits), t.bv(0, ptrBits), t.boolean(true), false});
  end = t.bv(1, ptrBits); // no object at address 0
}

SMTSortRef Memory::pointerSort() { return s.mkBVSort(ObjBits + ptrBits); }
SMTExprRef Memory::ptr(SMTExprRef obj, SMTExprRef off) { return t.concat(obj, off); }
SMTExprRef Memory::null() { return t.bv(0, ObjBits + ptrBits); }
SMTExprRef Memory::object(SMTExprRef p) {
  return t.extract(p, ObjBits + ptrBits - 1, ptrBits);
}
SMTExprRef Memory::offset(SMTExprRef p) { return t.extract(p, ptrBits - 1, 0); }
SMTExprRef Memory::advance(SMTExprRef p, SMTExprRef delta) {
  return ptr(object(p), t.add(offset(p), delta));
}
SMTExprRef Memory::bytePtr(SMTExprRef p, uint64_t i) {
  return i ? advance(p, t.bv(i, ptrBits)) : p;
}
SMTExprRef Memory::base(SMTExprRef obj) {
  SMTExprRef c, x, y;
  if (t.split(obj, c, x, y))
    return t.ite(c, base(x), base(y));
  if (const APInt *id = t.constant(obj); id && id->ult(objects.size()))
    return objects[id->getZExtValue()].base;
  return t.select(bases, obj);
}

SMTExprRef Memory::size(SMTExprRef obj) {
  SMTExprRef c, x, y;
  if (t.split(obj, c, x, y))
    return t.ite(c, size(x), size(y));
  if (const APInt *id = t.constant(obj); id && id->ult(objects.size()))
    return objects[id->getZExtValue()].size;
  SMTExprRef r = t.bv(0, ptrBits);
  for (unsigned id = 1; id < objects.size(); ++id)
    r = t.ite(t.eq(obj, t.bv(id, ObjBits)), objects[id].size, r);
  return r;
}

SMTExprRef Memory::fits(SMTExprRef size) {
  // Several objects near PTRDIFF_MAX on one path cannot all be
  // laid out, which ends that path; a real allocator would return null.
  return t.ule(size, t.lshr(t.bvnot(t.bv(0, ptrBits)),
                                    t.bv(1, ptrBits)));
}

SMTExprRef Memory::allocate(SMTExprRef size, uint64_t align, SMTExprRef exists, bool exposed) {
  unsigned id = objects.size();
  SMTExprRef b = t.symbol("base#" + std::to_string(id), s.mkBVSort(ptrBits));
  SMTExprRef e = t.add(b, size);
  // Ordered with a gap, aligned, not wrapping: objects never overlap, and a
  // one-past-the-end address belongs to its own object only. These bind
  // only where the object exists, or a size on one path would remove
  // inputs from every path.
  SMTExprRef layout = t.land(t.ugt(b, end), t.lnot(t.uaddo(b, size)));
  if (align > 1)
    layout = t.land(layout, t.eq(t.bvand(b, t.bv(align - 1, ptrBits)),
                                       t.bv(0, ptrBits)));
  s.addConstraint(t.implies(exists, layout));
  SMTExprRef obj = t.bv(id, ObjBits);
  s.addConstraint(t.eq(base(obj), b));
  objects.push_back({b, size, exists, exposed});
  end = t.ite(exists, e, end); // an object not allocated takes no space
  return ptr(obj, t.bv(0, ptrBits));
}

void Memory::allocateGlobals(const Module &m,
                             const std::function<bool(const GlobalValue *)> &exposed) {
  for (const GlobalVariable &g : m.globals()) {
    uint64_t size = dl.getTypeAllocSize(g.getValueType());
    globals[&g] = allocate(t.bv(size, ptrBits), g.getPointerAlignment(dl).value(),
                           t.boolean(true), exposed(&g));
    if (g.isConstant())
      objects.back().constant = &g;
  }
  // A function whose address is taken is an object of one byte, so that a
  // pointer to it is an ordinary pointer: stored, compared and called.
  for (const Function &f : m)
    if (f.hasAddressTaken(nullptr, false, true, false, false, /*IgnoreCastedDirectCall=*/true)) {
      globals[&f] = allocate(t.bv(1, ptrBits), 1, t.boolean(true), exposed(&f));
      objects.back().constant = &f;
    }
}

SMTExprRef Memory::global(const GlobalValue *g) const {
  auto it = globals.find(g);
  if (it == globals.end())
    throw Unsupported("address of " + g->getName().str());
  return it->second;
}

SMTExprRef Memory::address(SMTExprRef p) { return t.add(base(object(p)), offset(p)); }

SMTExprRef Memory::equal(SMTExprRef p, SMTExprRef q) {
  // Null is equal by both measures, so only a nonzero offset in object 0
  // needs the addresses; a test against null stays a test on the bits.
  auto out = [&](SMTExprRef x) {
    return t.land(t.eq(object(x), t.bv(0, ObjBits)), t.lnot(t.eq(offset(x), t.bv(0, ptrBits))));
  };
  SMTExprRef outside = t.lor(out(p), out(q));
  return t.lor(t.land(outside, t.eq(address(p), address(q))),
               t.land(t.lnot(outside), t.eq(p, q)));
}

SMTExprRef Memory::fromAddress(SMTExprRef a) {
  // One comparison per exposed object created so far. The object and the
  // offset are built apart, so that object() of the result is a choice
  // among constants.
  SMTExprRef obj = t.bv(0, ObjBits), off = a; // null, or outside the program
  for (unsigned id = 1; id < objects.size(); ++id) {
    const Object &o = objects[id];
    if (!o.exposed)
      continue;
    // One past the end still belongs to the object: the gap keeps it unique.
    // Only an object the path allocated holds an address: elsewhere its base
    // is free, and address 0 could land in it as a non-null pointer.
    SMTExprRef inside = t.land(o.exists, t.land(t.uge(a, o.base), t.ule(a, t.add(o.base, o.size))));
    obj = t.ite(inside, t.bv(id, ObjBits), obj);
    off = t.ite(inside, t.sub(a, o.base), off);
  }
  // What object() of the result folds to, which concat may have joined
  // with the offset's choices.
  SMTExprRef r = ptr(obj, off);
  if (SMTExprRef o = object(r); !t.constant(o))
    fromInteger[o.get()] = o;
  return r;
}

State Memory::initial() {
  SMTSortRef index = pointerSort();
  std::string n = std::to_string(fresh++);
  return {t.symbol("memory#" + n, s.mkArraySort(index, s.mkBVSort(8))),
          t.constArray(index, t.bv(IntProvenance, ObjBits))};
}

// Past this many known bytes in a state, all go to the arrays. 1 MiB keeps
// known the bytes a loop writes at constant indices into a large array, so
// that a read at a constant index finds its byte instead of walking a chain
// of stores.
static const size_t MaxKnown = 1 << 20;
// Past this many bytes of an object that differ at a join, it goes to the
// arrays on both sides.
static const size_t MaxChoices = 1024;

std::optional<Memory::Key> Memory::key(SMTExprRef p) const {
  const APInt *v = t.constant(p);
  if (!v)
    return std::nullopt;
  return Key{v->extractBitsAsZExtValue(ObjBits, ptrBits), v->extractBitsAsZExtValue(ptrBits, 0)};
}

SMTExprRef Memory::ptr(const Key &k) {
  // One constant, not two and a concat: a flush builds one per byte.
  return t.bv((APInt(ObjBits + ptrBits, k.first) << ptrBits) | APInt(ObjBits + ptrBits, k.second));
}

void Memory::flush(State &st, std::optional<uint64_t> object) {
  std::optional<Key> last; // the element last moved
  auto move = [&](Key k, const State::Cell &c) {
    unsigned shift = shifts.lookup(k.first);
    if (!shift)
      return storeArrays(st, k.first, ptr(k), c);
    // An element moves whole, as one store; its bytes not known are the
    // array's. Keys come in order, so its other bytes follow this one.
    // The array is read only for those: an element written whole needs no
    // read.
    Key e{k.first, k.second >> shift << shift};
    if (last == e)
      return;
    last = e;
    const State::Cell &own = *st.separate.find({e.first, 0});
    SMTExprRef i = index(e.first, ptr(e)), old, elem;
    for (unsigned j = 0; j < 1u << shift; ++j) {
      const State::Cell *b = st.known.find({e.first, e.second + j});
      if (!b && !old)
        old = t.select(own.data, i);
      SMTExprRef byte = b ? b->data : t.extract(old, 8 * j + 7, 8 * j);
      elem = j ? t.concat(byte, elem) : byte;
    }
    st.separate.set({e.first, 0}, {t.store(own.data, i, elem), own.prov});
  };
  if (object) {
    st.known.forObject(*object, move);
    st.known.eraseObject(*object);
  } else {
    st.known.forEach(move);
    st.known.clear();
  }
}

SMTExprRef Memory::index(uint64_t object, SMTExprRef at) {
  return owned.count(object) ? t.extract(offset(at), ptrBits - 1, shifts.lookup(object)) : at;
}

void Memory::own(State &st, SMTExprRef p, unsigned bits) {
  std::optional<Key> k = key(p);
  if (!k || k->first == 0)
    return;
  uint64_t o = k->first;
  unsigned shift = Log2_32(bits / 8);
  if (shift)
    shifts[o] = shift;
  SMTSortRef offsets = s.mkBVSort(ptrBits - shift);
  std::string n = std::to_string(o) + "#" + std::to_string(fresh);
  SMTExprRef prov = t.bv(IntProvenance, ObjBits);
  State::Cell initial{t.symbol("object#" + n, s.mkArraySort(offsets, s.mkBVSort(bits))),
                      shift ? prov : t.constArray(offsets, prov)};
  owned[o] = initial;
  st.separate.set({o, 0}, initial);
}

// The bits of byte at within its element: (offset mod bytes) * 8, n bits wide.
static SMTExprRef byteShift(encode::Terms &t, SMTExprRef off, unsigned shift, unsigned n) {
  return t.shl(t.zext(t.extract(off, shift - 1, 0), n - shift), t.bv(3, n));
}

State::Cell Memory::readOwn(uint64_t object, const State::Cell &own, SMTExprRef at) {
  SMTExprRef i = index(object, at);
  unsigned shift = shifts.lookup(object);
  if (!shift)
    return {t.select(own.data, i), t.select(own.prov, i)};
  SMTExprRef elem = t.select(own.data, i);
  if (const APInt *j = t.constant(t.extract(offset(at), shift - 1, 0))) {
    unsigned lo = 8 * j->getZExtValue();
    return {t.extract(elem, lo + 7, lo), own.prov};
  }
  unsigned n = elem->getWidth();
  return {t.extract(t.lshr(elem, byteShift(t, offset(at), shift, n)), 7, 0), own.prov};
}

State::Cell Memory::storeOwn(uint64_t object, const State::Cell &own, SMTExprRef at,
                             const State::Cell &c) {
  SMTExprRef i = index(object, at);
  unsigned shift = shifts.lookup(object);
  if (!shift)
    return {t.store(own.data, i, c.data), t.store(own.prov, i, c.prov)};
  // Read, mask and write the element back. The byte's provenance goes: a
  // pointer stored here through an unknown pointer reads back as one made
  // from an integer.
  SMTExprRef elem = t.select(own.data, i);
  unsigned n = elem->getWidth();
  SMTExprRef sh = byteShift(t, offset(at), shift, n);
  SMTExprRef kept = t.bvand(elem, t.bvnot(t.shl(t.bv(0xff, n), sh)));
  return {t.store(own.data, i, t.bvor(kept, t.shl(t.zext(c.data, n - 8), sh))), own.prov};
}

void Memory::summarise(State &st, SMTExprRef first, uint64_t stride, Type *ty, SMTExprRef count,
                       Iteration iteration) {
  uint64_t o = t.constant(object(first))->getZExtValue();
  flush(st, o);
  const State::Cell *own = st.separate.find({o, 0});
  State::Cell below = own ? *own : State::Cell{st.data, st.prov};
  bool byOffset = own && owned.count(o);
  // An object of elements (#118) is read and written only as whole values
  // of ty, at multiples of their size: an element is a value or none.
  unsigned shift = byOffset ? shifts.lookup(o) : 0, bits = storeBits(dl, ty);
  assert((!shift || bits == 8u << shift) && "the loop's store is an access of an element");
  SMTExprRef start = offset(first), objectId = t.bv(o, ObjBits), intProv = t.bv(IntProvenance, ObjBits);
  auto offsetOf = [=, this](const SMTExprRef &j) {
    return !byOffset ? offset(j) : shift ? t.concat(j, t.bv(0, shift)) : j;
  };
  // Whether the loop wrote none of the elements at the offsets j may take,
  // as Terms::range gives them: below the first, past the last, or between
  // two, the same distance from an element each time.
  const APInt *from = t.constant(start), *iterations = t.constant(count);
  auto misses = [=, this](const SMTExprRef &j) {
    using U = unsigned __int128;
    std::optional<encode::Terms::Range> v;
    if (!byOffset && t.isFalse(t.eq(object(j), objectId)))
      return true;
    if (!from || !iterations || iterations->isZero() || !(v = t.range(offsetOf(j))))
      return false;
    U lo = from->getZExtValue(), end = lo + U(stride) * (iterations->getZExtValue() - 1) + bits / 8;
    return end <= U(1) << ptrBits &&
           (v->hi < lo || v->lo >= end ||
            (v->stride % stride == 0 && (v->lo + U(stride) - lo % stride) % stride >= bits / 8));
  };
  // The element at index j of the arrays: whether the loop wrote it, and
  // what, unless j is in another object.
  auto at = [=, this](const SMTExprRef &j) -> std::pair<SMTExprRef, SMTExprRef> {
    SMTExprRef in = byOffset ? t.boolean(true) : t.eq(object(j), objectId);
    if (t.isFalse(in))
      return {in, {}};
    SMTExprRef off = offsetOf(j);
    SMTExprRef d = t.sub(off, start), n, b;
    if (isPowerOf2_64(stride)) {
      n = t.lshr(d, t.bv(Log2_64(stride), ptrBits));
      b = t.bvand(d, t.bv(stride - 1, ptrBits));
    } else {
      n = t.udiv(d, t.bv(stride, ptrBits));
      b = t.urem(d, t.bv(stride, ptrBits));
    }
    auto [stored, v] = iteration(n);
    in = t.land(in, t.land(t.land(t.ult(n, count), t.ult(b, t.bv(bits / 8, ptrBits))), stored));
    if (ty->isIntegerTy(1))
      v = t.ite(v, t.bv(1, 8), t.bv(0, 8));
    else if (v->getWidth() < bits)
      v = t.zext(v, bits - v->getWidth());
    if (shift)
      return {in, v};
    SMTExprRef r = t.extract(v, 7, 0);
    for (unsigned i = 1; i < bits / 8; ++i)
      r = t.ite(t.eq(b, t.bv(i, ptrBits)), t.extract(v, 8 * i + 7, 8 * i), r);
    return {in, r};
  };
  SMTSortRef index = !byOffset ? pointerSort() : s.mkBVSort(ptrBits - shift);
  std::string n = std::to_string(fresh++);
  State::Cell after{t.lambda(s.mkArraySort(index, s.mkBVSort(8u << shift)), "summary#" + n,
                             [=, this](const SMTExprRef &j) {
                               auto [in, v] = at(j);
                               SMTExprRef old = t.select(below.data, j);
                               return t.isFalse(in) ? old : t.ite(in, v, old);
                             },
                             below.data, misses),
                    below.prov};
  // An object of elements has no provenance array: it holds no pointer.
  if (!shift) {
    after.prov = t.lambda(s.mkArraySort(index, s.mkBVSort(ObjBits)), "summary_provenance#" + n,
                          [=, this](const SMTExprRef &j) {
                            SMTExprRef old = t.select(below.prov, j);
                            return old.get() == intProv.get() ? old
                                                              : t.ite(at(j).first, intProv, old);
                          },
                          below.prov, misses);
    provBelow[after.prov.get()] = {after.prov, below.prov}; // the pointers it may hold
  }
  if (own)
    st.separate.set({o, 0}, after);
  else
    st.data = after.data, st.prov = after.prov;
}

void Memory::storeArrays(State &st, uint64_t object, SMTExprRef at, const State::Cell &c) {
  if (const State::Cell *own = st.separate.find({object, 0})) {
    st.separate.set({object, 0}, storeOwn(object, *own, at, c));
    return;
  }
  st.data = t.store(st.data, at, c.data);
  st.prov = t.store(st.prov, at, c.prov);
}

State::Cell Memory::readArrays(const State &st, SMTExprRef at) {
  const APInt *obj = t.constant(object(at));
  if (obj)
    if (const State::Cell *own = st.separate.find({obj->getZExtValue(), 0}))
      return readOwn(obj->getZExtValue(), *own, at);
  State::Cell r{t.select(st.data, at), t.select(st.prov, at)};
  if (!obj)
    st.separate.forEach([&](Key k, const State::Cell &own) {
      SMTExprRef is = t.eq(object(at), t.bv(k.first, ObjBits));
      State::Cell b = readOwn(k.first, own, at);
      r = {t.ite(is, b.data, r.data), t.ite(is, b.prov, r.prov)};
    });
  return r;
}

std::optional<uint64_t> Memory::element(SMTExprRef p, Type *ty) {
  SMTExprRef c, x, y;
  const APInt *obj = t.constant(object(p));
  if (!obj || key(p) || t.split(p, c, x, y))
    return std::nullopt;
  auto it = shifts.find(obj->getZExtValue());
  if (it == shifts.end() || !ty->isIntegerTy(8u << it->second))
    return std::nullopt;
  return obj->getZExtValue();
}

CellMap Memory::mergeSeparate(SMTExprRef c, const State &a, const State &b) {
  if (a.separate.same(b.separate))
    return a.separate;
  // An object separate on one side only reads the other side's shared
  // arrays, which hold its bytes there under the same index; one given
  // arrays by own() on a path that has not created it yet, its initial ones.
  auto arrays = [this](const State &st, uint64_t o) {
    if (const State::Cell *own = st.separate.find({o, 0}))
      return *own;
    auto it = owned.find(o);
    return it != owned.end() ? it->second : State::Cell{st.data, st.prov};
  };
  CellMap r;
  auto add = [&](Key k, const State::Cell &) {
    if (r.find(k))
      return;
    State::Cell x = arrays(a, k.first), y = arrays(b, k.first);
    r.set(k, {t.ite(c, x.data, y.data), t.ite(c, x.prov, y.prov)});
  };
  a.separate.forEach(add);
  b.separate.forEach(add);
  return r;
}

void Memory::prepare(State &st, SMTExprRef p) {
  SMTExprRef c, x, y;
  if (key(p) || t.split(p, c, x, y))
    return;
  for (uint64_t o : candidates(p))
    flush(st, o);
}

std::vector<uint64_t> Memory::candidates(SMTExprRef p, bool integers, bool *read) {
  std::vector<uint64_t> r;
  std::vector<SMTExprRef> work{p};
  bool fromMemory = false;
  while (!work.empty()) {
    SMTExprRef e = work.back(), c, x, y;
    work.pop_back();
    // A choice of pointers whose sides are not constant keeps its object
    // inside, out of reach of object(): split the pointer first.
    if (e->getWidth() != ObjBits && !t.choice(e, c, x, y))
      e = object(e);
    if (!integers && fromInteger.count(e.get()))
      continue;
    if (const APInt *id = t.constant(e)) {
      r.push_back(id->getZExtValue());
    } else if (t.choice(e, c, x, y)) {
      work.push_back(x);
      work.push_back(y);
    } else {
      fromMemory = true;
    }
  }
  if (read)
    *read = fromMemory;
  else if (fromMemory) // where only pointers to exposed objects are stored
    for (uint64_t id = 0; id < objects.size(); ++id)
      if (id == 0 || objects[id].exposed)
        r.push_back(id);
  llvm::sort(r);
  r.erase(std::unique(r.begin(), r.end()), r.end());
  return r;
}

int Memory::side(const SMTExprRef &c) {
  if (!context)
    return -1;
  if (t.entails(context, c))
    return 1;
  return t.entails(context, t.lnot(c)) ? 0 : -1;
}

State::Cell Memory::readByte(const State &st, SMTExprRef at) {
  SMTExprRef c, x, y;
  if (t.split(at, c, x, y)) { // one of two constant pointers
    if (int pick = side(c); pick >= 0)
      return readByte(st, pick ? x : y);
    State::Cell a = readByte(st, x), b = readByte(st, y);
    return {t.ite(c, a.data, b.data), t.ite(c, a.prov, b.prov)};
  }
  if (std::optional<Key> k = key(at))
    if (const State::Cell *c = st.known.find(*k))
      return *c;
  return readArrays(st, at);
}

void Memory::writeByte(State &st, SMTExprRef at, const State::Cell &c) {
  SMTExprRef cond, x, y;
  if (t.split(at, cond, x, y)) { // each side keeps its old byte unless chosen
    if (int pick = side(cond); pick >= 0) {
      writeByte(st, pick ? x : y, c);
      return;
    }
    State::Cell a = readByte(st, x), b = readByte(st, y);
    writeByte(st, x, {t.ite(cond, c.data, a.data), t.ite(cond, c.prov, a.prov)});
    writeByte(st, y, {t.ite(cond, b.data, c.data), t.ite(cond, b.prov, c.prov)});
    return;
  }
  std::optional<Key> k = key(at);
  if (!k) {
    // The shared arrays take every write that may miss the separate
    // objects; reads of a separate object never look there.
    const APInt *obj = t.constant(object(at));
    bool shared = true;
    CellMap separate = st.separate;
    st.separate.forEach([&](Key k, const State::Cell &own) {
      uint64_t o = k.first;
      if (obj && obj->getZExtValue() != o)
        return;
      SMTExprRef is = obj ? t.boolean(true) : t.eq(object(at), t.bv(o, ObjBits));
      State::Cell stored = storeOwn(o, own, at, c);
      separate.set(k, {t.ite(is, stored.data, own.data), t.ite(is, stored.prov, own.prov)});
      shared = !obj;
    });
    st.separate = separate;
    if (shared) {
      st.data = t.store(st.data, at, c.data);
      st.prov = t.store(st.prov, at, c.prov);
    }
    return;
  }
  st.known.set(*k, c);
  if (st.known.size() > MaxKnown)
    flush(st);
}

State Memory::ite(SMTExprRef c, const State &a0, const State &b0) {
  if (a0.known.same(b0.known))
    return {t.ite(c, a0.data, b0.data), t.ite(c, a0.prov, b0.prov), a0.known,
            mergeSeparate(c, a0, b0)};
  // An object whose bytes would become a choice between two values that
  // multiply or divide goes to the arrays on both sides instead. There the
  // solver bit-blasts a stored value only where a read needs it; as a choice,
  // all of them up front (RERS Problem14 ran out of memory). So does one
  // with more than MaxChoices bytes that differ, counted before any byte is
  // read: reading many bytes from a side that flushed the object walks the
  // flush's stores for each, and a choice per byte of a large object copied
  // on both sides solved slower than a choice of arrays.
  State a = a0, b = b0;
  std::set<uint64_t> moved;
  llvm::DenseMap<uint64_t, size_t> differ;
  CellMap::diff(a.known, b.known, [&](Key k, const State::Cell *, const State::Cell *) {
    if (++differ[k.first] == MaxChoices + 1)
      moved.insert(k.first);
  });
  // Only the bytes whose cells differ need a choice; the rest stay as a has
  // them. A byte known on one side only reads the other side's arrays, which
  // the flushes below change only at other objects.
  std::vector<std::pair<Key, std::pair<State::Cell, State::Cell>>> sides;
  CellMap::diff(a.known, b.known, [&](Key k, const State::Cell *x, const State::Cell *y) {
    if (moved.count(k.first))
      return;
    State::Cell xa = x ? *x : readByte(a, ptr(k)), yb = y ? *y : readByte(b, ptr(k));
    if (xa.data.get() != yb.data.get() && (t.arithmetic(xa.data) || t.arithmetic(yb.data)))
      moved.insert(k.first);
    sides.push_back({k, {xa, yb}});
  });
  for (uint64_t object : moved) {
    flush(a, object);
    flush(b, object);
  }
  CellMap known = a.known;
  for (const auto &[key, xy] : sides)
    if (!moved.count(key.first))
      known.set(key, {t.ite(c, xy.first.data, xy.second.data),
                      t.ite(c, xy.first.prov, xy.second.prov)});
  State r{t.ite(c, a.data, b.data), t.ite(c, a.prov, b.prov), std::move(known),
          mergeSeparate(c, a, b)};
  if (r.known.size() > MaxKnown)
    flush(r);
  return r;
}

// Bits in memory order: byte i of the store is bits [8i+7, 8i].
void Memory::storeBytes(State &st, SMTExprRef p, SMTExprRef bits, SMTExprRef prov) {
  unsigned n = bits->getWidth() / 8;
  if (std::optional<Key> k = key(p)) {
    // A constant address: the bytes' keys are integers, so no address term
    // is built until a flush needs one. Offsets wrap at the pointer width.
    uint64_t mask = ptrBits >= 64 ? ~uint64_t(0) : (uint64_t(1) << ptrBits) - 1;
    for (unsigned i = 0; i < n; ++i)
      st.known.set({k->first, (k->second + i) & mask}, {t.extract(bits, 8 * i + 7, 8 * i), prov});
    if (st.known.size() > MaxKnown)
      flush(st);
    return;
  }
  prepare(st, p);
  for (unsigned i = 0; i < n; ++i)
    writeByte(st, bytePtr(p, i), {t.extract(bits, 8 * i + 7, 8 * i), prov});
}

void Memory::store(State &st, SMTExprRef p, SMTExprRef v, Type *ty) {
  unsigned bits = storeBits(dl, ty);
  if (ty->isPointerTy()) {
    // A pointer with no object but an offset, outside the program, was
    // made from an integer, so it is stored as one and a load converts it
    // again (#75). Null keeps object 0 and is never converted. The test is
    // on the offset, which, unlike the address, folds on each side of
    // ite(ok, p, null).
    SMTExprRef obj = object(v);
    SMTExprRef made = t.land(t.eq(obj, t.bv(0, ObjBits)), t.lnot(t.eq(offset(v), t.bv(0, ptrBits))));
    storeBytes(st, p, address(v), t.ite(made, t.bv(IntProvenance, ObjBits), obj));
    return;
  }
  if (!ty->isIntegerTy() && !ty->isFloatingPointTy()) // a float is its bits
    throw Unsupported("store of a non-integer value");
  if (std::optional<uint64_t> o = element(p, ty)) {
    prepare(st, p);
    const State::Cell &own = *st.separate.find({*o, 0});
    st.separate.set({*o, 0}, {t.store(own.data, index(*o, p), v), own.prov});
    return;
  }
  if (ty->isIntegerTy(1))
    v = t.ite(v, t.bv(1, 8), t.bv(0, 8));
  else if (v->getWidth() < bits)
    v = t.zext(v, bits - v->getWidth());
  storeBytes(st, p, v, t.bv(IntProvenance, ObjBits));
}

SMTExprRef Memory::load(State &st, SMTExprRef p, Type *ty) {
  if (!ty->isIntegerTy() && !ty->isPointerTy() && !ty->isFloatingPointTy())
    throw Unsupported("load of a non-integer value");
  prepare(st, p);
  if (std::optional<uint64_t> o = element(p, ty))
    return t.select(st.separate.find({*o, 0})->data, index(*o, p));
  unsigned n = storeBits(dl, ty) / 8;
  State::Cell first = readByte(st, p);
  SMTExprRef bits = first.data;
  for (unsigned i = 1; i < n; ++i)
    bits = t.concat(readByte(st, bytePtr(p, i)).data, bits);
  if (ty->isPointerTy()) {
    // The provenance of the first byte stands for the pointer.
    SMTExprRef obj = first.prov;
    // Bytes of an integer, among them a pointer made from one (see store),
    // which an object allocated since may now cover: the address is
    // converted now, as a fresh conversion would be (#75).
    SMTExprRef isInt = t.eq(obj, t.bv(IntProvenance, ObjBits));
    return t.ite(isInt, fromAddress(bits), ptr(obj, t.sub(bits, base(obj))));
  }
  if (ty->isIntegerTy(1))
    return t.eq(t.extract(bits, 0, 0), t.bv(1, 1));
  unsigned width = ty->getPrimitiveSizeInBits();
  return width < bits->getWidth() ? t.extract(bits, width - 1, 0) : bits;
}

// One array store per byte, so a large zeroed global or memset is
// a long store chain. A range store or a constant base would fix it.
bool Memory::fillObject(State &st, SMTExprRef p, SMTExprRef byte, uint64_t n) {
  SMTExprRef c, x, y;
  if (t.split(p, c, x, y)) { // a malloc result: the context may pick a side
    int pick = side(c);
    if (pick < 0)
      return false;
    p = pick ? x : y;
  }
  std::optional<Key> k = key(p);
  if (n < MinSeparate || !k || k->first == 0 || k->first >= objects.size() || k->second != 0)
    return false;
  const APInt *size = t.constant(objects[k->first].size);
  if (!size || *size != n)
    return false;
  fill(st, k->first, byte);
  return true;
}

void Memory::fill(State &st, SMTExprRef p, SMTExprRef byte) {
  fill(st, key(p)->first, byte);
}

void Memory::fill(State &st, uint64_t o, SMTExprRef byte) {
  st.known.eraseObject(o); // every byte is overwritten, so its known bytes are stale
  unsigned shift = shifts.lookup(o);
  SMTSortRef index = owned.count(o) ? s.mkBVSort(ptrBits - shift) : pointerSort();
  SMTExprRef elem = byte, prov = t.bv(IntProvenance, ObjBits);
  for (unsigned j = 1; j < 1u << shift; ++j)
    elem = t.concat(byte, elem);
  st.separate.set({o, 0}, {t.constArray(index, elem), shift ? prov : t.constArray(index, prov)});
}

void Memory::havoc(State &st, SMTExprRef p, std::optional<uint64_t> n) {
  havocWith(st, p, n, t.bv(IntProvenance, ObjBits), true);
}

std::vector<std::pair<const GlobalVariable *, SMTExprRef>> Memory::readOnly(SMTExprRef p) {
  std::vector<std::pair<const GlobalVariable *, SMTExprRef>> r;
  for (uint64_t o : candidates(p, false))
    if (o < objects.size())
      if (auto *g = dyn_cast_or_null<GlobalVariable>(objects[o].constant))
        r.push_back({g, into(p, o)});
  return r;
}

SMTExprRef Memory::into(SMTExprRef obj, uint64_t o) {
  SMTExprRef c, x, y;
  // As candidates() walks it.
  if (obj->getWidth() != ObjBits && !t.choice(obj, c, x, y))
    obj = object(obj);
  if (fromInteger.count(obj.get()))
    return t.boolean(false);
  if (t.choice(obj, c, x, y))
    return t.ite(c, into(x, o), into(y, o));
  return t.eq(obj, t.bv(o, ObjBits));
}

void Memory::havocCall(State &st, llvm::ArrayRef<SMTExprRef> written,
                       llvm::ArrayRef<SMTExprRef> read, bool reach) {
  std::vector<SMTExprRef> ps(written.begin(), written.end());
  ps.insert(ps.end(), read.begin(), read.end());
  // For each pointer, the objects it may point into, and each object
  // reached through stored pointers from where it points, in st as the
  // call finds it, with the objects of the pointer it is reached from.
  std::vector<std::vector<uint64_t>> roots(ps.size());
  std::vector<llvm::MapVector<uint64_t, std::vector<uint64_t>>> from(ps.size());
  // The objects the call may hold a pointer to: those it gets a pointer
  // into, exposed (it can store no other: the rest were passed where
  // LLVM says nothing is captured), and those reached.
  llvm::SetVector<uint64_t> held;
  llvm::DenseMap<uint64_t, llvm::DenseSet<uint64_t>> to;
  if (reach)
    to = pointers(st);
  for (size_t i = 0; i < ps.size(); ++i) {
    if (t.isTrue(t.eq(ps[i], null())))
      continue;
    roots[i] = candidates(ps[i]);
    for (uint64_t r : roots[i]) {
      if (r < objects.size() && objects[r].exposed)
        held.insert(r);
      llvm::SmallVector<uint64_t, 8> work{r};
      llvm::DenseSet<uint64_t> seen;
      while (!work.empty()) {
        auto it = to.find(work.pop_back_val());
        if (it == to.end())
          continue;
        for (uint64_t x : it->second)
          if (seen.insert(x).second) {
            from[i][x].push_back(r);
            held.insert(x);
            work.push_back(x);
          }
      }
    }
  }
  // Every object written may then hold a pointer to any of them, so that a
  // later call reaches them again.
  SMTExprRef prov = t.bv(IntProvenance, ObjBits);
  std::string id = std::to_string(fresh++);
  for (uint64_t x : held)
    prov = t.ite(t.symbol("havoc_provenance#" + id + "." + std::to_string(x), s.mkBoolSort()),
                 t.bv(x, ObjBits), prov);
  for (size_t i = 0; i < ps.size(); ++i) {
    if (i < written.size())
      havocWith(st, ps[i], std::nullopt, prov, false);
    for (const auto &[x, rs] : from[i]) {
      if (objects[x].constant)
        continue;
      if (rs.size() == roots[i].size()) {
        havocObject(st, x, prov);
        continue;
      }
      State a = st;
      havocObject(a, x, prov);
      SMTExprRef in = t.boolean(false);
      for (uint64_t r : rs)
        in = t.lor(in, t.eq(object(ps[i]), t.bv(r, ObjBits)));
      st = ite(in, a, st);
    }
  }
}

void Memory::havocWith(State &st, SMTExprRef p, std::optional<uint64_t> n, SMTExprRef prov,
                       bool constants) {
  SMTExprRef c, x, y;
  if (t.split(p, c, x, y)) {
    State a = st, b = st;
    havocWith(a, x, n, prov, constants);
    havocWith(b, y, n, prov, constants);
    st = ite(c, a, b);
    return;
  }
  const APInt *obj = t.constant(object(p));
  if (!obj) { // each object p may point into, where it does
    for (uint64_t o : candidates(p)) {
      State a = st;
      havocWith(a, ptr(t.bv(o, ObjBits), offset(p)), n, prov, constants);
      st = ite(t.eq(object(p), t.bv(o, ObjBits)), a, st);
    }
    return;
  }
  uint64_t o = obj->getZExtValue();
  std::optional<Key> k = key(p);
  // Null writes nothing; any other pointer with object 0 is into the memory
  // outside the program, which the call may write as a whole.
  if (o >= objects.size() || (o == 0 && k && k->second == 0) ||
      (objects[o].constant && !constants))
    return;
  const APInt *size = t.constant(objects[o].size);
  if (n && !(k && k->second == 0 && size && *size == *n)) { // part of the object
    if (*n > MaxBytes)
      throw Unsupported("nondet contents for " + std::to_string(*n) + " bytes");
    prepare(st, p);
    for (uint64_t i = 0; i < *n; ++i)
      writeByte(st, bytePtr(p, i), {t.symbol("havoc#" + std::to_string(fresh++), s.mkBVSort(8)),
                                     t.bv(IntProvenance, ObjBits)});
    return;
  }
  havocObject(st, o, prov);
}

void Memory::havocObject(State &st, uint64_t o, SMTExprRef prov) {
  unsigned shift = shifts.lookup(o); // an object of elements, as in fillObject
  SMTSortRef index = owned.count(o) ? s.mkBVSort(ptrBits - shift) : pointerSort();
  std::string id = std::to_string(fresh++);
  st.known.eraseObject(o);
  // An object of elements holds no pointer.
  st.separate.set({o, 0}, {t.symbol("havoc#" + id, s.mkArraySort(index, s.mkBVSort(8 << shift))),
                           shift ? t.bv(IntProvenance, ObjBits) : t.constArray(index, prov)});
}

llvm::DenseMap<uint64_t, llvm::DenseSet<uint64_t>> Memory::pointers(const State &st) {
  llvm::DenseMap<uint64_t, llvm::DenseSet<uint64_t>> to;
  SMTExprRef intProv = t.bv(IntProvenance, ObjBits);
  // A pointer read from memory may point to every exposed object, which
  // goes to an object once: each byte of a pointer copied may have a
  // provenance of its own.
  std::vector<uint64_t> exposed;
  for (uint64_t id = 1; id < objects.size(); ++id)
    if (objects[id].exposed)
      exposed.push_back(id);
  llvm::DenseSet<uint64_t> holdsRead;
  // The objects a byte of provenance prov may point to, for each object in,
  // with each provenance looked up once.
  llvm::DenseMap<const camada::SMTExpr *, std::pair<std::vector<uint64_t>, bool>> targets;
  auto add = [&](llvm::ArrayRef<uint64_t> in, const SMTExprRef &prov) {
    auto [it, inserted] = targets.try_emplace(prov.get());
    auto &[xs, read] = it->second;
    if (inserted)
      for (uint64_t x : candidates(prov, false, &read))
        if (x && x < objects.size()) // not null, nor an integer's
          xs.push_back(x);
    for (uint64_t o : in) {
      if (!xs.empty())
        to[o].insert(xs.begin(), xs.end());
      if (read && !exposed.empty() && holdsRead.insert(o).second)
        to[o].insert(exposed.begin(), exposed.end());
    }
  };
  // A pointer's bytes share one provenance: add each once per object.
  llvm::DenseSet<std::pair<uint64_t, const camada::SMTExpr *>> added;
  st.known.forEach([&](Key k, const State::Cell &c) {
    if (c.prov.get() != intProv.get() && added.insert({k.first, c.prov.get()}).second)
      add(k.first, c.prov);
  });
  // The provenance stored in the arrays: in those indexed by pointer, each
  // store for the objects its index may be in, which may count a store an
  // object's own arrays have since replaced; in an object's own indexed by
  // offset, each for that object. A constant array is an object's own, all
  // for it. An object's own arrays may choose the shared ones on one side
  // of a join, which the first walk has seen already.
  llvm::DenseSet<const camada::SMTExpr *> shared;
  auto walk = [&](const SMTExprRef &arrays, std::optional<uint64_t> own, bool byOffset) {
    llvm::DenseSet<const camada::SMTExpr *> local, &seen = own ? local : shared;
    std::vector<SMTExprRef> work{arrays};
    while (!work.empty()) {
      SMTExprRef e = work.back(), a, i, v, c;
      work.pop_back();
      if (!seen.insert(e.get()).second || (own && shared.count(e.get())))
        continue;
      if (SMTExprRef f = t.filledWith(e)) {
        if (f.get() == intProv.get())
          continue;
        if (!own)
          throw Unsupported("memory whose provenance is not known");
        add(*own, f);
        continue;
      }
      if (t.stored(e, a, i, v)) {
        if (v.get() != intProv.get())
          add(byOffset ? std::vector<uint64_t>{*own} : candidates(i), v);
        work.push_back(a);
      } else if (t.choice(e, c, a, v)) {
        work.push_back(a);
        work.push_back(v);
      } else if (auto it = provBelow.find(e.get()); it != provBelow.end()) {
        work.push_back(it->second.second);
      } else {
        throw Unsupported("memory whose provenance is not known");
      }
    }
  };
  walk(st.prov, std::nullopt, false);
  st.separate.forEach([&](Key k, const State::Cell &c) {
    if (!shifts.count(k.first))
      walk(c.prov, k.first, owned.count(k.first));
  });
  return to;
}

void Memory::set(State &st, SMTExprRef p, SMTExprRef byte, uint64_t n) {
  if (fillObject(st, p, byte, n))
    return;
  if (n > MaxBytes)
    throw Unsupported("a fill of " + std::to_string(n) + " bytes, not of a whole object");
  setBytes(st, p, byte, n);
}

void Memory::setBytes(State &st, SMTExprRef p, SMTExprRef byte, uint64_t n) {
  prepare(st, p);
  SMTExprRef prov = t.bv(IntProvenance, ObjBits);
  for (uint64_t i = 0; i < n; ++i)
    writeByte(st, bytePtr(p, i), {byte, prov});
}

void Memory::copy(State &st, SMTExprRef dst, SMTExprRef src, uint64_t n) {
  if (n > MaxBytes)
    throw Unsupported("a copy of " + std::to_string(n) + " bytes");
  // Read everything first, so overlapping ranges copy as memmove does.
  prepare(st, src);
  prepare(st, dst);
  std::vector<State::Cell> bytes;
  for (uint64_t i = 0; i < n; ++i)
    bytes.push_back(readByte(st, bytePtr(src, i)));
  for (uint64_t i = 0; i < n; ++i)
    writeByte(st, bytePtr(dst, i), bytes[i]);
}

void Memory::initialise(State &st, const Module &m,
                        const std::function<SMTExprRef(const Constant *)> &valueOf) {
  for (const GlobalVariable &g : m.globals()) {
    if (!g.hasInitializer()) // extern: contents unknown
      continue;
    const Constant *init = g.getInitializer();
    // Static storage zeroes padding, which storeConstant does not write.
    if (hasPadding(init->getType()) && !init->isNullValue())
      fillOrSet(st, globals.lookup(&g), t.bv(0, 8),
          dl.getTypeAllocSize(init->getType()).getFixedValue());
    storeConstant(st, globals.lookup(&g), init, valueOf);
  }
}

bool Memory::hasPadding(Type *ty) const {
  if (auto *st_ = dyn_cast<StructType>(ty))
    return dl.getStructLayout(st_)->hasPadding() ||
           llvm::any_of(st_->elements(), [this](Type *e) { return hasPadding(e); });
  if (auto *at = dyn_cast<ArrayType>(ty))
    return hasPadding(at->getElementType()) ||
           dl.getTypeAllocSize(at->getElementType()) != dl.getTypeStoreSize(at->getElementType());
  return false;
}

void Memory::storeConstant(State &st, SMTExprRef p, const Constant *c,
                           const std::function<SMTExprRef(const Constant *)> &valueOf) {
  Type *ty = c->getType();
  if (isa<UndefValue>(c)) // also poison: leave the contents nondet
    return;
  if (c->isNullValue()) {
    fillOrSet(st, p, t.bv(0, 8), dl.getTypeStoreSize(ty).getFixedValue());
    return;
  }
  if (auto *cf = dyn_cast<ConstantFP>(c)) { // memory holds a float's bits
    store(st, p, t.bv(cf->getValueAPF().bitcastToAPInt()), ty);
    return;
  }
  if (ty->isIntegerTy() || ty->isPointerTy()) {
    store(st, p, valueOf(c), ty);
    return;
  }
  if (auto *st_ = dyn_cast<StructType>(ty)) {
    const StructLayout *layout = dl.getStructLayout(st_);
    for (unsigned i = 0; i < st_->getNumElements(); ++i)
      storeConstant(st, bytePtr(p, layout->getElementOffset(i)), c->getAggregateElement(i),
                    valueOf);
    return;
  }
  if (auto *at = dyn_cast<ArrayType>(ty)) {
    uint64_t stride = dl.getTypeAllocSize(at->getElementType());
    for (uint64_t i = 0; i < at->getNumElements(); ++i)
      storeConstant(st, bytePtr(p, i * stride), c->getAggregateElement(i), valueOf);
    return;
  }
  // A vector's elements go where the scalarizer's element pointers read
  // them, one alloc size apart; vectors of i1 pack bits and are not handled.
  if (auto *vt = dyn_cast<FixedVectorType>(ty);
      vt && dl.getTypeSizeInBits(vt->getElementType()) % 8 == 0) {
    uint64_t stride = dl.getTypeAllocSize(vt->getElementType());
    for (unsigned i = 0; i < vt->getNumElements(); ++i)
      storeConstant(st, bytePtr(p, i * stride), c->getAggregateElement(i), valueOf);
    return;
  }
  throw Unsupported("global initialiser of this type");
}

} // namespace vereda::memory
