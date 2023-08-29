#ifndef _S21_MAP_H_
#define _S21_MAP_H_

#include <cassert>
#include <initializer_list>
#include <iterator>
#include <new>
#include <cstring>

#include "s21_list.h"

namespace s21
{

template <typename Map, typename Pointer, typename Reference, typename Node_pointer, typename Difference_type,
          /*typename Key_type,*/ typename Value_type>
// template <typename Vector, typename Pointer, typename Reference, typename Difference_type,
        //   typename Value_type>
class MapIteratorBase
{
  public:
    using difference_type = Difference_type;
    using value_type = Value_type;
    //using key_type = Key_type;
    using pointer = Pointer;
    using reference = Reference;
    using size_type = typename Map::size_type;
    using iterator_category = std::bidirectional_iterator_tag;
    using node_pointer = Node_pointer;


    // template <typename OtherVector, typename OtherPointer, typename OtherReference, typename OtherDifference_type, typename OtherValue_type>
    // friend class VectorIteratorBase;  // to be able to compare iterator and const_iterator


  public:
    MapIteratorBase(node_pointer p) noexcept : pointer_{p} {}

    
    // template <typename OtherPointer, typename OtherReference> // to be able to compare iterator and const_iterator
    // VectorIteratorBase(const VectorIteratorBase<Vector, OtherPointer, OtherReference, Difference_type, Value_type> &other) noexcept 
    //     : VectorIteratorBase(const_cast<Pointer>(other.pointer_)) 
    // {

    // }

    
    MapIteratorBase(const MapIteratorBase &other) noexcept 
        : MapIteratorBase(other.pointer_) 
    {

    }


    // template <typename OtherPointer, typename OtherReference> // to be able to compare iterator and const_iterator
    // VectorIteratorBase(VectorIteratorBase<Vector, OtherPointer, OtherReference, Difference_type, Value_type> &&other) noexcept 
    //     : VectorIteratorBase(const_cast<Pointer>(other.pointer_))
    // {
    //     other.pointer_ = nullptr;
    // }

    MapIteratorBase(MapIteratorBase &&other) noexcept 
        : MapIteratorBase(other.pointer_)
    {
        other.pointer_ = nullptr;
    }
    
    friend void swap(MapIteratorBase &left, MapIteratorBase &right) noexcept
    {
        using namespace std; // to enable ADL
        swap(left.pointer_, right.pointer_);
    }

    // MapIteratorBase &operator++() noexcept
    // {
    //     pointer_;
    //     ++
    // }

    // MapIteratorBase &operator--() noexcept
    // {
    //     --
    // }


    // template <typename OtherPointer, typename OtherReference> // to be able to compare iterator and const_iterator
    // VectorIteratorBase<Vector, OtherPointer, OtherReference, Difference_type, Value_type> &operator=(const VectorIteratorBase<Vector, OtherPointer, OtherReference, Difference_type, Value_type> &other) noexcept
    // {
    //     pointer_ = other.pointer_;
    //     return *this;
    // }

    // template <typename OtherPointer, typename OtherReference> // to be able to compare iterator and const_iterator
    // VectorIteratorBase<Vector, OtherPointer, OtherReference, Difference_type, Value_type> &operator=(VectorIteratorBase<Vector, OtherPointer, OtherReference, Difference_type, Value_type> &&other) noexcept
    // {
    //     pointer_ = other.pointer_;
    //     other.pointer_ = nullptr;
    //     return *this;
    // }



    MapIteratorBase &operator=(const MapIteratorBase &other) noexcept
    {
        pointer_ = other.pointer_;
        return *this;
    }

    MapIteratorBase &operator=(MapIteratorBase &&other) noexcept
    {
        pointer_ = other.pointer_;
        other.pointer_ = nullptr;
        return *this;
    }



    reference operator*() const noexcept
    {
        return pointer_->value_;
    }

    pointer operator->() const noexcept
    {
        return &(pointer_->value_);
    }

    size_type LeftHeight() const noexcept
    {
        return pointer_->lHeight_;
    }

    size_type RightHeight() const noexcept
    {
        return pointer_->rHeight_;
    }


    // template <typename OtherPointer, typename OtherReference> // to be able to compare iterator and const_iterator
    // bool operator==(const VectorIteratorBase<Vector, OtherPointer, OtherReference, Difference_type, Value_type> &other) const noexcept
    // {
    //     return pointer_ == other.pointer_;
    // }
    bool operator==(const MapIteratorBase &other) const noexcept
    {
        return pointer_ == other.pointer_;
    }

    // template <typename OtherPointer, typename OtherReference>
    // bool operator!=(const VectorIteratorBase<Vector, OtherPointer, OtherReference, Difference_type, Value_type> &other) const noexcept
    // {
    //     return !(*this == other);
    // }
    
    bool operator!=(const MapIteratorBase &other) const noexcept
    {
        return !(*this == other);
    }

