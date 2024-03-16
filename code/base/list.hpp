#pragma once

// List concepts
//

template <typename T>
concept DLLNode = requires(T a) {
  { a.next };
  { a.prev };
};

template <typename T>
concept SLLNode = requires(T a) {
  { a.next };
};


// Doubly Linked List
//

template <DLLNode T>
void
DLL_insert_at_front(T *&root, T *node);

template <DLLNode T>
void
DLL_insert_at_end(T *&root, T *node);

template <DLLNode T>
void
DLL_remove(T *&root, T *&node);

template <DLLNode T, UnsignedInteger TBase, UnsignedInteger TPtr>
void
DLL_insert_at_end_relative(TBase base, TPtr &root_idx, TPtr node_idx);

template <DLLNode T, UnsignedInteger TBase, UnsignedInteger TPtr>
void
DLL_remove_relative(TBase base, TPtr &root_idx, TPtr node_idx);


// Singly Linked List
//

template <SLLNode T>
void
SLL_insert_at_end(T *&root, T *&node);

template <SLLNode T, UnsignedInteger TBase, UnsignedInteger TPtr>
void
SLL_insert_at_end_relative(TBase base, TPtr &root_idx, TPtr node_idx);
