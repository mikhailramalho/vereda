// Stage memory: a persistent map from (object, offset) to a byte's data and
// provenance terms, for the known bytes of a memory state; the encoder keeps
// its path facts in one too (Trie of another cell type).
//
// Copying a map copies one pointer, and a change rebuilds only the path to
// its key, so the walker can keep a state per arrival at a block without
// copying its bytes. The map is a big-endian Patricia trie on the 128-bit key
// object:offset: its shape depends only on its keys, so two maps that grew
// from one share whole subtrees, which diff() skips. In-order iteration is in
// key order, and an object's keys are one subtree.
#pragma once
#include "encode/terms.h"
#include <cstddef>
#include <cstdint>
#include <utility>

namespace vereda::memory {

struct Cell {
  encode::Term data, prov;
  bool operator==(const Cell &o) const { return data == o.data && prov == o.prov; }
};

template <class C> class Trie {
public:
  using Key = std::pair<uint64_t, uint64_t>;

  size_t size() const { return root ? nodeSize(root.get()) : 0; }
  bool same(const Trie &o) const { return root == o.root; }

  const C *find(Key k) const {
    U u = pack(k);
    const Node *n = root.get();
    while (n && n->isBranch()) {
      const auto *b = asBranch(n);
      if (!inside(u, b))
        return nullptr;
      n = (u & b->bit()) ? b->right.get() : b->left.get();
    }
    return n && n->prefix() == u ? &asLeaf(n)->cell : nullptr;
  }
  void set(Key k, const C &c) { root = insert(root, pack(k), c); }
  void eraseObject(uint64_t o) { root = erase(root, pack({o, 0}), pack({o, ~uint64_t(0)})); }
  void eraseRange(uint64_t o, uint64_t lo, uint64_t hi) {
    root = erase(root, pack({o, lo}), pack({o, hi}));
  }
  void clear() { root.reset(); }

  // f(key, cell) in key order.
  template <class F> void forEach(F &&f) const { each(root.get(), f); }
  template <class F> void forObject(uint64_t o, F &&f) const {
    range(root.get(), pack({o, 0}), pack({o, ~uint64_t(0)}), f);
  }
  // f(key, a's cell or null, b's cell or null) once for each key whose cells
  // differ, skipping subtrees a and b share.
  template <class F> static void diff(const Trie &a, const Trie &b, F &&f) {
    diffNodes(a.root.get(), b.root.get(), f);
  }

private:
  using U = unsigned __int128;
  static constexpr uint8_t LeafBit = 255;

  // Intrusively refcounted split Leaf/Branch nodes keep the per-byte footprint
  // small (40/48 B instead of 128 B with std::shared_ptr). Vereda walks on one
  // thread, so non-atomic counts suffice.
  struct Node {
    mutable uint32_t refs = 1;
    uint8_t bitPos; // LeafBit for a leaf; 0..127 for a branch's distinguishing bit
    uint64_t prefixHi, prefixLo;
    Node(uint8_t b, U p) : bitPos(b), prefixHi(uint64_t(p >> 64)), prefixLo(uint64_t(p)) {}
    bool isBranch() const { return bitPos != LeafBit; }
    U prefix() const { return (U(prefixHi) << 64) | prefixLo; }
    U bit() const { return U(1) << bitPos; }
  };
  struct Leaf;
  struct Branch;

  class Ptr {
  public:
    Ptr() = default;
    explicit Ptr(const Node *p) : p(p) {}
    Ptr(const Ptr &o) : p(o.p) {
      if (p)
        ++p->refs;
    }
    Ptr(Ptr &&o) noexcept : p(o.p) { o.p = nullptr; }
    Ptr &operator=(const Ptr &o) {
      if (p != o.p) {
        if (o.p)
          ++o.p->refs;
        release();
        p = o.p;
      }
      return *this;
    }
    Ptr &operator=(Ptr &&o) noexcept {
      if (this != &o) {
        release();
        p = o.p;
        o.p = nullptr;
      }
      return *this;
    }
    ~Ptr() { release(); }
    void reset() {
      release();
      p = nullptr;
    }
    const Node *get() const { return p; }
    explicit operator bool() const { return p != nullptr; }
    bool operator==(const Ptr &o) const { return p == o.p; }

  private:
    void release() {
      if (p && --p->refs == 0)
        destroy(p);
    }
    const Node *p = nullptr;
  };

  struct Leaf : Node {
    C cell;
    Leaf(U k, const C &c) : Node(LeafBit, k), cell(c) {}
  };
  struct Branch : Node {
    size_t size;
    Ptr left, right;
    Branch(U p, uint8_t b, size_t n, Ptr l, Ptr r)
        : Node(b, p), size(n), left(std::move(l)), right(std::move(r)) {}
  };

  static const Leaf *asLeaf(const Node *n) { return static_cast<const Leaf *>(n); }
  static const Branch *asBranch(const Node *n) { return static_cast<const Branch *>(n); }
  static size_t nodeSize(const Node *n) { return n->isBranch() ? asBranch(n)->size : 1; }
  static void destroy(const Node *n) {
    if (n->isBranch())
      delete asBranch(n);
    else
      delete asLeaf(n);
  }

  Ptr root;

  static U pack(Key k) { return (U(k.first) << 64) | k.second; }
  static Key unpack(U u) { return {uint64_t(u >> 64), uint64_t(u)}; }
  static U above(U u, U bit) { return u & ~((bit << 1) - 1); } // the bits above bit
  static bool inside(U u, const Node *n) { return above(u, n->bit()) == n->prefix(); }
  static U last(const Node *n) {
    return n->isBranch() ? n->prefix() | ((n->bit() << 1) - 1) : n->prefix();
  }
  static uint8_t highestPos(U x) {
    uint64_t hi = uint64_t(x >> 64);
    return hi ? uint8_t(127 - __builtin_clzll(hi)) : uint8_t(63 - __builtin_clzll(uint64_t(x)));
  }

  static Ptr leaf(U k, const C &c) { return Ptr(new Leaf(k, c)); }
  static Ptr branch(U prefix, uint8_t bitPos, Ptr l, Ptr r) {
    size_t n = nodeSize(l.get()) + nodeSize(r.get());
    return Ptr(new Branch(prefix, bitPos, n, std::move(l), std::move(r)));
  }
  // Two trees whose keys differ above both their branch bits.
  static Ptr join(U p0, Ptr t0, U p1, Ptr t1) {
    uint8_t pos = highestPos(p0 ^ p1);
    U m = U(1) << pos;
    return (p0 & m) ? branch(above(p0, m), pos, std::move(t1), std::move(t0))
                    : branch(above(p0, m), pos, std::move(t0), std::move(t1));
  }

  static Ptr insert(const Ptr &t, U k, const C &c) {
    if (!t)
      return leaf(k, c);
    const Node *n = t.get();
    if (!n->isBranch()) {
      if (n->prefix() != k)
        return join(k, leaf(k, c), n->prefix(), t);
      return asLeaf(n)->cell == c ? t : leaf(k, c);
    }
    const Branch *b = asBranch(n);
    if (!inside(k, b))
      return join(k, leaf(k, c), b->prefix(), t);
    if (k & b->bit()) {
      Ptr r = insert(b->right, k, c);
      return r == b->right ? t : branch(b->prefix(), b->bitPos, b->left, std::move(r));
    }
    Ptr l = insert(b->left, k, c);
    return l == b->left ? t : branch(b->prefix(), b->bitPos, std::move(l), b->right);
  }

  // Removes the keys in [lo, hi].
  static Ptr erase(const Ptr &t, U lo, U hi) {
    if (!t)
      return t;
    const Node *n = t.get();
    if (last(n) < lo || n->prefix() > hi)
      return t;
    if (lo <= n->prefix() && last(n) <= hi)
      return {};
    const Branch *b = asBranch(n);
    Ptr l = erase(b->left, lo, hi), r = erase(b->right, lo, hi);
    if (l == b->left && r == b->right)
      return t;
    if (!l || !r)
      return l ? l : r;
    return branch(b->prefix(), b->bitPos, std::move(l), std::move(r));
  }

  template <class F> static void each(const Node *n, F &&f) {
    if (!n)
      return;
    if (!n->isBranch()) {
      f(unpack(n->prefix()), asLeaf(n)->cell);
      return;
    }
    const Branch *b = asBranch(n);
    each(b->left.get(), f);
    each(b->right.get(), f);
  }
  template <class F> static void range(const Node *n, U lo, U hi, F &&f) {
    if (!n || last(n) < lo || n->prefix() > hi)
      return;
    if (!n->isBranch()) {
      f(unpack(n->prefix()), asLeaf(n)->cell);
      return;
    }
    const Branch *b = asBranch(n);
    range(b->left.get(), lo, hi, f);
    range(b->right.get(), lo, hi, f);
  }

  template <class F> static void diffNodes(const Node *a, const Node *b, F &&f) {
    if (a == b)
      return;
    auto onlyA = [&](const Node *n) {
      each(n, [&](Key k, const C &c) { f(k, &c, nullptr); });
    };
    auto onlyB = [&](const Node *n) {
      each(n, [&](Key k, const C &c) { f(k, nullptr, &c); });
    };
    if (!a || !b) {
      a ? onlyA(a) : onlyB(b);
      return;
    }
    // A leaf on one side: look its key up on the other.
    if (!a->isBranch() || !b->isBranch()) {
      const Leaf *l = a->isBranch() ? asLeaf(b) : asLeaf(a);
      const Node *t = a->isBranch() ? a : b;
      Key k = unpack(l->prefix());
      const C *other = nullptr;
      each(t, [&](Key kt, const C &c) {
        if (kt == k)
          other = &c;
        else if (t == a)
          f(kt, &c, nullptr);
        else
          f(kt, nullptr, &c);
      });
      if (!other || !(*other == l->cell)) {
        if (l == a)
          f(k, &l->cell, other);
        else
          f(k, other, &l->cell);
      }
      return;
    }
    const Branch *ba = asBranch(a), *bb = asBranch(b);
    if (ba->bitPos == bb->bitPos && ba->prefix() == bb->prefix()) {
      diffNodes(ba->left.get(), bb->left.get(), f);
      diffNodes(ba->right.get(), bb->right.get(), f);
    } else if (ba->bitPos > bb->bitPos && inside(bb->prefix(), ba)) { // b within one side of a
      if (bb->prefix() & ba->bit()) {
        onlyA(ba->left.get());
        diffNodes(ba->right.get(), bb, f);
      } else {
        diffNodes(ba->left.get(), bb, f);
        onlyA(ba->right.get());
      }
    } else if (bb->bitPos > ba->bitPos && inside(ba->prefix(), bb)) {
      if (ba->prefix() & bb->bit()) {
        onlyB(bb->left.get());
        diffNodes(ba, bb->right.get(), f);
      } else {
        diffNodes(ba, bb->left.get(), f);
        onlyB(bb->right.get());
      }
    } else {
      onlyA(ba);
      onlyB(bb);
    }
  }
};

using CellMap = Trie<Cell>;

} // namespace vereda::memory