    explicit operator bool() const noexcept
    {
        return pointer_;
    }

   

  protected:
    node_pointer pointer_;
};

template <typename Map, typename Pointer = typename Map::pointer, typename Reference = typename Map::reference, typename Node_pointer = typename Map::node_pointer>
class MapIterator : public MapIteratorBase<Map, Pointer, Reference, Node_pointer, typename Map::difference_type, /*typename Map::key_type,*/ typename Map::value_type>
{
    // template<typename K, typename V, typename C>
    // friend class Map;  // to compare const_iterator with iterator

    // template<typename K, typename V, typename C>
    friend Map;  // to compare const_iterator with iterator


  private:
    using Base = MapIteratorBase<Map, Pointer, Reference, Node_pointer, typename Map::difference_type, /*typename Map::key_type,*/
                                  typename Map::value_type>;
  public:
    using typename Base::difference_type;   // otherwise everywhere in this class 'typename Base::difference_type' instead of 'difference_type'
    using typename Base::node_pointer;

    // template<typename VectorType, typename PointerType, typename ReferenceType>
    // friend class VectorIterator;  // to compare const_iterator with iterator

  public:
    using Base::Base;

    //          1
    //              4
    //                  6
    //                      7

    //          4
    //     1        6
    //                  7

    //                          
    //                4
    //          2               6
    //        1   3       5          7
    //     re         4.5  5.5


    // MapIterator &operator++() noexcept override  // ++i
    MapIterator &operator++() noexcept  // ++i
    {
        if (!this->pointer_->right_) {
            node_pointer temp = this->pointer_;
            while (true) {
                // find greater root
                if (!this->pointer_->root_) {
                    this->pointer_ = temp->right_;  // temp->right_ may be nullptr
                    return *this;
                }
                node_pointer root = this->pointer_->root_;
                // if we are in left subtree - root is greater
                if (root->left_ && root->left_ == this->pointer_) {
                // if (root->value_.first > temp->value_.first) {
                    this->pointer_ = root;
                    return *this;
                }
                this->pointer_ = this->pointer_->root_; 
            }
        }

        if (!this->pointer_->right_)
            return *this;
        
        this->pointer_ = this->pointer_->right_;
    
        while (true) {
            if (this->pointer_->left_)
            // if (this->pointer_)
                this->pointer_ = this->pointer_->left_;
            else
                return *this;
        }
    
        return *this;
    }

    MapIterator operator++(int) noexcept         // i++
    {
        MapIterator temporary(*this);
        // ++(this->pointer_);
        return temporary;
    }


    //                          
    //                    4
    //             2                 6
    //        1      3         5          7
    //     re  1.5    3.5   4.5  5.5        e
    MapIterator &operator--() noexcept
    {
        if (!this->pointer_->left_) {
            node_pointer temp = this->pointer_;
            while (true) {
                // find lower root
                if (!this->pointer_->root_) {
                    this->pointer_ = temp->left_;       // temp->left_ may be nullptr
                    return *this;
                }
                node_pointer root = this->pointer_->root_;
                // if we are in left subtree - root is greater
                if (root->right_ && root->right_ == this->pointer_) { // root is lower

                // if (root->value_.first < temp->value_.first) {  // TODO: sign of comparison
                    this->pointer_ = root;
                    return *this;
                }
                this->pointer_ = this->pointer_->root_; 
            }
        }

        if (!this->pointer_->left_)
            return *this;
        
        this->pointer_ = this->pointer_->left_;
    
        while(true) {
            if (this->pointer_->right_)
                this->pointer_ = this->pointer_->right_;
            else
                return *this;
        }
    
        return *this;
    }

    MapIterator operator--(int) noexcept
    {
        MapIterator temporary(*this);
        // --(this->pointer_);
        return temporary;
    }

    operator int() const = delete;

    // to enable creating iterator from const_iterator via static_cast
private:
    template <typename OtherPointer, typename OtherReference, typename OtherNode_pointer>
    explicit operator MapIterator<Map, OtherPointer, OtherReference, OtherNode_pointer>() const noexcept
    {
        return MapIterator<Map, OtherPointer, OtherReference, OtherNode_pointer>(const_cast<OtherNode_pointer>(this->pointer_));
    }
};

template <typename Map, typename Pointer = typename Map::pointer, typename Reference = typename Map::reference, typename Node_pointer = typename Map::node_pointer>
class MapReverseIterator : public MapIteratorBase<Map, Pointer, Reference, Node_pointer, typename Map::difference_type, /*typename Map::key_type,*/ typename Map::value_type>
{
  private:
    using Base = MapIteratorBase<Map, Pointer, Reference, Node_pointer, typename Map::difference_type, /*typename Map::key_type,*/
                                  typename Map::value_type>;
  public:
    using typename Base::difference_type;   // otherwise everywhere in this class 'typename Base::difference_type' instead of 'difference_type'

