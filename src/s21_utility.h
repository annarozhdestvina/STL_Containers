#ifndef _S21_UTILITY_H_
#define _S21_UTILITY_H_

namespace s21 {

namespace Utility {

// move-only
template <typename NodePointer, typename ValueReference>
class Handler {
 private:
  NodePointer pointer_;

 public:
  Handler(NodePointer pointer) : pointer_{pointer} {}

  Handler(const Handler& other) = delete;  // запретили создавать копированием

  Handler& operator=(const Handler& other) = delete;  // запретили копировать

  Handler(Handler&& other) noexcept : pointer_{other.pointer_} {
    other.pointer_ = nullptr;
  }

  NodePointer operator->() { return pointer_; }
  NodePointer operator->() const { return pointer_; }

  ValueReference Get() { return pointer_->value_; }
  const ValueReference Get() const { return pointer_->value_; }

  Handler& operator=(Handler&& other) noexcept {
    if (this == &other) return *this;

    this->~Handler();
    pointer_ = other.pointer_;
    other.pointer_ = nullptr;
    return *this;
  }

  ~Handler() {
    delete pointer_;
    pointer_ = nullptr;
  }
};

template <typename Key>
class Less {
 public:
  bool operator()(const Key& left, const Key& right) const {
    return left < right;
  };
};

template <typename Key>
class KeyGetterSet {
 public:
  const Key& operator()(const Key& value) const { return value; }
};

template <typename Key, typename Pair>
class KeyGetterMap {
 public:
  const Key& operator()(const Pair& pair) const { return pair.first; }
};

}  // namespace Utility

}  //  namespace s21

#endif  //  _S21_UTILITY_H_