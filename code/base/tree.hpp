#pragma once

template <typename T>
concept TreeNode = requires(T a) {
  { a.parent };
  { a.first_child };
  { a.next };
};

template <TreeNode T>
using process_node_proc = void(*)(T* node);

template <TreeNode T>
void Tree_pre_order(T* node, process_node_proc<T> f) {
  if (!node) {
    return;
  }

  f(node);
  
  T* child = node->first_child;
  while (child) {
    Tree_pre_order(child, f);
    child = child->next;
  }
}

template <TreeNode T>
void Tree_post_order(T* node, process_node_proc<T> f) {
  if (!node) {
    return;
  }

  T* child = node->first_child;
  while (child) {
    Tree_post_order(child, f);
    child = child->next;
  }

  f(node);
}
