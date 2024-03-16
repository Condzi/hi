#pragma once 
#include "all_inc.hpp"

// Doubly Linked List
//

template <DLLNode T>
void
DLL_insert_at_front(T *&root, T *node) {
  Assert(node);
  node->next = root;
  if (root) {
    root->prev = node;
  }
  root       = node;
  node->prev = 0;
}

template <DLLNode T>
void
DLL_insert_at_end(T *&root, T *node) {
  Assert(node);

  if (root) {
    T *it = root;
    while (it->next) {
      it = it->next;
    }

    it->next   = node;
    node->prev = it;
  } else {
    root       = node;
    node->prev = 0;
  }
}

template <DLLNode T>
void
DLL_remove(T *&root, T *&node) {
  Assert(root);
  Assert(node);

  if (node->prev) {
    node->prev->next = node->next;
  }
  if (node->next) {
    node->next->prev = node->prev;
  }

  if (root == node) {
    root = node->next;
  }

  node->next = node->prev = 0;
}

template <DLLNode T, UnsignedInteger TBase, UnsignedInteger TPtr>
void
DLL_insert_at_end_relative(TBase base, TPtr &root_idx, TPtr node_idx) {
  Assert(base);

  T *node = (T *)U64ToPtr(base + node_idx);
  if (root_idx) {
    T *it = (T *)U64ToPtr(base + root_idx);
    while (it->next) {
      it = (T *)U64ToPtr(base + it->next);
    }

    it->next   = node_idx;
    node->prev = PtrToU64(it) - base;
  } else {
    root_idx   = node_idx;
    node->prev = 0;
  }
}

template <DLLNode T, UnsignedInteger TBase, UnsignedInteger TPtr>
void
DLL_remove_relative(TBase base, TPtr &root_idx, TPtr node_idx) {
  Assert(base);

  T *node = (T *)U64ToPtr(base + node_idx);
  if (node->prev) {
    T *prev    = (T *)U64ToPtr(base + node->prev);
    prev->next = node->next;
  }
  if (node->next) {
    T *next    = (T *)U64ToPtr(base + node->next);
    next->prev = node->prev;
  }

  if (root_idx == node_idx) {
    root_idx = node->next;
  }

  node->next = node->prev = 0;
}


// Singly Linked List
//

template <SLLNode T>
void
SLL_insert_at_end(T *&root, T *&node) {
  if (!root) {
    root = node;
  } else {
    T *it = root;
    while (it->next) {
      it = it->next;
    }

    it->next = node;
  }
}

template <SLLNode T, UnsignedInteger TBase, UnsignedInteger TPtr>
void
SLL_insert_at_end_relative(TBase base, TPtr &root_idx, TPtr node_idx) {
  Assert(base);

  if (!root_idx) {
    root_idx = node_idx;
  } else {
    T *it = (T *)U64ToPtr(base + root_idx);
    while (it->next) {
      it = (T *)U64ToPtr(base + it->next);
    }

    it->next = node_idx;
  }
}