    // template<typename VectorType, typename PointerType, typename ReferenceType>
    // friend class VectorIterator;  // to compare const_iterator with iterator

  public:
    using Base::Base;

    // MapReverseIterator &operator++() noexcept override  // ++i
    MapReverseIterator &operator++() noexcept  // ++i
    { 
        // --(this->pointer_);
        if (!this->pointer_->left_) {
            typename Map::node_pointer temp = this->pointer_;   // очень странная херь!! проверить===============
            while (true) {
                // find lower root
                if (!this->pointer_->root_) {
                    this->pointer_ = temp->left_;       // temp->left_ may be nullptr
                    return *this;
                }
                typename Map::node_pointer root = this->pointer_->root_;
                // if we are in left subtree - root is greater
                if (root->right_ && root->right_ == this->pointer_) { // root is lower

                // if (root->value_.first < temp->value_.first) {  // TODO: sign of comparison
                    this->pointer_ = root;
                    return *this;
                }
                this->pointer_ = this->pointer_->root_; 
            }
        }

        if (!this->pointer_->left_)
            return *this;
        
        this->pointer_ = this->pointer_->left_;
    
        while(true) {
            if (this->pointer_->right_)
                this->pointer_ = this->pointer_->right_;
            else
                return *this;
        }

        return *this;
    }

    MapReverseIterator operator++(int) noexcept         // i++
    {
        MapReverseIterator temporary(*this);
        // --(this->pointer_);
        return temporary;
    }

    MapReverseIterator &operator--() noexcept
    {
        // ++(this->pointer_);
        if (!this->pointer_->right_) {
            typename Map::node_pointer temp = this->pointer_;
            while (true) {
                // find greater root
                if (!this->pointer_->root_) {
                    this->pointer_ = temp->right_;  // temp->right_ may be nullptr
                    return *this;
                }
                typename Map::node_pointer root = this->pointer_->root_;
                // if we are in left subtree - root is greater
                if (root->left_ && root->left_ == this->pointer_) {
                // if (root->value_.first > temp->value_.first) {
                    this->pointer_ = root;
                    return *this;
                }
                this->pointer_ = this->pointer_->root_; 
            }
        }

        if (!this->pointer_->right_)
            return *this;
        
        this->pointer_ = this->pointer_->right_;
    
        while(true) {
            if (this->pointer_->left_)
                this->pointer_ = this->pointer_->left_;
            else
                return *this;
        }
    
        return *this;
    }

    MapReverseIterator operator--(int) noexcept
    {
        MapReverseIterator temporary(*this);
        // ++(this->pointer_);
        return temporary;
    }

