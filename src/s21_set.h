#ifndef _S21_SET_H_
#define _S21_SET_H_

#include "s21_tree.h"
#include "s21_comparator.h"

namespace s21
{

template <typename Type, 
          typename Comparator = ComparatorSet<Type>,
          typename Node = Tree::SingleNode<Type, std::ptrdiff_t>>
class Set : public Tree::Tree<Type, Comparator, Node> {
public:
    using Base = Tree::Tree<Type, Comparator, Node>;

public:
    using Base::Base;

};

}   // namespace s21

#endif  //  _S21_SET_H_