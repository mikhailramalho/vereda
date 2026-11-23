#include "memory/memory.h"

#include "util/unsupported.h"
#include <llvm/IR/GlobalVariable.h>
#include <set>

using namespace llvm;
using camada::SMTExprRef;
using camada::SMTSortRef;
using vereda::Unsupported;

namespace vereda::memory {

static const unsigned ObjBits = 32;
static const int64_t IntProvenance = -1; // a byte that holds no pointer

Memory::Memory(encode::Terms &t, const DataLayout &dl)
    : t(t), s(t.solver()), dl(dl), ptrBits(dl.getPointerSizeInBits(0)) {
  if (dl.isBigEndian())
    throw Unsupported("big-endian targets");
  bases = t.symbol("bases", s.mkArraySort(s.mkBVSort(ObjBits), s.mkBVSort(ptrBits)));
  // Constrained before the object is recorded, as allocate() does: after it,
  // base() of the constant 0 folds to objects[0].base and the constraint to
  // true, which left bases[0] free and a stored null read back non-null.
  s.addConstraint(t.eq(base(t.bv(0, ObjBits)), t.bv(0, ptrBits)));
  objects.push_back({t.bv(0, ptrBits), t.bv(0, ptrBits)});
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

SMTExprRef Memory::allocate(SMTExprRef size, uint64_t align, SMTExprRef exists) {
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
  objects.push_back({b, size});
  end = t.ite(exists, e, end); // an object not allocated takes no space
  return ptr(obj, t.bv(0, ptrBits));
}

void Memory::allocateGlobals(const Module &m) {
  for (const GlobalVariable &g : m.globals()) {
    uint64_t size = dl.getTypeAllocSize(g.getValueType());
    globals[&g] = allocate(t.bv(size, ptrBits), g.getPointerAlignment(dl).value(),
                           t.boolean(true));
  }
  // A function whose address is taken is an object of one byte, so that a
  // pointer to it is an ordinary pointer: stored, compared and called.
  for (const Function &f : m)
    if (f.hasAddressTaken(nullptr, false, true, false, false, /*IgnoreCastedDirectCall=*/true))
      globals[&f] = allocate(t.bv(1, ptrBits), 1, t.boolean(true));
}

SMTExprRef Memory::global(const GlobalValue *g) const {
  auto it = globals.find(g);
  if (it == globals.end())
    throw Unsupported("address of " + g->getName().str());
  return it->second;
}

SMTExprRef Memory::address(SMTExprRef p) { return t.add(base(object(p)), offset(p)); }

SMTExprRef Memory::fromAddress(SMTExprRef a) {
  // The object whose range holds a, or null with offset a.
  // One comparison per object created so far; the static
  // points-to analysis will narrow this.
  SMTExprRef r = ptr(t.bv(0, ObjBits), a);
  for (unsigned id = 1; id < objects.size(); ++id) {
    const Object &o = objects[id];
    // One past the end still belongs to the object: the gap keeps it unique.
    SMTExprRef inside = t.land(t.uge(a, o.base), t.ule(a, t.add(o.base, o.size)));
    r = t.ite(inside, ptr(t.bv(id, ObjBits), t.sub(a, o.base)), r);
  }
  return r;
}

State Memory::initial() {
  SMTSortRef index = pointerSort();
  std::string n = std::to_string(fresh++);
  return {t.symbol("memory#" + n, s.mkArraySort(index, s.mkBVSort(8))),
          t.symbol("provenance#" + n, s.mkArraySort(index, s.mkBVSort(ObjBits)))};
}

// Past this many known bytes in a state, all go to the arrays,
// which bounds the copy on write; a persistent map would lift it.
static const size_t MaxKnown = 4096;

std::optional<Memory::Key> Memory::key(SMTExprRef p) const {
  const APInt *v = t.constant(p);
  if (!v)
    return std::nullopt;
  return Key{v->extractBitsAsZExtValue(ObjBits, ptrBits), v->extractBitsAsZExtValue(ptrBits, 0)};
}

SMTExprRef Memory::ptr(const Key &k) {
  return ptr(t.bv(k.first, ObjBits), t.bv(k.second, ptrBits));
}

State::Known &Memory::own(State &st) {
  if (!st.known)
    st.known = std::make_shared<State::Known>();
  else if (st.known.use_count() > 1)
    st.known = std::make_shared<State::Known>(*st.known);
  return *st.known;
}

void Memory::flush(State &st, std::optional<uint64_t> object) {
  if (!st.known)
    return;
  // Keys sort by object first, so one object's bytes are one range.
  auto range = [&] {
    auto &k = *st.known;
    return object ? std::make_pair(k.lower_bound({*object, 0}), k.lower_bound({*object + 1, 0}))
                  : std::make_pair(k.begin(), k.end());
  };
  auto [lo, hi] = range();
  if (lo == hi)
    return;
  if (st.known.use_count() > 1) {
    own(st);
    std::tie(lo, hi) = range();
  }
  for (auto it = lo; it != hi; ++it) {
    SMTExprRef at = ptr(it->first);
    auto [data, prov] = arraysOf(st, it->first.first);
    data = t.store(data, at, it->second.data);
    prov = t.store(prov, at, it->second.prov);
  }
  st.known->erase(lo, hi);
}

State::Separate &Memory::ownSeparate(State &st) {
  if (!st.separate)
    st.separate = std::make_shared<State::Separate>();
  else if (st.separate.use_count() > 1)
    st.separate = std::make_shared<State::Separate>(*st.separate);
  return *st.separate;
}

std::pair<SMTExprRef &, SMTExprRef &> Memory::arraysOf(State &st, uint64_t object) {
  if (st.separate && st.separate->count(object)) {
    State::Cell &own = ownSeparate(st)[object];
    return {own.data, own.prov};
  }
  return {st.data, st.prov};
}

State::Cell Memory::readArrays(const State &st, SMTExprRef at) {
  const APInt *obj = t.constant(object(at));
  if (st.separate && obj)
    if (auto it = st.separate->find(obj->getZExtValue()); it != st.separate->end())
      return {t.select(it->second.data, at), t.select(it->second.prov, at)};
  State::Cell r{t.select(st.data, at), t.select(st.prov, at)};
  if (st.separate && !obj)
    for (const auto &[o, own] : *st.separate) {
      SMTExprRef is = t.eq(object(at), t.bv(o, ObjBits));
      r = {t.ite(is, t.select(own.data, at), r.data), t.ite(is, t.select(own.prov, at), r.prov)};
    }
  return r;
}

std::shared_ptr<State::Separate> Memory::mergeSeparate(SMTExprRef c, const State &a,
                                                       const State &b) {
  if (a.separate == b.separate)
    return a.separate;
  // An object separate on one side only reads the other side's shared
  // arrays, which hold its bytes there under the same index.
  auto arrays = [](const State &st, uint64_t o) {
    if (st.separate)
      if (auto it = st.separate->find(o); it != st.separate->end())
        return it->second;
    return State::Cell{st.data, st.prov};
  };
  auto r = std::make_shared<State::Separate>();
  for (const State *st : {&a, &b})
    if (st->separate)
      for (const auto &e : *st->separate)
        if (!r->count(e.first)) {
          State::Cell x = arrays(a, e.first), y = arrays(b, e.first);
          (*r)[e.first] = {t.ite(c, x.data, y.data), t.ite(c, x.prov, y.prov)};
        }
  return r;
}

void Memory::prepare(State &st, SMTExprRef p) {
  SMTExprRef c, x, y;
  if (key(p) || t.split(p, c, x, y))
    return;
  const APInt *obj = t.constant(object(p));
  flush(st, obj ? std::optional<uint64_t>(obj->getZExtValue()) : std::nullopt);
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
  if (std::optional<Key> k = key(at); k && st.known)
    if (auto it = st.known->find(*k); it != st.known->end())
      return it->second;
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
    if (st.separate)
      for (auto &[o, own] : ownSeparate(st)) {
        if (obj && obj->getZExtValue() != o)
          continue;
        SMTExprRef is = obj ? t.boolean(true) : t.eq(object(at), t.bv(o, ObjBits));
        own.data = t.ite(is, t.store(own.data, at, c.data), own.data);
        own.prov = t.ite(is, t.store(own.prov, at, c.prov), own.prov);
        shared = !obj;
      }
    if (shared) {
      st.data = t.store(st.data, at, c.data);
      st.prov = t.store(st.prov, at, c.prov);
    }
    return;
  }
  own(st)[*k] = c;
  if (st.known->size() > MaxKnown)
    flush(st);
}

State Memory::ite(SMTExprRef c, const State &a0, const State &b0) {
  if (a0.known == b0.known)
    return {t.ite(c, a0.data, b0.data), t.ite(c, a0.prov, b0.prov), a0.known,
            mergeSeparate(c, a0, b0)};
  // An object whose bytes would become a choice between two values that
  // multiply or divide goes to the arrays on both sides instead. There the
  // solver bit-blasts a stored value only where a read needs it; as a choice,
  // all of them up front (RERS Problem14 ran out of memory).
  State a = a0, b = b0;
  std::map<Key, std::pair<State::Cell, State::Cell>> sides;
  std::set<uint64_t> moved;
  for (const State *st : {&a, &b})
    if (st->known)
      for (const auto &e : *st->known) {
        auto [it, added] = sides.try_emplace(e.first);
        if (!added)
          continue;
        // A byte known on one side only reads the other side's arrays,
        // which the flushes below change only at other objects.
        it->second = {readByte(a, ptr(e.first)), readByte(b, ptr(e.first))};
        const SMTExprRef &x = it->second.first.data, &y = it->second.second.data;
        if (x.get() != y.get() && (t.arithmetic(x) || t.arithmetic(y)))
          moved.insert(e.first.first);
      }
  for (uint64_t object : moved) {
    flush(a, object);
    flush(b, object);
  }
  auto known = std::make_shared<State::Known>();
  for (const auto &[key, xy] : sides)
    if (!moved.count(key.first))
      known->emplace(key, State::Cell{t.ite(c, xy.first.data, xy.second.data),
                                      t.ite(c, xy.first.prov, xy.second.prov)});
  State r{t.ite(c, a.data, b.data), t.ite(c, a.prov, b.prov), std::move(known),
          mergeSeparate(c, a, b)};
  if (r.known->size() > MaxKnown)
    flush(r);
  return r;
}

// Bits in memory order: byte i of the store is bits [8i+7, 8i].
void Memory::storeBytes(State &st, SMTExprRef p, SMTExprRef bits, SMTExprRef prov) {
  prepare(st, p);
  unsigned n = bits->getWidth() / 8;
  for (unsigned i = 0; i < n; ++i)
    writeByte(st, bytePtr(p, i), {t.extract(bits, 8 * i + 7, 8 * i), prov});
}

static unsigned storeBits(const DataLayout &dl, Type *ty) {
  return 8 * dl.getTypeStoreSize(ty).getFixedValue();
}

void Memory::store(State &st, SMTExprRef p, SMTExprRef v, Type *ty) {
  unsigned bits = storeBits(dl, ty);
  if (ty->isPointerTy()) {
    storeBytes(st, p, address(v), object(v));
    return;
  }
  if (!ty->isIntegerTy() && !ty->isFloatingPointTy()) // a float is its bits
    throw Unsupported("store of a non-integer value");
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
  unsigned n = storeBits(dl, ty) / 8;
  State::Cell first = readByte(st, p);
  SMTExprRef bits = first.data;
  for (unsigned i = 1; i < n; ++i)
    bits = t.concat(readByte(st, bytePtr(p, i)).data, bits);
  if (ty->isPointerTy()) {
    // The provenance of the first byte stands for the pointer.
    SMTExprRef obj = first.prov;
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
  uint64_t o = k->first;
  if (st.known) { // every byte is overwritten, so its known bytes are stale
    State::Known &known = own(st);
    known.erase(known.lower_bound({o, 0}), known.lower_bound({o + 1, 0}));
  }
  ownSeparate(st)[o] = {t.constArray(pointerSort(), byte),
                        t.constArray(pointerSort(), t.bv(IntProvenance, ObjBits))};
  return true;
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