    operator int() const = delete;

private:
    template <typename OtherPointer, typename OtherReference, typename OtherNode_pointer>
    explicit operator MapIterator<Map, OtherPointer, OtherReference, OtherNode_pointer>() const noexcept
    {
        return MapIterator<Map, OtherPointer, OtherReference, OtherNode_pointer>(const_cast<OtherNode_pointer>(this->pointer_));
    }
};



// std::pair<iterator, bool> insert_recursive(Node* root, const_reference value) {
        // assert(root && "Root should always exist!");
        // root always exists
        // if (value.first < root->value_.first) {
        // if (value < root->value_) {
        // if (Comparator(value, root->value_)) {


template <typename Type>
class ComparatorSet {
public:
    bool operator()(const Type& left, const Type& right) const {
        return left < right;
    }

};

template <typename PairType>
class ComparatorMap {
public:
    bool operator()(const PairType& left, const PairType& right) const {
        return left.first < right.first;
    }

};




template <typename Value, typename Comparator> class MapBase
{
    struct Node;
  public:
    // using value_type = std::pair<const Key, Type>;
    using value_type = Value;
    // using mapped_type = Type;
    // using key_type = Key;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;

    using node_pointer = Node*;
    using const_node_pointer = const Node*;
    using comparator = Comparator;

  public:
    using iterator = MapIterator<MapBase<Value, Comparator>>;
    using const_iterator = MapIterator<MapBase<Value, Comparator>, const_pointer, const_reference, const_node_pointer>;
    using reverse_iterator = MapReverseIterator<MapBase<Value, Comparator> >;
    using const_reverse_iterator = MapReverseIterator<MapBase<Value, Comparator>, const_pointer, const_reference, const_node_pointer>;

  private:
    size_type size_;

private:
    struct Node {
        value_type value_;  // key + value

        Node* root_;
        Node* left_;
        Node* right_;

        size_type lHeight_;
        size_type rHeight_;

        Node() : value_{value_type()}, root_{nullptr}, left_{nullptr}, right_{nullptr}, lHeight_{0ull}, rHeight_{0ull} {}
        
        Node(const_reference value, Node* root = nullptr, Node* left = nullptr, Node* right = nullptr)
         : value_{value}, root_{root}, left_{left}, right_{right}, lHeight_{0ull}, rHeight_{0ull} {

        }
        Node(value_type&& value, Node* root = nullptr, Node* left = nullptr, Node* right = nullptr)
         : value_{std::move(value)}, root_{root}, left_{left}, right_{right}, lHeight_{0ull}, rHeight_{0ull} {
 
        }

    };

private:
    Node* root_;
    mutable Node end_;      // non-existing element, element after last existing element
    mutable Node rend_;     // non-existing element, element before begin
   
private:
    comparator comparator_;


public:
    MapBase() : size_{0},  end_{}, rend_{}, root_{nullptr}
    {
        // rend_.root_ = &end_;
        // end_.root_ = &rend_;
    }

    MapBase(std::initializer_list<value_type> list) : MapBase()
    {
        for (auto&& element : list)
            Insert(std::move(element));
    }

private:
    void deallocate(Node** node) {
        if((*node)->left_ && (*node)->left_ != &rend_) 
            deallocate(&(*node)->left_);
           
        if((*node)->right_ && (*node)->right_ != &end_) 
            deallocate(&(*node)->right_);

        delete *node;
        *node = nullptr;
    }

public:
    ~MapBase() {

        // std::cout << "~des map \n";

        if(root_)
            deallocate(&root_);
        
        size_ = 0;
        
    }

    size_type Size() const noexcept {
        return size_;
    } 

    size_type Height() const noexcept {
        if (!root_)
            return 0ull;
        return root_->lHeight_ > root_->rHeight_ ? root_->lHeight_ : root_->rHeight_;
    }

    // Map<int, float> m1;
    // Map<int, float>::iterator azaza = m1.begin();
    // ++azaza;
    // iterators============================================================
    iterator begin() {          // existing beginning
        return size_ ? iterator(rend_.root_) : end();
    }
    reverse_iterator rbegin() {          
        return size_ ? reverse_iterator(end_.root_) : rend();
    }
    const_iterator begin() const {    // existing beginning
        return cbegin();
    }
    const_reverse_iterator rbegin() const {    
        return rbegin();
    }
    const_iterator cbegin() const {
        return size_ ? const_iterator(rend_.root_) : cend();
    }
    iterator end() {        // non-existing end
        return iterator(&end_);
    }
    reverse_iterator rend() {        // non-existing begin
        return reverse_iterator(&rend_);
    }
    const_iterator end() const {
        return cend();
    }
    const_reverse_iterator rend() const {
        return crend();
    }
    const_iterator cend() const {
        return const_iterator(&end_);
    }
    const_reverse_iterator crend() const {
        return const_iterator(&rend_);
    }


    

private:
    //modifiers==============================================================
    //         4
    //    2           6
    //  1   3       5    7
    //                      8
    Node* create_node(Node* root, const_reference value) {
        Node* new_node = new Node(value, root);
        ++size_;
        return new_node;
    }

    void updateEnd() {
        assert(root_ && "Root should always exist!");
        Node* new_end = root_;
        while (new_end->right_ != nullptr && new_end->right_ != &end_) {
            new_end = new_end->right_;
        }
        end_.root_ = new_end;
        new_end->right_ = &end_;     // otherwise it will not stop when if (new_end->right_)
    }

    void updateReverseEnd() {
        assert(root_ && "Root should always exist!");
        Node* new_end = root_;
        while (new_end->left_ && new_end->left_ != &rend_) {
            new_end = new_end->left_;
        }
        rend_.root_ = new_end;
        new_end->left_ = &rend_;     // otherwise it will not stop when if (new_end->left_)
    }

    //       4
    bool leftLeftCase(Node* root) const noexcept {
        if (root->lHeight_ - root->rHeight_ != 2ull)
            return false;
        if (!root->left_) {
            // assert(0 && "left_ should exist!");
            return false;
        }
        if (root->left_->lHeight_ - root->left_->rHeight_ != 1ull) {
            // assert(0 && "Expected 1 difference!");
            return false;
        }
        return true;
        // return unbalanced(root) && root->left_ && root->left_->lHeight_ - root->left_->rHeight_ == 1ull;    // ? TODO: consider rHeight of root
    }
    bool rightRightCase(Node* root) const noexcept {
        if (root->rHeight_ - root->lHeight_ != 2ull)
            return false;
        if (!root->right_) {
            // assert(0 && "right_ should exist!");
            return false;
        }
        if (root->right_->rHeight_ - root->right_->lHeight_ != 1ull) {
            // assert(0 && "Expected 1 difference!");
            return false;
        } 
        return true;
        // return unbalanced(root) && root->right_ && root->right_->rHeight_ - root->right_->lHeight_ == 1ull;
    }
    bool leftRightCase(Node* root) const noexcept {
        if (root->lHeight_ - root->rHeight_ != 2ull)
            return false; 
        if (!root->left_)
            return false;
        if (root->left_->rHeight_ - root->left_->lHeight_ != 1ull) {
            // assert(0 && "Expected 1 difference!");
            return false;
        } 
        return true;
        // return unbalanced(root) && root->left_ && root->left_->rHeight_ - root->left_->lHeight_ == 1ull;
    }
    bool rightLeftCase(Node* root) const noexcept {
        if (root->rHeight_ - root->lHeight_ != 2ull)
            return false;
        if (!root->right_)
            return false;
        if (root->right_->lHeight_ - root->right_->rHeight_ != 1ull) {
            // assert(0 && "Expected 1 difference!");
            return false;
        }
        return true;
        // return unbalanced(root) && root->right_ && root->right_->lHeight_ - root->right_->rHeight_ == 1ull;
    }


    void leftLeftBalance(Node* root) {
        // assert(0 && "leftleftBalance");
        Node* root_root = root->root_;
        assert(root->left_ && "left_ should exist!");
        Node* pivot = root->left_;
        Node *b = pivot->right_;

        pivot->right_ = root;
        root->root_ = pivot;

        root->left_ = b;
        if (b)
            b->root_ = root;

        pivot->root_ = root_root;
        if (root_root) {
            if (root_root->left_ == root)
                root_root->left_ = pivot;
            else
                root_root->right_ = pivot;
        } else {
            root_ = pivot;
        }

        updateLeftHeight(root);         // order matters
        updateRightHeight(pivot);       // order matters
    }
    void rightRightBalance(Node* root) {
        // assert(0 && "rightrightBalance");
        Node* root_root = root->root_;
        assert(root->right_ && "right_ should exist!");
        Node* pivot = root->right_;
        Node *b = pivot->left_;

        pivot->left_ = root;
        root->root_ = pivot;

        root->right_ = b;
        if (b)
            b->root_ = root;

        pivot->root_ = root_root;
        if (root_root) {
            if (root_root->left_ == root)
                root_root->left_ = pivot;
            else
                root_root->right_ = pivot;
        } else {
            root_ = pivot;
        }

        updateRightHeight(root);        // order matters
        updateLeftHeight(pivot);        // order matters
    }
    void leftRightBalance(Node* root) {
        // assert(0 && "left right balance");
        if (!root->left_) {
//            std::cout << *this << '\n';
            std::cout << '{' << root->value_.first << '-' << root->lHeight_ << '-' << root->rHeight_ << "}\n";
            assert(root->left_ && "left_ should exist!");
        }
        if (!root->left_->right_) {
//            std::cout << *this << '\n';
            std::cout << '{' << root->value_.first << '-' << root->lHeight_ << '-' << root->rHeight_ << "}\n";
            assert(root->left_->right_ && "left_->right_ should exist!");
        }
        
        Node* child = root->left_;
        Node* pivot = child->right_;
        Node* c = pivot->left_;

        pivot->left_ = child;
        child->root_ = pivot;

        child->right_ = c;
        if (c)
            c->root_ = child;

        root->left_ = pivot;
        pivot->root_ = root;

        updateRightHeight(child);       // order matters
        updateLeftHeight(pivot);        // order matters

        leftLeftBalance(root);
    }
    void rightLeftBalance(Node* root) {
        // assert(0 && "right left balance");
        assert(root->right_ && "right_ should exist!");
        assert(root->right_->left_ && "right_->left_ should exist!");
        
        Node* child = root->right_;
        Node* pivot = child->left_;
        Node* c = pivot->right_;

        pivot->right_ = child;
        child->root_ = pivot;

        child->left_ = c;
        if (c)
            c->root_ = child;

        root->right_ = pivot;
        pivot->root_ = root;

        updateLeftHeight(child);        // order matters
        updateRightHeight(pivot);       // order matters

        rightRightBalance(root);
    }




    void balance(Node* root) noexcept {
        // return;
        if (leftRightCase(root))
            leftRightBalance(root);
        else if (rightLeftCase(root))
            rightLeftBalance(root);
        else if (leftLeftCase(root))
            leftLeftBalance(root);
        else if (rightRightCase(root))
            rightRightBalance(root);
        else {
            // std::cout << *this << '\n';
            std::cout << '{' << root->value_.first << '-' << root->lHeight_ << '-' << root->rHeight_ << "}\n";
            assert(0 && "Unknown type of unbalanced case!");
        }
    }

    void updateLeftHeight(Node* node) {
        assert(node && "Node should exist!");
        if (!node->left_ || node->left_ == &rend_) {
            node->lHeight_ = 0ull;
            return;
        }

        if (node->left_->lHeight_ > node->left_->rHeight_)
            node->lHeight_ = 1ull + node->left_->lHeight_;
        else
            node->lHeight_ = 1ull + node->left_->rHeight_;
    }

    void updateRightHeight(Node* node) {
        assert(node && "Node should exist!");
        if (!node->right_ || node->right_ == &end_) {
            node->rHeight_ = 0ull;
            return;
        }
        if (node->right_)
            if (node->right_->lHeight_ > node->right_->rHeight_)
                node->rHeight_ = 1ull + node->right_->lHeight_;
            else
                node->rHeight_ = 1ull + node->right_->rHeight_;
    }

    std::pair<iterator, bool> insert_recursive(Node* root, const_reference value) {
        assert(root && "Root should always exist!");
        // root always exists
        // if (value.first < root->value_.first) {
        // if (value < root->value_) {
        if (comparator_(value, root->value_)) {
            if (root->left_ && root->left_ != &rend_) {
                const auto [_, created] = insert_recursive(root->left_, value);
                if (created) {
                    updateLeftHeight(root);
                    if (unbalanced(root))
                        balance(root);
                }
                return {_, created};
            } else {
                root->left_ = create_node(root, value);
                root->lHeight_ = 1ull;
                updateReverseEnd();
                return std::make_pair(iterator(root->left_), true);
            }
        // } else if (root->value_.first < value.first) {
        } else if (comparator_(root->value_, value)) {
            if (root->right_ && root->right_ != &end_) {
                const auto [_, created] = insert_recursive(root->right_, value);
                if (created) {
                    updateRightHeight(root);
                    if (unbalanced(root))
                        balance(root);
                }
                return {_, created};
            } else {
                root->right_ = create_node(root, value);
                root->rHeight_ = 1ull;
                updateEnd();
                return std::make_pair(iterator(root->right_), true);
            }
        }
        // equal
        return std::make_pair(iterator(root), false);
    }
public:
    bool unbalanced(Node* node) const noexcept {
        const auto difference = node->lHeight_ > node->rHeight_ ? (node->lHeight_ - node->rHeight_) : (node->rHeight_ - node->lHeight_);
        assert(difference <= 2 && "Critical disbalance!");
        return difference > 1;
    }

public:
    std::pair<iterator, bool> Insert(const_reference value) {
        if (!root_) {
            root_ = create_node(nullptr, value);
            updateReverseEnd();
            updateEnd();
            return std::make_pair(iterator(root_), true);
        }
        // return insert_recursive(root_, value);
        const auto [_, created] = insert_recursive(root_, value);
        if (created) {
            updateLeftHeight(root_);
            updateRightHeight(root_);
            if (unbalanced(root_))
                balance(root_);
        }
        return {_, created};
    }
    // template< class P >
    // std::pair<iterator, bool> insert( P&& value );
// (2)	(since C++11)
// std::pair<iterator, bool> insert( value_type&& value );
// (3)	(since C++17)
// (4)	

// iterator insert( const_iterator pos, const value_type& value );
// (since C++11)
// template< class P >
// iterator insert( const_iterator pos, P&& value );
// (5)	(since C++11)
// iterator insert( const_iterator pos, value_type&& value );
// (6)	(since C++17)
// template< class InputIt >
// void insert( InputIt first, InputIt last );
// (7)	
// void insert( std::initializer_list<value_type> ilist );
// (8)	(since C++11)
// insert_return_type insert( node_type&& nh );
// (9)	(since C++17)
// iterator insert( const_iterator pos, node_type&& nh );
// (10)	(since C++17)
private:
    //modifiers==============================================================
    //                      4
    //         2                                          6
    //  1           3                  5                              7
    //   1.5                   4.5             5.5               6.5         8
    //    1.6               4.1           5.4        5.8      6.1         7.5          8.5
    //          1.8                               5.7                             8.2        9
    //       1.7                                                                8.1   
    
    iterator erase_recursive(Node* root, const_iterator pos) {
        assert(root && "root should exist!");
    
        if (comparator_(*pos, (root)->value_)) {  // pos < root->value_
            if ((root)->left_ && (root)->left_ != &rend_) {
                iterator result = erase_recursive(((root)->left_), pos);
                updateLeftHeight(root);
                if (unbalanced(root))
                    balance(root);
                return result;
            } else {
                // not found
                assert(0 && "Element should exist!");
            } 

        } else if (comparator_((root)->value_, *pos)) {
            if ((root)->right_ && (root)->right_ != &end_) {
                iterator result = erase_recursive(((root)->right_), pos);
                updateRightHeight(root);
                if (unbalanced(root))
                    balance(root);
                return result;
            } else {
                // not found
                assert(0 && "Element should exist!");
            }
        } 

        // found
        iterator result(root);
        ++result;

        Node* new_node = nullptr;
        bool no_replace = true;

        if ((root)->left_ && (root)->left_ != &rend_) {
            
            no_replace = false;
            new_node = (root)->left_;

            if (!new_node->right_) {

                new_node->right_ = (root)->right_;
                if ((root)->right_)
                    (root)->right_->root_ = new_node;
                updateRightHeight(new_node);

                if ((root)->root_) 
                    if ((root)->root_->right_ == (root))
                        (root)->root_->right_ = new_node;
                    else
                        (root)->root_->left_ = new_node;
                new_node->root_ = (root)->root_;

                if (unbalanced(new_node))
                    balance(new_node);

            } else {

                while (new_node->right_)
                    new_node = new_node->right_;
                
                new_node->right_ = (root)->right_;
                if ((root)->right_)
                    (root)->right_ = new_node;
                updateRightHeight(new_node);

                new_node->root_->right_ = new_node->left_;
                if (new_node->left_)
                    new_node->left_->root_ = new_node->root_;

                Node* temp = new_node->root_;
                while (temp != (root)->left_) {
                    updateRightHeight(temp);
                    temp = temp->root_;
                }

                new_node->left_ = (root)->left_;
                (root)->left_->root_ = new_node;
                updateLeftHeight(new_node);

                if ((root)->root_)
                    if ((root)->root_->right_ == (root))
                        (root)->root_->right_ = new_node;
                    else
                        (root)->root_->left_ = new_node;
                new_node->root_ = (root)->root_;

                if (unbalanced(new_node))
                    balance(new_node);

            }


        } else if ((root)->right_ && (root)->right_ != &end_) {

            no_replace = false;
            new_node = (root)->right_;

            if (!new_node->left_) {

                new_node->left_ = (root)->left_;
                if ((root)->left_)
                    (root)->left_->root_ = new_node;
                updateLeftHeight(new_node);
                

                if ((root)->root_) 
                    if ((root)->root_->left_ == (root))
                        (root)->root_->left_ = new_node;
                    else
                        (root)->root_->right_ = new_node;
                new_node->root_ = (root)->root_;

                if (unbalanced(new_node))
                    balance(new_node);

            } else {

                while (new_node->left_)
                    new_node = new_node->left_;
                
                new_node->left_ = (root)->left_;
                if ((root)->left_)
                    (root)->left_ = new_node;
                updateLeftHeight(new_node);

                new_node->root_->left_ = new_node->right_;
                if (new_node->right_)
                    new_node->right_->root_ = new_node->root_;

                Node* temp = new_node->root_;
                while (temp != (root)->right_) {
                    updateLeftHeight(temp);
                    temp = temp->root_;
                }

                new_node->right_ = (root)->right_;
                (root)->right_->root_ = new_node;
                updateRightHeight(new_node);

                if ((root)->root_)
                    if ((root)->root_->left_ == (root))
                        (root)->root_->left_ = new_node;
                    else
                        (root)->root_->right_ = new_node;
                new_node->root_ = (root)->root_;

                if (unbalanced(new_node))
                    balance(new_node);

            }

            
        }
    
        if (no_replace && root->root_)
            if (root->root_->left_ == root)
                root->root_->left_ = nullptr;
            else
                root->root_->right_ = nullptr;

        delete root;
        root = nullptr;
        --size_;

        if (new_node && !new_node->root_)
            root_ = new_node;

        if (root_) {
            updateEnd();
            updateReverseEnd();
        }

        return result;
    }

public:
    iterator Erase(const_iterator pos) {
        if (!root_)
            assert(0 && "Trying to erase from empty tree!");
        return erase_recursive(root_, pos);
    }




protected:
    const_iterator find_recursive(Node* root, const_reference value, bool lowerBound = false, bool upperBound = false, Node* bound = nullptr) const
    {
        assert(root && "root should exist!");
    
        if (comparator_(value, root->value_)) {  // key < root->value_
            if (root->left_ && root->left_ != &rend_) {
                bound = root;
                return find_recursive(root->left_, value, lowerBound, upperBound, bound);
            } else {
                // not found
                // or return root if lowerBound or upperBound
                if (lowerBound || upperBound)
                    return const_iterator(root);
                return end();
            }
        } else if (comparator_(root->value_, value)) {
            if (root->right_ && root->right_ != &end_) {
                return find_recursive(root->right_, value, lowerBound, upperBound, bound);
            } else {
                // not found
                // or if lowerBound or upperBound return the last time we took left subtree
                if (lowerBound || upperBound)
                    return const_iterator(bound);
                return end();
            }
        } else {
            // found
            // or return next if upperBound
            if (upperBound)
                return ++const_iterator(root);
            return const_iterator(root);
        }
    }

protected:
    iterator find(const_reference value) {
        if (!root_)
            return end();
        return static_cast<iterator>(find_recursive(root_, value));
    }
    const_iterator find(const_reference value) const {
        if (!root_)
            return end();
        return find_recursive(root_, value);    
    }

    // template< class K > iterator find( const K& x );
    // template< class K > const_iterator find( const K& x ) const;

    bool contains(const_reference value) const {
        return find(value) != end();
    }

    iterator lower_bound(const_reference value) {
        if (!root_)
            return end();
        return static_cast<iterator>(find_recursive(root_, value, true, false, &end_)); 
    }

    const_iterator lower_bound(const_reference value) const {
        if (!root_)
            return end();
        return find_recursive(root_, value, true, false, &end_); 
    }

    iterator upper_bound(const_reference value) {
        if (!root_)
            return end();
        return static_cast<iterator>(find_recursive(root_, value, false, true, &end_)); 
    }

    const_iterator upper_bound(const_reference value) const {
        if (!root_)
            return end();
        return find_recursive(root_, value, false, true, &end_); 
    }

    // template< class K > bool contains( const K& x ) const;


    std::pair<iterator,iterator> equal_range(const_reference value) {
        if (!root_)
            return std::make_pair(end(), end());

        iterator lower = lower_bound(value);
        iterator upper = upper_bound(value);
        return std::make_pair(lower, upper);
    }
    std::pair<const_iterator,const_iterator> equal_range(const_reference value) const {
        if (!root_)
            return std::make_pair(end(), end());

        const_iterator lower = static_cast<const_iterator>(lower_bound(value));
        const_iterator upper = static_cast<const_iterator>(upper_bound(value));
        return std::make_pair(lower, upper);
    }
    //template< class K >
    //std::pair<iterator,iterator> equal_range( const K& x );
    //template< class K >
    //std::pair<const_iterator,const_iterator> equal_range( const K& x ) const;

};



template <typename Type, typename Comparator = ComparatorSet<Type>>
class Set : public MapBase<Type, Comparator> {

};

template <typename Type, typename Comparator = ComparatorSet<Type>>
class MultiSet : public MapBase<List<Type>, Comparator> {

};

template <typename Key, typename Type, typename Comparator = ComparatorMap<std::pair<const Key, Type>>>
class Map : public MapBase<std::pair<const Key, Type>, Comparator> {

public:
    using Base = MapBase<std::pair<const Key, Type>, Comparator>;

public:
    using value_type        = typename Base::value_type;
    using mapped_type       = Type;
    using key_type          = Key;
    using size_type         = typename Base::size_type;
    using difference_type   = typename Base::difference_type;
    using reference         = typename Base::reference;
    using const_reference   = typename Base::const_reference;
    using pointer           = typename Base::pointer;
    using const_pointer     = typename Base::const_pointer;

public:
    using iterator                  = typename Base::iterator;
    using const_iterator            = typename Base::const_iterator;
    using reverse_iterator          = typename Base::reverse_iterator;
    using const_reverse_iterator    = typename Base::const_reverse_iterator;

public:
    using Base::Base;


    
    
    
    iterator Find(const key_type& key) {
        return this->find(value_type(key, mapped_type()));
    }
    const_iterator Find(const key_type& key) const {
        return this->find(value_type(key, mapped_type()));
    }
    bool Contains(const key_type& key) const {
        return this->contains(value_type(key, mapped_type()));
    }
    iterator Lower_bound(const Key& key) {
        return this->lower_bound(value_type(key, mapped_type()));
    }
    const_iterator Lower_bound(const Key& key) const {
        return this->lower_bound(value_type(key, mapped_type()));
    }
    iterator Upper_bound(const Key& key) {
        return this->upper_bound(value_type(key, mapped_type()));
    }
    const_iterator Upper_bound(const Key& key) const {
        return this->upper_bound(value_type(key, mapped_type()));
    }

