#include "memory/memory.h"
#include "memory/table.h"

#include "util/unsupported.h"
#include <llvm/ADT/DenseSet.h>
#include <llvm/ADT/MapVector.h>
#include <llvm/ADT/SetVector.h>
#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/IR/GlobalVariable.h>
#include <set>
#include <vector>

using namespace llvm;
using vereda::encode::Term;
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
  objects.push_back({t.bv(0, ptrBits), t.bv(0, ptrBits), t.boolean(true), 0, false, true});
  end = t.bv(1, ptrBits); // no object at address 0
}

SMTSortRef Memory::pointerSort() { return s.mkBVSort(ObjBits + ptrBits); }
Term Memory::ptr(Term obj, Term off) { return t.concat(obj, off); }
Term Memory::null() { return t.bv(0, ObjBits + ptrBits); }
Term Memory::object(Term p) {
  return t.extract(p, ObjBits + ptrBits - 1, ptrBits);
}
Term Memory::offset(Term p) { return t.extract(p, ptrBits - 1, 0); }
Term Memory::advance(Term p, Term delta) {
  return ptr(object(p), t.add(offset(p), delta));
}
Term Memory::mask(Term p, Term m) {
  if (const APInt *mv = t.constant(m); mv && mv->isAllOnes())
    return p;
  Term c, x, y;
  if (t.split(p, c, x, y))
    return t.ite(c, mask(x, m), mask(y, m));
  Term obj = object(p);
  if (const APInt *id = t.constant(obj); id && id->ult(objects.size())) {
    uint64_t o = id->getZExtValue();
    if (o == 0)
      return ptr(obj, t.bvand(offset(p), m));
    // When m only clears bits already zero in base(o), ((base + off) & m) - base == off & m.
    uint64_t a = objects[o].align;
    if (const APInt *mv = t.constant(m);
        mv && a > 1 && isPowerOf2_64(a) && (~*mv).ult(a))
      return ptr(obj, t.bvand(offset(p), m));
  }
  return ptr(obj, t.sub(t.bvand(address(p), m), base(obj)));
}
Term Memory::bytePtr(Term p, uint64_t i) {
  return i ? advance(p, t.bv(i, ptrBits)) : p;
}
void Memory::ensureLayout(uint64_t id) {
  if (id == 0 || id >= objects.size() || objects[id].laidOut)
    return;
  Object &o = objects[id];
  o.laidOut = true;
  Term layout = t.land(t.ugt(o.base, end), t.lnot(t.uaddo(o.base, o.size)));
  if (o.align > 1)
    layout = t.land(layout, t.eq(t.bvand(o.base, t.bv(o.align - 1, ptrBits)),
                                 t.bv(0, ptrBits)));
  t.addConstraint(layout);
  end = t.add(o.base, o.size);
  if (bases && o.exposed)
    t.addConstraint(t.eq(t.select(bases, t.bv(id, ObjBits)), o.base));
}

Term Memory::base(Term obj) {
  Term c, x, y;
  if (t.split(obj, c, x, y))
    return t.ite(c, base(x), base(y));
  if (const APInt *id = t.constant(obj); id && id->ult(objects.size())) {
    uint64_t idx = id->getZExtValue();
    ensureLayout(idx);
    return objects[idx].base;
  }
  if (!bases) {
    for (unsigned id = 1; id < objects.size(); ++id)
      if (objects[id].exposed)
        ensureLayout(id);
    bases = t.symbol("bases", s.mkArraySort(s.mkBVSort(ObjBits), s.mkBVSort(ptrBits)));
    for (unsigned id = 0; id < objects.size(); ++id)
      if (id == 0 || objects[id].exposed)
        t.addConstraint(t.eq(t.select(bases, t.bv(id, ObjBits)), objects[id].base));
  }
  return t.select(bases, obj);
}

Term Memory::size(Term obj) {
  Term c, x, y;
  if (t.split(obj, c, x, y))
    return t.ite(c, size(x), size(y));
  if (const APInt *id = t.constant(obj); id && id->ult(objects.size()))
    return objects[id->getZExtValue()].size;
  if (!sizes) {
    sizes = t.symbol("sizes", s.mkArraySort(s.mkBVSort(ObjBits), s.mkBVSort(ptrBits)));
    for (unsigned id = 0; id < objects.size(); ++id)
      t.addConstraint(t.eq(t.select(sizes, t.bv(id, ObjBits)), objects[id].size));
  }
  return t.select(sizes, obj);
}

Term Memory::fits(Term size) {
  // Several objects near PTRDIFF_MAX on one path cannot all be
  // laid out, which ends that path; a real allocator would return null.
  return t.ule(size, t.lshr(t.bvnot(t.bv(0, ptrBits)),
                                    t.bv(1, ptrBits)));
}

