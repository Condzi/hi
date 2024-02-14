#pragma once

// List Manipulation
//

template <typename T>
void
DLL_insert_at_end(T *&root, T *&node) {
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

template <typename T>
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

template <typename T>
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