    std::pair<iterator,iterator> Equal_range(const Key& key) {
        return this->equal_range(value_type(key, mapped_type()));
    }
    std::pair<const_iterator,const_iterator> Equal_range(const Key& key) const {
        return this->equal_range(value_type(key, mapped_type()));
    }
};


template <typename Key, typename Type>
bool operator==(const s21::Map<Key, Type>& left, const s21::Map<Key, Type>& right) {
    if(left.Size() != right.Size())
        return false;
    
    auto it_left = left.cbegin();
    auto it_right = right.cbegin();


    while(it_left != left.cend()) {
        // assert(0 && "Azaza");
        if(*it_left != *it_right) 
            return false;
        
        it_left++;
        it_right++;
        
    }

    return true;   
}


// template <typename Type> bool operator==(const Vector<Type> &left, const Vector<Type> &right)
// {
//     if (left.Size() != right.Size())
//         return false;

//     auto left_iterator = left.cbegin();
//     auto right_iterator = right.cbegin();

//     while (left_iterator != left.cend())
//     {
//         if (*left_iterator != *right_iterator)
//             return false;
//         ++left_iterator;
//         ++right_iterator;
//     }

//     return true;
// }

// template <typename Type> bool operator!=(const Vector<Type> &left, const Vector<Type> &right)
// {
//     return !(left == right);
// }

} // namespace s21


// int main() {
//     s21::Map<int, double> m;
//     m.Insert(std::make_pair(1, 1.1));
//     m.Insert(std::make_pair(1, 4.4));
//     m.Insert(std::make_pair(2, 2.2));
//     m.Insert(std::make_pair(3, 3.3));
    
//     std::cout << m;
//     return 0;
// }
#endif //  _S21_MAP_H_