Term Memory::allocate(Term size, uint64_t align, Term exists, bool exposed) {
  unsigned id = objects.size();
  Term b = t.symbol("base#" + std::to_string(id), s.mkBVSort(ptrBits));
  Term e = t.add(b, size);
  // Ordered with a gap, aligned, not wrapping: objects never overlap, and a
  // one-past-the-end address belongs to its own object only. A constant-size
  // object that fits within a quarter of the address space alongside earlier
  // reserved objects gets its layout unconditionally so path guards stay out
  // of the global solver context; objects.back().exists still records exists
  // so fromAddress() never maps an address into an unallocated object.
  // Symbolic or huge allocations bind layout only where exists holds, or a
  // size on one path would remove inputs from every path.
  bool unconditional = t.isTrue(exists);
  bool defer = false;
  if (!t.isFalse(exists)) {
    using U = unsigned __int128;
    U limit = U(1) << (ptrBits - 2);
    if (const APInt *sz = t.constant(size); sz && sz->getActiveBits() <= 64) {
      U need = U(sz->getZExtValue()) + std::max<uint64_t>(align, 1);
      if (endBounded && U(reserved) + need <= limit) {
        reserved += static_cast<uint64_t>(need);
        unconditional = true;
        defer = true;
      } else {
        endBounded = false;
      }
    } else {
      endBounded = false;
    }
  }
  Term obj = t.bv(id, ObjBits);
  if (sizes)
    t.addConstraint(t.eq(t.select(sizes, obj), size));
  if (defer && (!bases || !exposed)) {
    // Layout constraints are asserted on demand by ensureLayout() when this
    // object's base address is first observed. Because all deferred objects
    // fit within a quarter of the address space, laying out any subset in any
    // order remains satisfiable and non-overlapping.
    objects.push_back({b, size, exists, align, exposed, false});
    return ptr(obj, t.bv(0, ptrBits));
  }
  Term layout = t.land(t.ugt(b, end), t.lnot(t.uaddo(b, size)));
  if (align > 1)
    layout = t.land(layout, t.eq(t.bvand(b, t.bv(align - 1, ptrBits)),
                                       t.bv(0, ptrBits)));
  t.addConstraint(unconditional ? layout : t.implies(exists, layout));
  if (bases && exposed)
    t.addConstraint(t.eq(t.select(bases, obj), b));
  objects.push_back({b, size, exists, align, exposed, true});
  end = unconditional ? e : t.ite(exists, e, end); // an object not allocated takes no space
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

Term Memory::global(const GlobalValue *g) const {
  auto it = globals.find(g);
  if (it == globals.end())
    throw Unsupported("address of " + g->getName().str());
  return it->second;
}

Term Memory::address(Term p) { return t.add(base(object(p)), offset(p)); }

Term Memory::isNullMemo(Term p, DenseMap<const void *, Term> &memo) {
  if (auto it = nullEq.find(p.id()); it != nullEq.end())
    return it->second.second;
  if (auto it = memo.find(p.id()); it != memo.end())
    return it->second;
  Term c, x, y;
  if (!t.choice(p, c, x, y))
    return {};
  Term lx = isNullMemo(x, memo), ly = isNullMemo(y, memo);
  Term r;
  if (lx || ly)
    r = t.ite(c, lx ? lx : t.eq(x, null()), ly ? ly : t.eq(y, null()));
  memo[p.id()] = r;
  return r;
}

Term Memory::isNull(Term p) {
  if (!nullEq.empty()) {
    DenseMap<const void *, Term> memo;
    if (Term r = isNullMemo(p, memo))
      return r;
  }
  return t.eq(p, null());
}

Term Memory::equal(Term p, Term q) {
  // Null is equal by both measures: no allocated object holds address 0, and
  // an outside pointer in object 0 has nonzero offset (address != 0).
  Term z = null();
  if (p == z)
    return isNull(q);
  if (q == z)
    return isNull(p);
  auto out = [&](Term x) {
    return t.land(t.eq(object(x), t.bv(0, ObjBits)), t.lnot(t.eq(offset(x), t.bv(0, ptrBits))));
  };
  Term outside = t.lor(out(p), out(q));
  if (t.isFalse(outside))
    return t.eq(p, q);
  return t.lor(t.land(outside, t.eq(address(p), address(q))),
               t.land(t.lnot(outside), t.eq(p, q)));
}

Term Memory::fromAddress(Term a) {
  unsigned budget = 32;
  return fromAddress(a, budget);
}

Term Memory::fromAddress(Term a, unsigned &budget) {
  // Address 0 is null: objects start above 0 (end starts at 1), so no
  // allocated object can hold 0.
  if (const APInt *v = t.constant(a); v && v->isZero())
    return null();
  Term c, left, right;
  if (budget && t.sumChoice(a, c, left, right)) {
    --budget;
    Term x = fromAddress(left, budget), y = fromAddress(right, budget);
    return t.ite(c, x, y);
  }
  // One comparison per exposed object created so far. The object and the
  // offset are built apart, so that object() of the result is a choice
  // among constants.
  Term obj = t.bv(0, ObjBits), off = a; // null, or outside the program
  for (unsigned id = 1; id < objects.size(); ++id) {
    if (!objects[id].exposed)
      continue;
    ensureLayout(id);
    const Object &o = objects[id];
    // One past the end still belongs to the object: the gap keeps it unique.
    // Only an object the path allocated holds an address: elsewhere its base
    // is free, and address 0 could land in it as a non-null pointer. Where
    // it exists its end does not wrap, so a is in it iff a - base <= size,
    // which folds for a base plus a bounded offset; where that is true
    // no other object holds a.
    Term x = t.sub(a, o.base);
    Term within = t.ule(x, o.size);
    if (known) {
      int d = t.decide(within, known);
      if (d >= 0)
        within = t.boolean(d);
    }
    Term inside = t.land(o.exists, within);
    if (t.isTrue(inside) || (context && t.entails(context, inside)))
      return ptr(t.bv(id, ObjBits), x);
    obj = t.ite(inside, t.bv(id, ObjBits), obj);
    off = t.ite(inside, x, off);
  }
  // What object() of the result folds to, which concat may have joined
  // with the offset's choices.
  Term r = ptr(obj, off);
  if (Term o = object(r); !t.constant(o))
    fromInteger[o.id()] = o;
  // No allocated object holds address 0, so fromAddress(a) == null() iff a == 0.
  if (!t.constant(r))
    nullEq[r.id()] = {r, t.eq(a, t.bv(0, ptrBits))};
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

std::optional<Memory::Key> Memory::key(Term p) const {
  const APInt *v = t.constant(p);
  if (!v)
    return std::nullopt;
  return Key{v->extractBitsAsZExtValue(ObjBits, ptrBits), v->extractBitsAsZExtValue(ptrBits, 0)};
}

Term Memory::ptr(const Key &k) {
  // One constant, not two and a concat: a flush builds one per byte.
  return t.bv((APInt(ObjBits + ptrBits, k.first) << ptrBits) | APInt(ObjBits + ptrBits, k.second));
}

void Memory::flushObject(State &st, uint64_t o) {
  unsigned shift = shifts.lookup(o);
  unsigned elemBytes = 1u << shift;
  bool byOffset = owned.count(o);
  const APInt *size = o > 0 && o < objects.size() ? t.constant(objects[o].size) : nullptr;
  // When every byte of an object's constant size is in st.known, the prior
  // array is never read: use its dominant value or an exact compact function
  // of the index instead of a store chain over the nondet initial array.
  if (size && !size->isZero() && size->ule(MaxKnown) &&
      (size->getZExtValue() & (elemBytes - 1)) == 0 &&
      (st.separate.find({o, 0}) || size->getZExtValue() >= MinSeparate)) {
    uint64_t n = size->getZExtValue();
    std::vector<State::Cell> bytes;
    bytes.reserve(n);
    uint64_t expected = 0;
    bool allKnown = true;
    st.known.forObject(o, [&](Key k, const State::Cell &c) {
      if (!allKnown || k.second != expected) {
        allKnown = false;
        return;
      }
      bytes.push_back(c);
      ++expected;
    });
    if (allKnown && expected == n) {
      uint64_t numElems = n >> shift;
      std::vector<Term> elems;
      elems.reserve(numElems);
      for (uint64_t e = 0; e < numElems; ++e) {
        if (elemBytes == 1) {
          elems.push_back(bytes[e].data);
          continue;
        }
        uint64_t val = 0;
        bool allConst = true;
        for (unsigned j = 0; j < elemBytes; ++j) {
          if (const APInt *c = t.constant(bytes[(e << shift) + j].data))
            val |= c->getZExtValue() << (8 * j);
          else
            allConst = false;
        }
        if (allConst) {
          elems.push_back(t.bv(APInt(8 * elemBytes, val)));
        } else {
          Term elem = bytes[e << shift].data;
          for (unsigned j = 1; j < elemBytes; ++j)
            elem = t.concat(bytes[(e << shift) + j].data, elem);
          elems.push_back(elem);
        }
      }
      struct Run {
        uint64_t lo, hi;
        Term val;
      };
      std::vector<Run> runs;
      llvm::DenseMap<const void *, std::pair<Term, uint64_t>> counts;
      Term dom = elems[0];
      uint64_t domCount = 0;
      for (uint64_t e = 0; e < numElems; ++e) {
        auto &entry = counts[elems[e].id()];
        if (!entry.first)
          entry.first = elems[e];
        if (++entry.second > domCount) {
          domCount = entry.second;
          dom = elems[e];
        }
        if (!runs.empty() && runs.back().val == elems[e])
          runs.back().hi = e + 1;
        else
          runs.push_back({e, e + 1, elems[e]});
      }
      SMTSortRef indexSort = byOffset ? s.mkBVSort(ptrBits - shift) : pointerSort();
      unsigned idxWidth = byOffset ? ptrBits - shift : ptrBits;
      std::vector<Run> otherRuns;
      uint64_t otherElems = numElems - domCount;
      for (const Run &r : runs)
        if (r.val != dom)
          otherRuns.push_back(r);
      Term data;
      constexpr size_t MaxLambdaRuns = 8;
      constexpr uint64_t MinLambdaElems = 8;
      encode::Terms::Def compact;
      if (byOffset && elemBytes <= 8 && numElems >= MinLambdaElems && domCount != numElems)
        compact = tableFunction(t, elems);
      if (compact) {
        // All elements have already been written: compress their exact values,
        // including modular wrap, without relying on a loop summary.
        data = t.lambda(
            s.mkArraySort(indexSort, s.mkBVSort(8u << shift)),
            "flush#" + std::to_string(fresh++),
            [this, compact = std::move(compact), dom, numElems](const Term &j) {
              return t.ite(t.ult(j, t.bv(APInt(j.width(), numElems))), compact(j), dom);
            });
      } else if (!otherRuns.empty() && otherRuns.size() <= MaxLambdaRuns &&
          otherElems >= MinLambdaElems && otherElems >= 2 * otherRuns.size()) {
        data = t.lambda(
            s.mkArraySort(indexSort, s.mkBVSort(8u << shift)),
            "flush#" + std::to_string(fresh++),
            [this, byOffset, idxWidth, dom, otherRuns = std::move(otherRuns)](const Term &j) {
              Term off = byOffset ? j : offset(j);
              Term r = dom;
              for (const Run &run : otherRuns) {
                Term in;
                if (run.lo + 1 == run.hi)
                  in = t.eq(off, t.bv(APInt(idxWidth, run.lo)));
                else if (run.lo == 0)
                  in = t.ult(off, t.bv(APInt(idxWidth, run.hi)));
                else
                  in = t.land(t.uge(off, t.bv(APInt(idxWidth, run.lo))),
                              t.ult(off, t.bv(APInt(idxWidth, run.hi))));
                r = t.ite(in, run.val, r);
              }
              return r;
            });
      } else {
        data = t.constArray(indexSort, dom);
        for (const Run &r : otherRuns)
          for (uint64_t e = r.lo; e < r.hi; ++e)
            data = t.store(data, byOffset ? t.bv(APInt(idxWidth, e)) : ptr({o, e << shift}), r.val);
      }
      Term prov = t.bv(IntProvenance, ObjBits);
      if (!shift) {
        Term domProv = bytes[0].prov;
        uint64_t domProvCount = 0;
        llvm::DenseMap<const void *, std::pair<Term, uint64_t>> provCounts;
        for (uint64_t i = 0; i < n; ++i) {
          auto &entry = provCounts[bytes[i].prov.id()];
          if (!entry.first)
            entry.first = bytes[i].prov;
          if (++entry.second > domProvCount) {
            domProvCount = entry.second;
            domProv = bytes[i].prov;
          }
        }
        prov = t.constArray(indexSort, domProv);
        for (uint64_t i = 0; i < n; ++i)
          if (bytes[i].prov != domProv)
            prov = t.store(prov, byOffset ? t.bv(APInt(idxWidth, i)) : ptr({o, i}), bytes[i].prov);
      }
      st.separate.set({o, 0}, {data, prov});
      return;
    }
  }
  std::optional<Key> last; // the element last moved
  st.known.forObject(o, [&](Key k, const State::Cell &c) {
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
    Term i = t.bv(APInt(ptrBits - shift, e.second >> shift));
    // Assemble constant elements directly so flush does not intern
    // intermediate 16/24/...-bit concat constants.
    uint64_t val = 0;
    bool allConst = true;
    const State::Cell *bytes[8];
    for (unsigned j = 0; j < elemBytes; ++j) {
      bytes[j] = st.known.find({e.first, e.second + j});
      const APInt *c = bytes[j] ? t.constant(bytes[j]->data) : nullptr;
      if (c)
        val |= c->getZExtValue() << (8 * j);
      else
        allConst = false;
    }
    Term elem;
    if (allConst) {
      elem = t.bv(APInt(8 * elemBytes, val));
    } else {
      Term old;
      for (unsigned j = 0; j < elemBytes; ++j) {
        if (!bytes[j] && !old)
          old = t.select(own.data, i);
        Term byte = bytes[j] ? bytes[j]->data : t.extract(old, 8 * j + 7, 8 * j);
        elem = j ? t.concat(byte, elem) : byte;
      }
    }
    st.separate.set({e.first, 0}, {t.store(own.data, i, elem), own.prov});
  });
}

void Memory::flush(State &st, std::optional<uint64_t> object) {
  if (object) {
    flushObject(st, *object);
    st.known.eraseObject(*object);
    return;
  }
  std::vector<uint64_t> objs;
  st.known.forEach([&](Key k, const State::Cell &) {
    if (objs.empty() || objs.back() != k.first)
      objs.push_back(k.first);
  });
  for (uint64_t o : objs)
    flushObject(st, o);
  st.known.clear();
}

Term Memory::index(uint64_t object, Term at) {
  return owned.count(object) ? t.extract(offset(at), ptrBits - 1, shifts.lookup(object)) : at;
}

void Memory::own(State &st, Term p, unsigned bits) {
  std::optional<Key> k = key(p);
  if (!k || k->first == 0)
    return;
  uint64_t o = k->first;
  unsigned shift = Log2_32(bits / 8);
  if (shift)
    shifts[o] = shift;
  SMTSortRef offsets = s.mkBVSort(ptrBits - shift);
  std::string n = std::to_string(o) + "#" + std::to_string(fresh);
  Term prov = t.bv(IntProvenance, ObjBits);
  State::Cell initial{t.symbol("object#" + n, s.mkArraySort(offsets, s.mkBVSort(bits))),
                      shift ? prov : t.constArray(offsets, prov)};
  owned[o] = initial;
  st.separate.set({o, 0}, initial);
}

void Memory::ownFields(State &st, Term p, uint64_t stride, const std::vector<Field> &fields) {
  uint64_t root = key(p)->first;
  unsigned shift = Log2_64(stride);
  Term count = t.lshr(objects[root].size, t.bv(shift, ptrBits));
  StructStorage storage{shift, {}};
  for (const Field &field : fields) {
    uint64_t id = objects.size();
    Term size = t.mul(count, t.bv(field.bytes, ptrBits));
    // These are storage partitions, never allocations or program pointers.
    objects.push_back({t.bv(0, ptrBits), size, t.boolean(false), 1, false, true});
    if (sizes)
      t.addConstraint(t.eq(t.select(sizes, t.bv(id, ObjBits)), size));
    Term at = ptr(t.bv(id, ObjBits), t.bv(0, ptrBits));
    own(st, at, field.pointer ? 8 : 8 * field.bytes);
    storage.fields.push_back({field, id});
    fieldOwners[id] = root;
  }
  fieldStorage[root] = std::move(storage);
}

Term Memory::fieldPointer(Term p, unsigned bytes) {
  const APInt *id = t.constant(object(p));
  if (!id)
    return p;
  auto it = fieldStorage.find(id->getZExtValue());
  if (it == fieldStorage.end())
    return p;
  Term off = offset(p);
  for (const FieldStorage &storage : it->second.fields) {
    const Field &f = storage.field;
    for (unsigned delta = 0; delta + bytes <= f.bytes; ++delta) {
      Term relative = t.sub(off, t.bv(f.offset + delta, ptrBits));
      const APInt *low = t.constant(t.extract(relative, it->second.shift - 1, 0));
      if (!low || !low->isZero())
        continue;
      Term i = t.lshr(relative, t.bv(it->second.shift, ptrBits));
      Term mapped = t.add(t.mul(i, t.bv(f.bytes, ptrBits)), t.bv(delta, ptrBits));
      return ptr(t.bv(storage.object, ObjBits), mapped);
    }
  }
  return p;
}

// The bits of byte at within its element: (offset mod bytes) * 8, n bits wide.
static Term byteShift(encode::Terms &t, Term off, unsigned shift, unsigned n) {
  return t.shl(t.zext(t.extract(off, shift - 1, 0), n - shift), t.bv(3, n));
}

State::Cell Memory::readOwn(uint64_t object, const State::Cell &own, Term at,
                            bool withProvenance) {
  Term i = index(object, at);
  unsigned shift = shifts.lookup(object);
  if (!shift)
    return {t.select(own.data, i), withProvenance ? t.select(own.prov, i) : Term()};
  Term elem = t.select(own.data, i);
  if (const APInt *j = t.constant(t.extract(offset(at), shift - 1, 0))) {
    unsigned lo = 8 * j->getZExtValue();
    return {t.extract(elem, lo + 7, lo), own.prov};
  }
  unsigned n = elem.width();
  return {t.extract(t.lshr(elem, byteShift(t, offset(at), shift, n)), 7, 0), own.prov};
}

State::Cell Memory::storeOwn(uint64_t object, const State::Cell &own, Term at,
                             const State::Cell &c) {
  Term i = index(object, at);
  unsigned shift = shifts.lookup(object);
  if (!shift)
    return {t.store(own.data, i, c.data), t.store(own.prov, i, c.prov)};
  // Read, mask and write the element back. The byte's provenance goes: a
  // pointer stored here through an unknown pointer reads back as one made
  // from an integer.
  Term elem = t.select(own.data, i);
  unsigned n = elem.width();
  Term sh = byteShift(t, offset(at), shift, n);
  Term kept = t.bvand(elem, t.bvnot(t.shl(t.bv(0xff, n), sh)));
  return {t.store(own.data, i, t.bvor(kept, t.shl(t.zext(c.data, n - 8), sh))), own.prov};
}

void Memory::summarise(State &st, Term first, uint64_t stride, Type *ty, Term count,
                       Iteration iteration) {
  Term mapped = fieldPointer(first, storeBits(dl, ty) / 8);
  if (mapped != first) {
    uint64_t root = t.constant(object(first))->getZExtValue();
    uint64_t child = t.constant(object(mapped))->getZExtValue();
    const StructStorage &storage = fieldStorage.find(root)->second;
    for (const FieldStorage &field : storage.fields)
      if (field.object == child) {
        assert((stride & ((uint64_t(1) << storage.shift) - 1)) == 0);
        stride = (stride >> storage.shift) * field.field.bytes;
        break;
      }
    first = mapped;
  }
  uint64_t o = t.constant(object(first))->getZExtValue();
  flush(st, o);
  const State::Cell *own = st.separate.find({o, 0});
  State::Cell below = own ? *own : State::Cell{st.data, st.prov};
  bool byOffset = own && owned.count(o);
  // An object of elements (#118) is read and written only as whole values
  // of ty, at multiples of their size: an element is a value or none.
  unsigned shift = byOffset ? shifts.lookup(o) : 0, bits = storeBits(dl, ty);
  assert((!shift || bits == 8u << shift) && "the loop's store is an access of an element");
  Term start = offset(first), objectId = t.bv(o, ObjBits), intProv = t.bv(IntProvenance, ObjBits);
  auto offsetOf = [=, this](const Term &j) {
    return !byOffset ? offset(j) : shift ? t.concat(j, t.bv(0, shift)) : j;
  };
  // Whether the loop wrote none of the elements at the offsets j may take,
  // as Terms::range gives them: below the first, past the last, or between
  // two, the same distance from an element each time.
  const APInt *from = t.constant(start), *iterations = t.constant(count);
  auto misses = [=, this](const Term &j) {
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
  auto at = [=, this](const Term &j) -> std::pair<Term, Term> {
    Term in = byOffset ? t.boolean(true) : t.eq(object(j), objectId);
    if (t.isFalse(in))
      return {in, {}};
    Term off = offsetOf(j);
    Term d = t.sub(off, start), n, b;
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
    else if (v.width() < bits)
      v = t.zext(v, bits - v.width());
    if (shift)
      return {in, v};
    Term r = t.extract(v, 7, 0);
    for (unsigned i = 1; i < bits / 8; ++i)
      r = t.ite(t.eq(b, t.bv(i, ptrBits)), t.extract(v, 8 * i + 7, 8 * i), r);
    return {in, r};
  };
  SMTSortRef index = !byOffset ? pointerSort() : s.mkBVSort(ptrBits - shift);
  std::string n = std::to_string(fresh++);
  State::Cell after{t.lambda(s.mkArraySort(index, s.mkBVSort(8u << shift)), "summary#" + n,
                             [=, this](const Term &j) {
                               auto [in, v] = at(j);
                               Term old = t.select(below.data, j);
                               return t.isFalse(in) ? old : t.ite(in, v, old);
                             },
                             below.data, misses),
                    below.prov};
  // An object of elements has no provenance array: it holds no pointer.
  if (!shift) {
    after.prov = t.lambda(s.mkArraySort(index, s.mkBVSort(ObjBits)), "summary_provenance#" + n,
                          [=, this](const Term &j) {
                            Term old = t.select(below.prov, j);
                            return old == intProv ? old : t.ite(at(j).first, intProv, old);
                          },
                          below.prov, misses);
    provBelow[after.prov.id()] = {after.prov, {{below.prov, std::nullopt, byOffset}}}; // the pointers it may hold
  }
  if (own)
    st.separate.set({o, 0}, after);
  else
    st.data = after.data, st.prov = after.prov;
}

void Memory::storeArrays(State &st, uint64_t object, Term at, const State::Cell &c) {
  if (const State::Cell *own = st.separate.find({object, 0})) {
    st.separate.set({object, 0}, storeOwn(object, *own, at, c));
    return;
  }
  st.data = t.store(st.data, at, c.data);
  st.prov = t.store(st.prov, at, c.prov);
}

State::Cell Memory::readArrays(const State &st, Term at, bool withProvenance) {
  Term mapped = fieldPointer(at);
  if (mapped != at)
    return readByte(st, mapped, withProvenance);
  Term objTerm = object(at);
  if (const APInt *obj = t.constant(objTerm)) {
    if (const State::Cell *own = st.separate.find({obj->getZExtValue(), 0}))
      return readOwn(obj->getZExtValue(), *own, at, withProvenance);
    return {t.select(st.data, at), withProvenance ? t.select(st.prov, at) : Term()};
  }
  std::vector<uint64_t> objs = candidates(at);
  bool allSeparate = !objs.empty();
  llvm::SmallVector<std::pair<uint64_t, const State::Cell *>, 4> sep;
  for (uint64_t o : objs) {
    if (const State::Cell *own = st.separate.find({o, 0}))
      sep.push_back({o, own});
    else
      allSeparate = false;
  }
  State::Cell r;
  if (!allSeparate)
    r = {t.select(st.data, at), withProvenance ? t.select(st.prov, at) : Term()};
  for (const auto &[o, own] : sep) {
    Term is = t.eq(objTerm, t.bv(o, ObjBits));
    if (t.isFalse(is))
      continue;
    State::Cell b = readOwn(o, *own, at, withProvenance);
    if (t.isTrue(is))
      return b;
    if (!r.data)
      r = b;
    else
      r = {t.ite(is, b.data, r.data), withProvenance ? t.ite(is, b.prov, r.prov) : Term()};
  }
  if (!r.data)
    r = {t.select(st.data, at), withProvenance ? t.select(st.prov, at) : Term()};
  return r;
}

std::optional<uint64_t> Memory::element(Term p, Type *ty) {
  Term c, x, y;
  const APInt *obj = t.constant(object(p));
  if (!obj || key(p) || t.split(p, c, x, y))
    return std::nullopt;
  auto it = shifts.find(obj->getZExtValue());
  if (it == shifts.end() || !ty->isIntegerTy(8u << it->second))
    return std::nullopt;
  return obj->getZExtValue();
}

CellMap Memory::mergeSeparate(Term c, const State &a, const State &b) {
  if (a.separate.same(b.separate))
    return a.separate;
  // An object separate on one side only reads the other side's shared
  // arrays, which hold its bytes there under the same index; one given
  // arrays by own() on a path that has not created it yet, its initial ones.
  auto fallback = [this](const State &st, uint64_t o) {
    auto it = owned.find(o);
    return it != owned.end() ? it->second : State::Cell{st.data, st.prov};
  };
  CellMap r = a.separate;
  CellMap::diff(a.separate, b.separate,
                [&](Key k, const State::Cell *x, const State::Cell *y) {
                  State::Cell xa = x ? *x : fallback(a, k.first);
                  State::Cell yb = y ? *y : fallback(b, k.first);
                  r.set(k, {t.ite(c, xa.data, yb.data), t.ite(c, xa.prov, yb.prov)});
                });
  return r;
}

void Memory::prepare(State &st, Term p) {
  Term c, x, y;
  if (key(p) || t.split(p, c, x, y))
    return;
  for (uint64_t o : candidates(p))
    flush(st, o);
}

std::vector<uint64_t> Memory::candidates(Term p, bool integers, bool *read) {
  std::vector<uint64_t> r;
  std::vector<Term> work{p};
  llvm::SmallPtrSet<const void *, 16> seen;
  bool fromMemory = false;
  while (!work.empty()) {
    Term e = work.back(), c, x, y;
    work.pop_back();
    // A choice of pointers whose sides are not constant keeps its object
    // inside, out of reach of object(): split the pointer first.
    if (e.width() != ObjBits && !t.choice(e, c, x, y))
      e = object(e);
    if (!integers && fromInteger.count(e.id()))
      continue;
    if (const APInt *id = t.constant(e)) {
      r.push_back(id->getZExtValue());
    } else if (t.choice(e, c, x, y)) {
      if (seen.insert(e.id()).second) {
        work.push_back(x);
        work.push_back(y);
      }
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

int Memory::side(const Term &c) {
  if (!context)
    return -1;
  if (t.entails(context, c))
    return 1;
  return t.entails(context, t.lnot(c)) ? 0 : -1;
}

State::Cell Memory::readByte(const State &st, Term at, bool withProvenance) {
  at = fieldPointer(at);
  Term c, x, y;
  if (t.split(at, c, x, y)) { // one of two constant pointers
    if (int pick = side(c); pick >= 0)
      return readByte(st, pick ? x : y, withProvenance);
    State::Cell a = readByte(st, x, withProvenance), b = readByte(st, y, withProvenance);
    return {t.ite(c, a.data, b.data), withProvenance ? t.ite(c, a.prov, b.prov) : Term()};
  }
  if (std::optional<Key> k = key(at))
    if (const State::Cell *c = st.known.find(*k))
      return *c;
  return readArrays(st, at, withProvenance);
}

void Memory::writeByte(State &st, Term at, const State::Cell &c) {
  at = fieldPointer(at);
  Term cond, x, y;
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
    Term objTerm = object(at);
    if (const APInt *obj = t.constant(objTerm)) {
      storeArrays(st, obj->getZExtValue(), at, c);
      return;
    }
    bool shared = false;
    for (uint64_t o : candidates(at)) {
      const State::Cell *own = st.separate.find({o, 0});
      if (!own) {
        shared = true;
        continue;
      }
      Term is = t.eq(objTerm, t.bv(o, ObjBits));
      if (t.isFalse(is))
        continue;
      State::Cell stored = storeOwn(o, *own, at, c);
      st.separate.set({o, 0}, {t.ite(is, stored.data, own->data),
                               t.ite(is, stored.prov, own->prov)});
    }
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

State Memory::ite(Term c, const State &a0, const State &b0) {
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
    if (xa.data != yb.data && (t.arithmetic(xa.data) || t.arithmetic(yb.data)))
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
void Memory::storeBytes(State &st, Term p, Term bits, Term prov) {
  unsigned n = bits.width() / 8;
  if (std::optional<Key> k = key(p)) {
    // A constant address: the bytes' keys are integers, so no address term
    // is built until a flush needs one. Offsets wrap at the pointer width.
    uint64_t mask = ptrBits >= 64 ? ~uint64_t(0) : (uint64_t(1) << ptrBits) - 1;
    for (unsigned i = 0; i < n; ++i)
      st.known.set(*key(fieldPointer(ptr({k->first, (k->second + i) & mask}))),
                   {t.extract(bits, 8 * i + 7, 8 * i), prov});
    if (st.known.size() > MaxKnown)
      flush(st);
    return;
  }
  prepare(st, p);
  for (unsigned i = 0; i < n; ++i)
    writeByte(st, bytePtr(p, i), {t.extract(bits, 8 * i + 7, 8 * i), prov});
}

void Memory::store(State &st, Term p, Term v, Type *ty) {
  Term mapped = fieldPointer(p, storeBits(dl, ty) / 8);
  if (mapped == p && !initialising)
    if (const APInt *id = t.constant(object(p)); id && fieldStorage.count(id->getZExtValue()))
      throw Unsupported("field array store with an unresolved offset");
  p = mapped;
  unsigned bits = storeBits(dl, ty);
  if (ty->isPointerTy()) {
    // A pointer with no object but an offset, outside the program, was
    // made from an integer, so it is stored as one and a load converts it
    // again (#75). Null keeps object 0 and is never converted. The test is
    // on the offset, which, unlike the address, folds on each side of
    // ite(ok, p, null).
    Term obj = object(v);
    Term made = t.land(t.eq(obj, t.bv(0, ObjBits)), t.lnot(t.eq(offset(v), t.bv(0, ptrBits))));
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
  else if (v.width() < bits)
    v = t.zext(v, bits - v.width());
  storeBytes(st, p, v, t.bv(IntProvenance, ObjBits));
}

Term Memory::load(State &st, Term p, Type *ty) {
  if (!ty->isIntegerTy() && !ty->isPointerTy() && !ty->isFloatingPointTy())
    throw Unsupported("load of a non-integer value");
  Term mapped = fieldPointer(p, storeBits(dl, ty) / 8);
  if (mapped == p)
    if (const APInt *id = t.constant(object(p)); id && fieldStorage.count(id->getZExtValue()))
      throw Unsupported("field array access with an unresolved offset");
  p = mapped;
  unsigned n = storeBits(dl, ty) / 8;
  uint64_t mask = ptrBits >= 64 ? ~uint64_t(0) : (uint64_t(1) << ptrBits) - 1;
  auto finishLoad = [&](const State::Cell &first, Term bits) -> Term {
    if (ty->isPointerTy()) {
      // The provenance of the first byte stands for the pointer.
      Term obj = first.prov;
      // Bytes of an integer, among them a pointer made from one (see store),
      // which an object allocated since may now cover: the address is
      // converted now, as a fresh conversion would be (#75).
      Term isInt = t.eq(obj, t.bv(IntProvenance, ObjBits));
      if (t.isTrue(isInt))
        return fromAddress(bits);
      Term r = t.isFalse(isInt)
                   ? ptr(obj, t.sub(bits, base(obj)))
                   : t.ite(isInt, fromAddress(bits), ptr(obj, t.sub(bits, base(obj))));
      // r == null() iff bits == 0 and (isInt || obj == 0): avoids both
      // fromAddress(bits) and select(bases, obj), while still distinguishing
      // a wild out-of-bounds pointer (obj != 0, bits == 0) from null.
      if (!t.constant(r) && !nullEq.count(r.id()))
        nullEq[r.id()] = {r, t.land(t.eq(bits, t.bv(0, ptrBits)),
                                    t.lor(isInt, t.eq(obj, t.bv(0, ObjBits))))};
      return r;
    }
    if (ty->isIntegerTy(1))
      return t.eq(t.extract(bits, 0, 0), t.bv(1, 1));
    unsigned width = ty->getPrimitiveSizeInBits();
    return width < bits.width() ? t.extract(bits, width - 1, 0) : bits;
  };
  // Fold loads from a known object whose symbolic offset is bounded to a small
  // finite domain of known bytes (before prepare() flushes st.known): when all
  // candidates hold the same value (e.g. a pointer table pointing to one
  // object), return it directly; for a small table of distinct pointers, keep
  // their known object IDs via an ite chain instead of flattening into memory.
  if (!key(p)) {
    if (const APInt *id = t.constant(object(p))) {
      Term off = offset(p);
      encode::Terms::Known bounds = known ? encode::Terms::Known(known) : encode::Terms::Known{};
      if (std::optional<encode::Terms::Range> r = t.range(off, bounds)) {
        constexpr uint64_t MaxFoldDomain = 16;
        constexpr uint64_t MaxPtrChoices = 8;
        uint64_t count = r->lo == r->hi ? 1 : (r->hi - r->lo) / r->stride + 1;
        if (count >= 1 && count <= MaxFoldDomain) {
          uint64_t objId = id->getZExtValue();
          bool allKnown = true;
          for (uint64_t i = 0; i < count && allKnown; ++i) {
            uint64_t pos = r->lo + i * r->stride;
            for (unsigned b = 0; b < n; ++b)
              if (!st.known.find({objId, (pos + b) & mask})) {
                allKnown = false;
                break;
              }
          }
          if (allKnown) {
            SmallVector<std::pair<State::Cell, Term>, 16> raw;
            raw.reserve(count);
            bool allSame = true;
            for (uint64_t i = 0; i < count; ++i) {
              uint64_t pos = r->lo + i * r->stride;
              State::Cell first = *st.known.find({objId, pos & mask});
              Term bits = first.data;
              for (unsigned b = 1; b < n; ++b)
                bits = t.concat(st.known.find({objId, (pos + b) & mask})->data, bits);
              if (i > 0 && (bits != raw[0].second ||
                            (ty->isPointerTy() && first.prov != raw[0].first.prov)))
                allSame = false;
              raw.emplace_back(first, bits);
            }
            if (allSame)
              return finishLoad(raw[0].first, raw[0].second);
            if (ty->isPointerTy() && count <= MaxPtrChoices) {
              Term res = finishLoad(raw.back().first, raw.back().second);
              for (uint64_t i = count - 1; i > 0; --i)
                res = t.ite(t.eq(off, t.bv(r->lo + (i - 1) * r->stride, ptrBits)),
                            finishLoad(raw[i - 1].first, raw[i - 1].second), res);
              return res;
            }
          }
        }
      }
    }
  }
  prepare(st, p);
  if (std::optional<uint64_t> o = element(p, ty))
    return t.select(st.separate.find({*o, 0})->data, index(*o, p));
  State::Cell first;
  Term bits;
  if (std::optional<Key> k = key(p)) {
    auto readAt = [&](uint64_t i) -> State::Cell {
      Key ki{k->first, (k->second + i) & mask};
      if (const State::Cell *c = st.known.find(ki))
        return *c;
      return readArrays(st, ptr(ki), ty->isPointerTy() && i == 0);
    };
    first = readAt(0);
    bits = first.data;
    for (unsigned i = 1; i < n; ++i)
      bits = t.concat(readAt(i).data, bits);
  } else {
    first = readByte(st, p, ty->isPointerTy());
    bits = first.data;
    for (unsigned i = 1; i < n; ++i)
      bits = t.concat(readByte(st, bytePtr(p, i), false).data, bits);
  }
  return finishLoad(first, bits);
}

// One array store per byte, so a large zeroed global or memset is
// a long store chain. A range store or a constant base would fix it.
bool Memory::fillObject(State &st, Term p, Term byte, uint64_t n) {
  Term c, x, y;
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

void Memory::fill(State &st, Term p, Term byte) {
  fill(st, key(p)->first, byte);
}

void Memory::fill(State &st, uint64_t o, Term byte) {
  if (auto it = fieldStorage.find(o); it != fieldStorage.end())
    for (const FieldStorage &field : it->second.fields)
      fill(st, field.object, byte);
  st.known.eraseObject(o); // every byte is overwritten, so its known bytes are stale
  unsigned shift = shifts.lookup(o);
  SMTSortRef index = owned.count(o) ? s.mkBVSort(ptrBits - shift) : pointerSort();
  Term elem = byte, prov = t.bv(IntProvenance, ObjBits);
  for (unsigned j = 1; j < 1u << shift; ++j)
    elem = t.concat(byte, elem);
  st.separate.set({o, 0}, {t.constArray(index, elem), shift ? prov : t.constArray(index, prov)});
}

bool Memory::applyRange(State &st, uint64_t o, Term p, uint64_t n, const char *name,
                        const std::function<Term(const Term &j, const Term &d)> &valueAt) {
  using U = unsigned __int128;
  if (n < MinSeparate || o == 0 || o >= objects.size())
    return false;
  if (ptrBits < 64 && n >= (U(1) << ptrBits))
    return false;
  std::optional<Key> k = key(p);
  if (k && U(k->second) + n > (U(1) << ptrBits))
    return false;
  unsigned shift = shifts.lookup(o);
  if (shift && (!k || (k->second & ((1u << shift) - 1)) != 0 || (n & ((1u << shift) - 1)) != 0))
    return false;
  if (k)
    st.known.eraseRange(o, k->second, k->second + n - 1);
  else
    flush(st, o);
  const State::Cell *own = st.separate.find({o, 0});
  State::Cell below = own ? *own : State::Cell{st.data, st.prov};
  bool byOffset = own && owned.count(o);
  SMTSortRef index = !byOffset ? pointerSort() : s.mkBVSort(ptrBits - shift);
  SMTSortRef dataSort = s.mkArraySort(index, s.mkBVSort(8u << shift));
  Term objectId = t.bv(o, ObjBits), intProv = t.bv(IntProvenance, ObjBits);
  std::function<bool(const Term &)> misses;
  std::function<std::pair<Term, Term>(const Term &)> inRange;
  if (shift) {
    unsigned idxBits = ptrBits - shift;
    uint64_t eLo = k->second >> shift, eCount = n >> shift;
    Term eStart = t.bv(eLo, idxBits), eLen = t.bv(eCount, idxBits);
    misses = [this, eLo, eCount](const Term &j) {
      std::optional<encode::Terms::Range> v = t.range(j);
      return v && (v->hi < eLo || v->lo >= eLo + eCount);
    };
    inRange = [this, eStart, eLen](const Term &j) -> std::pair<Term, Term> {
      Term d = t.sub(j, eStart);
      return {t.ult(d, eLen), d};
    };
  } else {
    Term start = offset(p), len = t.bv(n, ptrBits);
    std::optional<uint64_t> from = k ? std::optional<uint64_t>(k->second) : std::nullopt;
    misses = [this, byOffset, objectId, from, n](const Term &j) {
      if (!byOffset && t.isFalse(t.eq(object(j), objectId)))
        return true;
      if (!from)
        return false;
      Term off = byOffset ? j : offset(j);
      std::optional<encode::Terms::Range> v = t.range(off);
      return v && (v->hi < *from || v->lo >= U(*from) + U(n));
    };
    inRange = [this, byOffset, objectId, start, len](const Term &j) -> std::pair<Term, Term> {
      Term inObj = byOffset ? t.boolean(true) : t.eq(object(j), objectId);
      if (t.isFalse(inObj))
        return {inObj, {}};
      Term off = byOffset ? j : offset(j);
      Term d = t.sub(off, start);
      return {t.land(inObj, t.ult(d, len)), d};
    };
  }
  std::string id = std::to_string(fresh++);
  State::Cell after{
      t.lambda(dataSort, std::string(name) + "#" + id,
               [this, inRange, valueAt, below](const Term &j) {
                 auto [in, d] = inRange(j);
                 if (t.isFalse(in))
                   return t.select(below.data, j);
                 Term v = valueAt(j, d);
                 if (t.isTrue(in))
                   return v;
                 return t.ite(in, v, t.select(below.data, j));
               },
               below.data, misses),
      below.prov};
  if (!shift && t.filledWith(below.prov) != intProv) {
    after.prov = t.lambda(s.mkArraySort(index, s.mkBVSort(ObjBits)),
                          std::string(name) + "_provenance#" + id,
                          [this, inRange, intProv, below](const Term &j) {
                            auto [in, d] = inRange(j);
                            if (t.isTrue(in))
                              return intProv;
                            Term old = t.select(below.prov, j);
                            return t.isFalse(in) || old == intProv ? old
                                                                   : t.ite(in, intProv, old);
                          },
                          below.prov, misses);
    provBelow[after.prov.id()] = {after.prov, {{below.prov, std::nullopt, byOffset}}};
  }
  if (own)
    st.separate.set({o, 0}, after);
  else
    st.data = after.data, st.prov = after.prov;
  return true;
}

bool Memory::fillRange(State &st, Term p, Term byte, uint64_t n) {
  Term c, x, y;
  if (t.split(p, c, x, y)) {
    int pick = side(c);
    if (pick >= 0)
      p = pick ? x : y;
  }
  const APInt *obj = t.constant(object(p));
  if (n < MinSeparate || !obj)
    return false;
  uint64_t o = obj->getZExtValue();
  if (o == 0 || o >= objects.size())
    return false;
  unsigned shift = shifts.lookup(o);
  Term elem = byte;
  for (unsigned j = 1; j < 1u << shift; ++j)
    elem = t.concat(byte, elem);
  return applyRange(st, o, p, n, "fill",
                    [elem](const Term &, const Term &) { return elem; });
}

bool Memory::fillElementRange(State &st, Term p, Term elem, uint64_t stride, uint64_t count) {
  unsigned elemBytes = elem.width() / 8;
  uint64_t total = count * stride;
  if (total < MinSeparate || elemBytes != stride)
    return false;
  if (const APInt *cv = t.constant(elem)) {
    uint64_t b0 = cv->extractBitsAsZExtValue(8, 0);
    bool uniform = true;
    for (unsigned i = 1; i < elemBytes; ++i)
      if (cv->extractBitsAsZExtValue(8, 8 * i) != b0) {
        uniform = false;
        break;
      }
    if (uniform) {
      fillOrSet(st, p, t.bv(b0, 8), total);
      return true;
    }
  }
  std::optional<Key> k = key(p);
  if (!k || k->first == 0 || k->first >= objects.size())
    return false;
  uint64_t o = k->first;
  unsigned shift = shifts.lookup(o);
  if (shift) {
    if (elemBytes != (1u << shift) || (k->second & (elemBytes - 1)) != 0)
      return false;
    const APInt *size = t.constant(objects[o].size);
    if (k->second == 0 && size && *size == total) {
      st.known.eraseObject(o);
      SMTSortRef index = s.mkBVSort(ptrBits - shift);
      st.separate.set({o, 0}, {t.constArray(index, elem), t.bv(IntProvenance, ObjBits)});
      return true;
    }
    return applyRange(st, o, p, total, "fill",
                      [elem](const Term &, const Term &) { return elem; });
  }
  return applyRange(st, o, p, total, "fill",
                    [this, elem, elemBytes](const Term &, const Term &d) {
                      Term b = isPowerOf2_64(elemBytes)
                                   ? t.bvand(d, t.bv(elemBytes - 1, ptrBits))
                                   : t.urem(d, t.bv(elemBytes, ptrBits));
                      Term r = t.extract(elem, 8 * elemBytes - 1, 8 * (elemBytes - 1));
                      for (unsigned i = elemBytes - 1; i-- > 0;)
                        r = t.ite(t.eq(b, t.bv(i, ptrBits)),
                                  t.extract(elem, 8 * i + 7, 8 * i), r);
                      return r;
                    });
}

bool Memory::havocRange(State &st, uint64_t o, Term p, uint64_t n) {
  using U = unsigned __int128;
  if (n < MinSeparate || o == 0 || o >= objects.size())
    return false;
  if (ptrBits < 64 && n >= (U(1) << ptrBits))
    return false;
  std::optional<Key> k = key(p);
  if (k && U(k->second) + n > (U(1) << ptrBits))
    return false;
  unsigned shift = shifts.lookup(o);
  if (shift && (!k || (k->second & ((1u << shift) - 1)) != 0 || (n & ((1u << shift) - 1)) != 0))
    return false;
  bool byOffset = st.separate.find({o, 0}) && owned.count(o);
  SMTSortRef index = !byOffset ? pointerSort() : s.mkBVSort(ptrBits - shift);
  Term freshArr = t.symbol("havoc#" + std::to_string(fresh++),
                           s.mkArraySort(index, s.mkBVSort(8u << shift)));
  return applyRange(st, o, p, n, "havoc",
                    [this, freshArr](const Term &j, const Term &) {
                      return t.select(freshArr, j);
                    });
}

void Memory::havoc(State &st, Term p, std::optional<uint64_t> n) {
  havocWith(st, p, n, t.bv(IntProvenance, ObjBits), true);
}

std::vector<std::pair<const GlobalVariable *, Term>> Memory::readOnly(Term p) {
  std::vector<std::pair<const GlobalVariable *, Term>> r;
  for (uint64_t o : candidates(p, false))
    if (o < objects.size())
      if (auto *g = dyn_cast_or_null<GlobalVariable>(objects[o].constant))
        r.push_back({g, into(p, o)});
  return r;
}

Term Memory::into(Term obj, uint64_t o) {
  Term c, x, y;
  // As candidates() walks it.
  if (obj.width() != ObjBits && !t.choice(obj, c, x, y))
    obj = object(obj);
  if (fromInteger.count(obj.id()))
    return t.boolean(false);
  if (t.choice(obj, c, x, y))
    return t.ite(c, into(x, o), into(y, o));
  return t.eq(obj, t.bv(o, ObjBits));
}

void Memory::havocCall(State &st, llvm::ArrayRef<Term> written,
                       llvm::ArrayRef<Term> read, bool reach) {
  std::vector<Term> ps(written.begin(), written.end());
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
  Term prov = t.bv(IntProvenance, ObjBits);
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
      Term in = t.boolean(false);
      for (uint64_t r : rs)
        in = t.lor(in, t.eq(object(ps[i]), t.bv(r, ObjBits)));
      st = ite(in, a, st);
    }
  }
}

void Memory::havocWith(State &st, Term p, std::optional<uint64_t> n, Term prov,
                       bool constants) {
  Term c, x, y;
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
    if (havocRange(st, o, p, *n))
      return;
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

void Memory::havocObject(State &st, uint64_t o, Term prov) {
  if (auto it = fieldStorage.find(o); it != fieldStorage.end())
    for (const FieldStorage &field : it->second.fields)
      havocObject(st, field.object, prov);
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
  Term intProv = t.bv(IntProvenance, ObjBits);
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
  llvm::DenseMap<const void *, std::pair<std::vector<uint64_t>, bool>> targets;
  auto add = [&](llvm::ArrayRef<uint64_t> in, const Term &prov) {
    auto [it, inserted] = targets.try_emplace(prov.id());
    auto &[xs, read] = it->second;
    if (inserted)
      for (uint64_t x : candidates(prov, false, &read))
        if (x && x < objects.size()) // not null, nor an integer's
          xs.push_back(x);
    for (uint64_t o : in) {
      if (auto owner = fieldOwners.find(o); owner != fieldOwners.end())
        o = owner->second;
      if (!xs.empty())
        to[o].insert(xs.begin(), xs.end());
      if (read && !exposed.empty() && holdsRead.insert(o).second)
        to[o].insert(exposed.begin(), exposed.end());
    }
  };
  // A pointer's bytes share one provenance: add each once per object.
  llvm::DenseSet<std::pair<uint64_t, const void *>> added;
  st.known.forEach([&](Key k, const State::Cell &c) {
    if (c.prov != intProv && added.insert({k.first, c.prov.id()}).second)
      add(k.first, c.prov);
  });
  // The provenance stored in the arrays: in those indexed by pointer, each
  // store for the objects its index may be in, which may count a store an
  // object's own arrays have since replaced; in an object's own indexed by
  // offset, each for that object. A constant array is an object's own, all
  // for it. An object's own arrays may choose the shared ones on one side
  // of a join, which the first walk has seen already.
  llvm::DenseSet<const void *> shared;
  std::function<void(const Term &, std::optional<uint64_t>, std::optional<uint64_t>, bool)> walk =
      [&](const Term &arrays, std::optional<uint64_t> source, std::optional<uint64_t> dest,
          bool byOffset) {
        llvm::DenseSet<const void *> local, &seen = dest ? local : shared;
        std::vector<Term> work{arrays};
        while (!work.empty()) {
          Term e = work.back(), a, i, v, c;
          work.pop_back();
          if (!seen.insert(e.id()).second || (dest && source == dest && shared.count(e.id())))
            continue;
          if (Term f = t.filledWith(e)) {
            if (f == intProv)
              continue;
            if (!dest)
              throw Unsupported("memory whose provenance is not known");
            add(*dest, f);
            continue;
          }
          if (t.stored(e, a, i, v)) {
            if (v != intProv) {
              if (byOffset) {
                add(*dest, v);
              } else if (source && dest) {
                std::vector<uint64_t> cs = candidates(i);
                if (llvm::is_contained(cs, *source))
                  add(*dest, v);
              } else {
                add(candidates(i), v);
              }
            }
            work.push_back(a);
          } else if (t.choice(e, c, a, v)) {
            work.push_back(a);
            work.push_back(v);
          } else if (auto it = provBelow.find(e.id()); it != provBelow.end()) {
            for (const ProvEdge &edge : it->second.second) {
              if (!edge.fromObject)
                work.push_back(edge.array);
              else
                walk(edge.array, edge.fromObject, dest, edge.byOffset);
            }
          } else {
            throw Unsupported("memory whose provenance is not known");
          }
        }
      };
  walk(st.prov, std::nullopt, std::nullopt, false);
  st.separate.forEach([&](Key k, const State::Cell &c) {
    if (!shifts.count(k.first))
      walk(c.prov, k.first, k.first, owned.count(k.first));
  });
  return to;
}

void Memory::set(State &st, Term p, Term byte, uint64_t n) {
  if (fillObject(st, p, byte, n) || fillRange(st, p, byte, n))
    return;
  if (n > MaxBytes)
    throw Unsupported("a fill of " + std::to_string(n) + " bytes, not of a whole object");
  setBytes(st, p, byte, n);
}

void Memory::setBytes(State &st, Term p, Term byte, uint64_t n) {
  Term prov = t.bv(IntProvenance, ObjBits);
  if (std::optional<Key> k = key(p)) {
    uint64_t mask = ptrBits >= 64 ? ~uint64_t(0) : (uint64_t(1) << ptrBits) - 1;
    State::Cell cell{byte, prov};
    for (uint64_t i = 0; i < n; ++i) {
      st.known.set(*key(fieldPointer(ptr({k->first, (k->second + i) & mask}))), cell);
      if (st.known.size() > MaxKnown)
        flush(st);
    }
    return;
  }
  prepare(st, p);
  for (uint64_t i = 0; i < n; ++i)
    writeByte(st, bytePtr(p, i), {byte, prov});
}

bool Memory::copyArray(State &st, Term dst, Term src, uint64_t n, bool backing) {
  if (!backing)
    for (Term p : {dst, src})
      for (uint64_t o : candidates(p))
        if (fieldStorage.count(o))
          return false;
  if (n < MinSeparate)
    return false;
  if (ptrBits < 64 && n >= (uint64_t(1) << ptrBits))
    return false;
  Term c, x, y;
  if (t.split(dst, c, x, y)) {
    int pick = side(c);
    if (pick >= 0)
      return copyArray(st, pick ? x : y, src, n);
    State a = st, b = st;
    if (!copyArray(a, x, src, n) || !copyArray(b, y, src, n))
      return false;
    st = ite(c, a, b);
    return true;
  }
  if (t.split(src, c, x, y)) {
    if (int pick = side(c); pick >= 0)
      src = pick ? x : y;
  }
  const APInt *dstObjId = t.constant(object(dst));
  if (!dstObjId || dstObjId->isZero() || dstObjId->uge(objects.size()))
    return false;
  uint64_t dstObj = dstObjId->getZExtValue();

  // Flush all objects src may point into so their arrays are up to date.
  std::vector<uint64_t> srcObjs = candidates(src);
  for (uint64_t o : srcObjs)
    flush(st, o);

  Term dstOff = offset(dst);
  const APInt *dstOffC = t.constant(dstOff);
  const APInt *dstSize = t.constant(objects[dstObj].size);
  bool wholeDst = dstOffC && dstOffC->isZero() && dstSize && *dstSize == n;
  Term intProv = t.bv(IntProvenance, ObjBits);

  // Fast path 1: whole-object copy between two owned objects with matching
  // element width reuses src's arrays directly.
  if (wholeDst && owned.count(dstObj)) {
    if (const APInt *srcObjId = t.constant(object(src))) {
      uint64_t srcObj = srcObjId->getZExtValue();
      const APInt *srcOffC = t.constant(offset(src));
      if (srcOffC && srcOffC->isZero() && owned.count(srcObj) &&
          shifts.lookup(dstObj) == shifts.lookup(srcObj)) {
        if (const State::Cell *srcOwn = st.separate.find({srcObj, 0})) {
          st.known.eraseObject(dstObj);
          st.separate.set({dstObj, 0}, *srcOwn);
          return true;
        }
      }
    }
  }

  unsigned shift = shifts.lookup(dstObj);
  unsigned elemBytes = 1u << shift;

  // Fast path 2: whole-object copy from a constant-filled object becomes a
  // constant-array fill of dst.
  if (wholeDst && srcObjs.size() == 1) {
    if (const State::Cell *srcOwn = st.separate.find({srcObjs[0], 0})) {
      unsigned srcShift = shifts.lookup(srcObjs[0]);
      if (!srcShift) {
        if (Term fillByte = t.filledWith(srcOwn->data);
            fillByte && t.filledWith(srcOwn->prov) == intProv) {
          fill(st, dstObj, fillByte);
          return true;
        }
      } else if (srcShift == shift) {
        if (Term fillElem = t.filledWith(srcOwn->data)) {
          st.known.eraseObject(dstObj);
          st.separate.set({dstObj, 0},
                          {t.constArray(s.mkBVSort(ptrBits - shift), fillElem), intProv});
          return true;
        }
      }
    }
  }

  // An element-width destination array requires element-aligned offset and
  // length; unaligned copies fall back to byte-by-byte.
  if (shift && ((n & (elemBytes - 1)) != 0 ||
                !t.isTrue(t.eq(t.extract(dstOff, shift - 1, 0), t.bv(0, shift)))))
    return false;

  // For a partial copy, dstObj's bytes outside [dstOff, dstOff + n) must be in
  // its arrays before wrapping them in a lambda. For a whole-object copy, all
  // known bytes of dstObj are overwritten (and if dstObj was in srcObjs, it
  // was already flushed above).
  if (wholeDst)
    st.known.eraseObject(dstObj);
  else
    flush(st, dstObj);

  // Snapshot the pre-copy state for reading src, which preserves memmove
  // overlap semantics when dstObj == srcObj.
  State srcState = st;
  srcState.known.clear();

  const State::Cell *own = st.separate.find({dstObj, 0});
  State::Cell below = own ? *own : State::Cell{st.data, st.prov};
  bool byOffset = owned.count(dstObj);
  SMTSortRef indexSort = byOffset ? s.mkBVSort(ptrBits - shift) : pointerSort();
  Term dstObjTerm = t.bv(dstObj, ObjBits);

  auto offsetOf = [=, this](const Term &j) {
    return !byOffset ? offset(j) : shift ? t.concat(j, t.bv(0, shift)) : j;
  };

  // True when j is definitely outside the copied range [dstOff, dstOff + n),
  // so select forwards straight to below without instantiating the lambda.
  std::function<bool(const Term &)> misses;
  if (!wholeDst) {
    misses = [=, this](const Term &j) {
      if (!byOffset && t.isFalse(t.eq(object(j), dstObjTerm)))
        return true;
      if (!dstOffC)
        return false;
      std::optional<encode::Terms::Range> v = t.range(offsetOf(j));
      if (!v)
        return false;
      using U = unsigned __int128;
      U lo = dstOffC->getZExtValue(), end = lo + U(n);
      return end <= (U(1) << ptrBits) && (v->hi < lo || v->lo >= end);
    };
  }

  // When dst is an element array and src is a single owned element array of
  // the same width at an aligned offset, read elements directly with one select.
  const State::Cell *directSrc = nullptr;
  uint64_t directSrcObj = 0;
  if (shift && srcObjs.size() == 1 && shifts.lookup(srcObjs[0]) == shift &&
      t.isTrue(t.eq(t.extract(offset(src), shift - 1, 0), t.bv(0, shift)))) {
    directSrcObj = srcObjs[0];
    directSrc = srcState.separate.find({directSrcObj, 0});
  }
  Term directSrcData = directSrc ? directSrc->data : Term();

  Term len = t.bv(n, ptrBits);
  auto inRange = [=, this](const Term &j) -> std::pair<Term, Term> {
    Term off = offsetOf(j);
    Term d = t.sub(off, dstOff);
    if (wholeDst)
      return {t.boolean(true), d};
    return {t.ult(d, len), d};
  };

  std::string id = std::to_string(fresh++);
  Term newData = t.lambda(
      s.mkArraySort(indexSort, s.mkBVSort(8u << shift)), "copy#" + id,
      [=, this](const Term &j) {
        auto [in, d] = inRange(j);
        if (t.isFalse(in))
          return t.select(below.data, j);
        Term srcAt = advance(src, d);
        Term srcVal;
        if (directSrcData) {
          srcVal = t.select(directSrcData, index(directSrcObj, srcAt));
        } else {
          srcVal = readArrays(srcState, srcAt, false).data;
          for (unsigned b = 1; b < elemBytes; ++b)
            srcVal = t.concat(readArrays(srcState, bytePtr(srcAt, b), false).data, srcVal);
        }
        return t.isTrue(in) ? srcVal : t.ite(in, srcVal, t.select(below.data, j));
      },
      wholeDst ? Term() : below.data, misses);

  if (shift) {
    st.separate.set({dstObj, 0}, {newData, intProv});
    return true;
  }

  // If no src candidate can hold a pointer and below.prov holds none (or is
  // overwritten whole), the destination's provenance stays a constant array.
  llvm::SmallVector<ProvEdge, 2> provEdges;
  bool srcHasPointers = false;
  for (uint64_t o : srcObjs) {
    if (shifts.count(o))
      continue;
    const State::Cell *so = srcState.separate.find({o, 0});
    Term arr = so ? so->prov : srcState.prov;
    if (t.filledWith(arr) == intProv)
      continue;
    srcHasPointers = true;
    provEdges.push_back({arr, o, so && owned.count(o)});
  }

  Term newProv;
  if (!srcHasPointers && (wholeDst || t.filledWith(below.prov) == intProv)) {
    newProv = t.constArray(indexSort, intProv);
  } else {
    newProv = t.lambda(
        s.mkArraySort(indexSort, s.mkBVSort(ObjBits)), "copy_provenance#" + id,
        [=, this](const Term &j) {
          auto [in, d] = inRange(j);
          if (t.isFalse(in))
            return t.select(below.prov, j);
          Term srcProv = srcHasPointers ? readArrays(srcState, advance(src, d)).prov : intProv;
          return t.isTrue(in) ? srcProv : t.ite(in, srcProv, t.select(below.prov, j));
        },
        wholeDst ? Term() : below.prov, misses);
    if (!wholeDst)
      provEdges.push_back({below.prov, std::nullopt, byOffset});
    provBelow[newProv.id()] = {newProv, std::move(provEdges)};
  }

  st.separate.set({dstObj, 0}, {newData, newProv});
  return true;
}

void Memory::copy(State &st, Term dst, Term src, uint64_t n) {
  if (!n || dst == src)
    return;
  auto d = key(dst), s = key(src);
  if (d && s && d->second == 0 && s->second == 0) {
    auto to = fieldStorage.find(d->first), from = fieldStorage.find(s->first);
    if (to != fieldStorage.end() && from != fieldStorage.end() &&
        to->second.shift == from->second.shift &&
        to->second.fields.size() == from->second.fields.size()) {
      const APInt *ds = t.constant(objects[d->first].size);
      const APInt *ss = t.constant(objects[s->first].size);
      std::vector<std::pair<FieldStorage, FieldStorage>> pairs;
      if (ds && ss && *ds == n && *ss == n)
        for (const auto &a : to->second.fields)
          for (const auto &b : from->second.fields)
            if (a.field.offset == b.field.offset && a.field.bytes == b.field.bytes)
              pairs.emplace_back(a, b);
      if (pairs.size() == to->second.fields.size()) {
        for (const auto &[a, b] : pairs)
          copy(st, ptr(t.bv(a.object, ObjBits), t.bv(0, ptrBits)),
               ptr(t.bv(b.object, ObjBits), t.bv(0, ptrBits)),
               (n >> to->second.shift) * a.field.bytes);
        // The original backing still preserves padding and unobserved fields.
        if (copyArray(st, dst, src, n, true))
          return;
      }
    }
  }
  if (copyArray(st, dst, src, n))
    return;
  if (n > MaxBytes)
    throw Unsupported("a copy of " + std::to_string(n) + " bytes");
  // Read everything first, so overlapping ranges copy as memmove does.
  prepare(st, src);
  prepare(st, dst);
  uint64_t mask = ptrBits >= 64 ? ~uint64_t(0) : (uint64_t(1) << ptrBits) - 1;
  std::vector<State::Cell> bytes;
  bytes.reserve(n);
  if (std::optional<Key> sk = key(src)) {
    for (uint64_t i = 0; i < n; ++i) {
      Key ki{sk->first, (sk->second + i) & mask};
      if (const State::Cell *c = st.known.find(ki))
        bytes.push_back(*c);
      else
        bytes.push_back(readArrays(st, ptr(ki)));
    }
  } else {
    for (uint64_t i = 0; i < n; ++i)
      bytes.push_back(readByte(st, bytePtr(src, i)));
  }
  if (std::optional<Key> dk = key(dst)) {
    for (uint64_t i = 0; i < n; ++i) {
      st.known.set(*key(fieldPointer(ptr({dk->first, (dk->second + i) & mask}))), bytes[i]);
      if (st.known.size() > MaxKnown)
        flush(st);
    }
  } else {
    for (uint64_t i = 0; i < n; ++i)
      writeByte(st, bytePtr(dst, i), bytes[i]);
  }
}

void Memory::initialise(State &st, const Module &m,
                        const std::function<Term(const Constant *)> &valueOf) {
  initialising = true;
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
  initialising = false;
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

// If c is an array (possibly nested) whose elements are all identical, returns
// the repeated non-array element and sets count to the total element count.
static const Constant *splatElement(const Constant *c, uint64_t &count) {
  count = 1;
  while (auto *at = dyn_cast<ArrayType>(c->getType())) {
    uint64_t n = at->getNumElements();
    if (n == 0)
      return nullptr;
    const Constant *first = nullptr;
    if (auto *cda = dyn_cast<ConstantDataArray>(c)) {
      StringRef raw = cda->getRawDataValues();
      unsigned elemBytes = cda->getElementByteSize();
      if (raw.size() != n * elemBytes ||
          (n > 1 && raw.drop_back(elemBytes) != raw.drop_front(elemBytes)))
        return nullptr;
      first = cda->getElementAsConstant(0);
    } else if (auto *ca = dyn_cast<ConstantArray>(c)) {
      first = ca->getOperand(0);
      for (uint64_t i = 1; i < n; ++i)
        if (ca->getOperand(i) != first)
          return nullptr;
    } else {
      return nullptr;
    }
    count *= n;
    c = first;
  }
  return c;
}

void Memory::storeConstant(State &st, Term p, const Constant *c,
                           const std::function<Term(const Constant *)> &valueOf) {
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
    uint64_t count = 0;
    if (const Constant *elem = splatElement(c, count)) {
      if (isa<UndefValue>(elem))
        return;
      if (elem->isNullValue()) {
        fillOrSet(st, p, t.bv(0, 8), dl.getTypeAllocSize(ty).getFixedValue());
        return;
      }
      Type *elemTy = elem->getType();
      uint64_t elemStride = dl.getTypeAllocSize(elemTy);
      if (elemTy->isIntegerTy() || elemTy->isFloatingPointTy()) {
        Term bits;
        if (auto *cf = dyn_cast<ConstantFP>(elem)) {
          bits = t.bv(cf->getValueAPF().bitcastToAPInt());
        } else {
          bits = valueOf(elem);
          unsigned sb = storeBits(dl, elemTy);
          if (elemTy->isIntegerTy(1))
            bits = t.ite(bits, t.bv(1, 8), t.bv(0, 8));
          else if (bits.width() < sb)
            bits = t.zext(bits, sb - bits.width());
        }
        if (fillElementRange(st, p, bits, elemStride, count))
          return;
      }
    }
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
