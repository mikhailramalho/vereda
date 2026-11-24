// Stage memory: a persistent map from (object, offset) to a byte's data and
// provenance terms, for the known bytes of a memory state.
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
#include <memory>
#include <utility>

namespace vereda::memory {

struct Cell {
  encode::Term data, prov;
  bool operator==(const Cell &o) const { return data == o.data && prov == o.prov; }
};

class CellMap {
public:
  using Key = std::pair<uint64_t, uint64_t>;

  size_t size() const { return root ? root->size : 0; }
  bool same(const CellMap &o) const { return root == o.root; }

  const Cell *find(Key k) const {
    U u = pack(k);
    const Node *n = root.get();
    while (n && n->bit) {
      if (!inside(u, n))
        return nullptr;
      n = (u & n->bit) ? n->right.get() : n->left.get();
    }
    return n && n->prefix == u ? &n->cell : nullptr;
  }
  void set(Key k, const Cell &c) { root = insert(root, pack(k), c); }
  void eraseObject(uint64_t o) { root = erase(root, pack({o, 0}), pack({o, ~uint64_t(0)})); }
  void clear() { root.reset(); }

  // f(key, cell) in key order.
  template <class F> void forEach(F &&f) const { each(root.get(), f); }
  template <class F> void forObject(uint64_t o, F &&f) const {
    range(root.get(), pack({o, 0}), pack({o, ~uint64_t(0)}), f);
  }
  // f(key, a's cell or null, b's cell or null) once for each key whose cells
  // differ, skipping subtrees a and b share.
  template <class F> static void diff(const CellMap &a, const CellMap &b, F &&f) {
    diffNodes(a.root.get(), b.root.get(), f);
  }

private:
  using U = unsigned __int128;
  struct Node;
  using Ptr = std::shared_ptr<const Node>;
  // A leaf has bit 0 and its key as prefix. A branch holds the keys whose
  // bits above bit equal prefix's: those with bit clear on the left.
  struct Node {
    U prefix, bit;
    size_t size;
    Ptr left, right;
    Cell cell;
  };
  Ptr root;

  static U pack(Key k) { return (U(k.first) << 64) | k.second; }
  static Key unpack(U u) { return {uint64_t(u >> 64), uint64_t(u)}; }
  static U above(U u, U bit) { return u & ~((bit << 1) - 1); } // the bits above bit
  static bool inside(U u, const Node *n) { return above(u, n->bit) == n->prefix; }
  static U last(const Node *n) { return n->bit ? n->prefix | ((n->bit << 1) - 1) : n->prefix; }
  static U highest(U x) {
    uint64_t hi = uint64_t(x >> 64);
    return hi ? U(1) << (127 - __builtin_clzll(hi)) : U(1) << (63 - __builtin_clzll(uint64_t(x)));
  }

  static Ptr leaf(U k, const Cell &c) { return std::make_shared<const Node>(Node{k, 0, 1, {}, {}, c}); }
  static Ptr branch(U prefix, U bit, Ptr l, Ptr r) {
    size_t n = l->size + r->size;
    return std::make_shared<const Node>(Node{prefix, bit, n, std::move(l), std::move(r), {}});
  }
  // Two trees whose keys differ above both their branch bits.
  static Ptr join(U p0, Ptr t0, U p1, Ptr t1) {
    U m = highest(p0 ^ p1);
    return (p0 & m) ? branch(above(p0, m), m, std::move(t1), std::move(t0))
                    : branch(above(p0, m), m, std::move(t0), std::move(t1));
  }

  static Ptr insert(const Ptr &t, U k, const Cell &c) {
    if (!t)
      return leaf(k, c);
    if (!t->bit) {
      if (t->prefix != k)
        return join(k, leaf(k, c), t->prefix, t);
      return t->cell == c ? t : leaf(k, c);
    }
    if (!inside(k, t.get()))
      return join(k, leaf(k, c), t->prefix, t);
    if (k & t->bit) {
      Ptr r = insert(t->right, k, c);
      return r == t->right ? t : branch(t->prefix, t->bit, t->left, std::move(r));
    }
    Ptr l = insert(t->left, k, c);
    return l == t->left ? t : branch(t->prefix, t->bit, std::move(l), t->right);
  }

  // Removes the keys in [lo, hi].
  static Ptr erase(const Ptr &t, U lo, U hi) {
    if (!t || last(t.get()) < lo || t->prefix > hi)
      return t;
    if (lo <= t->prefix && last(t.get()) <= hi)
      return nullptr;
    Ptr l = erase(t->left, lo, hi), r = erase(t->right, lo, hi);
    if (l == t->left && r == t->right)
      return t;
    if (!l || !r)
      return l ? l : r;
    return branch(t->prefix, t->bit, std::move(l), std::move(r));
  }

  template <class F> static void each(const Node *n, F &&f) {
    if (!n)
      return;
    if (!n->bit) {
      f(unpack(n->prefix), n->cell);
      return;
    }
    each(n->left.get(), f);
    each(n->right.get(), f);
  }
  template <class F> static void range(const Node *n, U lo, U hi, F &&f) {
    if (!n || last(n) < lo || n->prefix > hi)
      return;
    if (!n->bit) {
      f(unpack(n->prefix), n->cell);
      return;
    }
    range(n->left.get(), lo, hi, f);
    range(n->right.get(), lo, hi, f);
  }

  template <class F> static void diffNodes(const Node *a, const Node *b, F &&f) {
    if (a == b)
      return;
    auto onlyA = [&](const Node *n) {
      each(n, [&](Key k, const Cell &c) { f(k, &c, nullptr); });
    };
    auto onlyB = [&](const Node *n) {
      each(n, [&](Key k, const Cell &c) { f(k, nullptr, &c); });
    };
    if (!a || !b) {
      a ? onlyA(a) : onlyB(b);
      return;
    }
    // A leaf on one side: look its key up on the other.
    if (!a->bit || !b->bit) {
      const Node *l = a->bit ? b : a, *t = a->bit ? a : b;
      Key k = unpack(l->prefix);
      const Cell *other = nullptr;
      each(t, [&](Key kt, const Cell &c) {
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
    if (a->bit == b->bit && a->prefix == b->prefix) {
      diffNodes(a->left.get(), b->left.get(), f);
      diffNodes(a->right.get(), b->right.get(), f);
    } else if (a->bit > b->bit && inside(b->prefix, a)) { // b within one side of a
      if (b->prefix & a->bit) {
        onlyA(a->left.get());
        diffNodes(a->right.get(), b, f);
      } else {
        diffNodes(a->left.get(), b, f);
        onlyA(a->right.get());
      }
    } else if (b->bit > a->bit && inside(a->prefix, b)) {
      if (a->prefix & b->bit) {
        onlyB(b->left.get());
        diffNodes(a, b->right.get(), f);
      } else {
        diffNodes(a, b->left.get(), f);
        onlyB(b->right.get());
      }
    } else {
      onlyA(a);
      onlyB(b);
    }
  }
};

} // namespace vereda::memory
