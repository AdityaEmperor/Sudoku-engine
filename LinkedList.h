#ifndef CUSTOM_LINKED_LIST_H
#define CUSTOM_LINKED_LIST_H

#include <cstddef>
#include <utility>

// A singly linked list of key-value pairs.
// This is the bucket storage used by the custom hash table.
template <typename K, typename V>
class LinkedList {
public:
    struct Node {
        const K key;
        V value;
        Node* next;

        Node(const K& k, const V& v, Node* n = nullptr)
            : key(k), value(v), next(n) {}
    };

private:
    Node* head_;
    std::size_t size_;

public:
    LinkedList() : head_(nullptr), size_(0) {}
    ~LinkedList() { clear(); }

    LinkedList(const LinkedList& other) : head_(nullptr), size_(0) {
        Node* tail = nullptr;
        for (Node* cur = other.head_; cur; cur = cur->next) {
            Node* fresh = new Node(cur->key, cur->value);
            if (!head_) head_ = fresh;
            else tail->next = fresh;
            tail = fresh;
            ++size_;
        }
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this == &other) return *this;
        LinkedList copy(other);
        swap(copy);
        return *this;
    }

    LinkedList(LinkedList&& other) noexcept
        : head_(other.head_), size_(other.size_) {
        other.head_ = nullptr;
        other.size_ = 0;
    }

    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this == &other) return *this;
        clear();
        head_ = other.head_;
        size_ = other.size_;
        other.head_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    void swap(LinkedList& other) noexcept {
        Node* h = head_; head_ = other.head_; other.head_ = h;
        std::size_t s = size_; size_ = other.size_; other.size_ = s;
    }

    // Inserts a new key or updates the value if the key already exists.
    // Returns true only when a new node was inserted.
    bool insertOrAssign(const K& key, const V& value) {
        Node* cur = head_;
        while (cur) {
            if (cur->key == key) {
                cur->value = value;
                return false;
            }
            cur = cur->next;
        }
        head_ = new Node(key, value, head_);
        ++size_;
        return true;
    }

    // Returns a pointer to the value, or nullptr if the key is absent.
    V* findValue(const K& key) {
        for (Node* cur = head_; cur; cur = cur->next)
            if (cur->key == key) return &cur->value;
        return nullptr;
    }

    const V* findValue(const K& key) const {
        for (const Node* cur = head_; cur; cur = cur->next)
            if (cur->key == key) return &cur->value;
        return nullptr;
    }

    Node* findNode(const K& key) {
        for (Node* cur = head_; cur; cur = cur->next)
            if (cur->key == key) return cur;
        return nullptr;
    }

    const Node* findNode(const K& key) const {
        for (const Node* cur = head_; cur; cur = cur->next)
            if (cur->key == key) return cur;
        return nullptr;
    }

    bool erase(const K& key) {
        Node* cur = head_;
        Node* previous = nullptr;
        while (cur) {
            if (cur->key == key) {
                if (previous) previous->next = cur->next;
                else head_ = cur->next;
                delete cur;
                --size_;
                return true;
            }
            previous = cur;
            cur = cur->next;
        }
        return false;
    }

    void clear() noexcept {
        while (head_) {
            Node* next = head_->next;
            delete head_;
            head_ = next;
        }
        size_ = 0;
    }

    Node* head() noexcept { return head_; }
    const Node* head() const noexcept { return head_; }
    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
};

#endif
