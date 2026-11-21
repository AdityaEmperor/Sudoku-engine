#ifndef CUSTOM_UNORDERED_MAP_H
#define CUSTOM_UNORDERED_MAP_H

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <initializer_list>
#include "LinkedList.h"


template <typename K, typename V,
          typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>>
class CustomUnorderedMap {
private:
    using Bucket = LinkedList<K, V>;
    Bucket* buckets_;
    std::size_t bucketCount_;
    std::size_t size_;
    float maxLoadFactor_;
    Hash hash_;
    KeyEqual equal_;

    std::size_t indexFor(const K& key, std::size_t count) const {
        return hash_(key) % count;
    }

    void rehashIfNeeded() {
        if (load_factor() > maxLoadFactor_) {
            std::size_t newCount = bucketCount_ ? bucketCount_ * 2 : 8;
            while (static_cast<float>(size_) / newCount > maxLoadFactor_)
                newCount *= 2;
            rehash(newCount);
        }
    }

public:
    using key_type = K;
    using mapped_type = V;
    using size_type = std::size_t;

    class iterator {
        friend class CustomUnorderedMap;
        CustomUnorderedMap* owner_;
        std::size_t bucket_;
        typename Bucket::Node* node_;

        iterator(CustomUnorderedMap* owner, std::size_t b,
                 typename Bucket::Node* n)
            : owner_(owner), bucket_(b), node_(n) {}

    public:
        iterator() : owner_(nullptr), bucket_(0), node_(nullptr) {}

        std::pair<const K&, V&> operator*() const {
            return {node_->key, node_->value};
        }

        typename Bucket::Node* operator->() const { return node_; }

        iterator& operator++() {
            if (!owner_ || !node_) return *this;
            if (node_->next) {
                node_ = node_->next;
                return *this;
            }
            ++bucket_;
            while (bucket_ < owner_->bucketCount_ &&
                   owner_->buckets_[bucket_].empty())
                ++bucket_;
            node_ = (bucket_ < owner_->bucketCount_)
                  ? owner_->buckets_[bucket_].head() : nullptr;
            return *this;
        }

        iterator operator++(int) {
            iterator old = *this;
            ++(*this);
            return old;
        }

        bool operator==(const iterator& other) const {
            return owner_ == other.owner_ && node_ == other.node_;
        }
        bool operator!=(const iterator& other) const { return !(*this == other); }
    };

    class const_iterator {
        friend class CustomUnorderedMap;
        const CustomUnorderedMap* owner_;
        std::size_t bucket_;
        const typename Bucket::Node* node_;

        const_iterator(const CustomUnorderedMap* owner, std::size_t b,
                       const typename Bucket::Node* n)
            : owner_(owner), bucket_(b), node_(n) {}

    public:
        const_iterator() : owner_(nullptr), bucket_(0), node_(nullptr) {}
        std::pair<const K&, const V&> operator*() const {
            return {node_->key, node_->value};
        }
        const typename Bucket::Node* operator->() const { return node_; }

        const_iterator& operator++() {
            if (!owner_ || !node_) return *this;
            if (node_->next) {
                node_ = node_->next;
                return *this;
            }
            ++bucket_;
            while (bucket_ < owner_->bucketCount_ &&
                   owner_->buckets_[bucket_].empty())
                ++bucket_;
            node_ = (bucket_ < owner_->bucketCount_)
                  ? owner_->buckets_[bucket_].head() : nullptr;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator old = *this;
            ++(*this);
            return old;
        }
        bool operator==(const const_iterator& other) const {
            return owner_ == other.owner_ && node_ == other.node_;
        }
        bool operator!=(const const_iterator& other) const { return !(*this == other); }
    };

    explicit CustomUnorderedMap(std::size_t bucketCount = 8,
                           const Hash& hash = Hash(),
                           const KeyEqual& equal = KeyEqual())
        : buckets_(new Bucket[bucketCount ? bucketCount : 8]),
          bucketCount_(bucketCount ? bucketCount : 8),
          size_(0), maxLoadFactor_(1.0f), hash_(hash), equal_(equal) {}

    CustomUnorderedMap(std::initializer_list<std::pair<K, V>> items)
        : CustomUnorderedMap() {
        for (const auto& item : items) insert(item);
    }

    ~CustomUnorderedMap() { delete[] buckets_; }

    CustomUnorderedMap(const CustomUnorderedMap& other)
        : buckets_(new Bucket[other.bucketCount_]),
          bucketCount_(other.bucketCount_), size_(0),
          maxLoadFactor_(other.maxLoadFactor_),
          hash_(other.hash_), equal_(other.equal_) {
        for (const auto& item : other) insert({item.first, item.second});
    }

    CustomUnorderedMap& operator=(const CustomUnorderedMap& other) {
        if (this == &other) return *this;
        CustomUnorderedMap copy(other);
        swap(copy);
        return *this;
    }

    CustomUnorderedMap(CustomUnorderedMap&& other)
        : buckets_(other.buckets_), bucketCount_(other.bucketCount_),
          size_(other.size_), maxLoadFactor_(other.maxLoadFactor_),
          hash_(std::move(other.hash_)), equal_(std::move(other.equal_)) {
        other.buckets_ = new Bucket[8];
        other.bucketCount_ = 8;
        other.size_ = 0;
    }

    CustomUnorderedMap& operator=(CustomUnorderedMap&& other) {
        if (this == &other) return *this;
        delete[] buckets_;
        buckets_ = other.buckets_;
        bucketCount_ = other.bucketCount_;
        size_ = other.size_;
        maxLoadFactor_ = other.maxLoadFactor_;
        hash_ = std::move(other.hash_);
        equal_ = std::move(other.equal_);
        other.buckets_ = new Bucket[8];
        other.bucketCount_ = 8;
        other.size_ = 0;
        return *this;
    }

    void swap(CustomUnorderedMap& other) {
        std::swap(buckets_, other.buckets_);
        std::swap(bucketCount_, other.bucketCount_);
        std::swap(size_, other.size_);
        std::swap(maxLoadFactor_, other.maxLoadFactor_);
        std::swap(hash_, other.hash_);
        std::swap(equal_, other.equal_);
    }

    // operator[] inserts a missing key with a default-constructed value.
    V& operator[](const K& key) {
        Bucket& bucket = buckets_[indexFor(key, bucketCount_)];
        if (V* value = bucket.findValue(key)) return *value;
        bucket.insertOrAssign(key, V{});
        ++size_;
        rehashIfNeeded();
        return *buckets_[indexFor(key, bucketCount_)].findValue(key);
    }

    V& at(const K& key) {
        V* value = buckets_[indexFor(key, bucketCount_)].findValue(key);
        if (!value) throw std::out_of_range("unordered_map::at: key not found");
        return *value;
    }

    const V& at(const K& key) const {
        const V* value = buckets_[indexFor(key, bucketCount_)].findValue(key);
        if (!value) throw std::out_of_range("unordered_map::at: key not found");
        return *value;
    }

    iterator find(const K& key) {
        std::size_t b = indexFor(key, bucketCount_);
        auto* node = buckets_[b].findNode(key);
        return node ? iterator(this, b, node) : end();
    }

    const_iterator find(const K& key) const {
        std::size_t b = indexFor(key, bucketCount_);
        auto* node = buckets_[b].findNode(key);
        return node ? const_iterator(this, b, node) : cend();
    }

    iterator end() { return iterator(this, bucketCount_, nullptr); }
    const_iterator end() const { return cend(); }
    const_iterator cend() const { return const_iterator(this, bucketCount_, nullptr); }

    iterator begin() {
        for (std::size_t b = 0; b < bucketCount_; ++b)
            if (!buckets_[b].empty()) return iterator(this, b, buckets_[b].head());
        return end();
    }

    const_iterator begin() const { return cbegin(); }
    const_iterator cbegin() const {
        for (std::size_t b = 0; b < bucketCount_; ++b)
            if (!buckets_[b].empty())
                return const_iterator(this, b, buckets_[b].head());
        return cend();
    }

    // Inserts only if key is absent. Returns {iterator, inserted}.
    std::pair<iterator, bool> insert(const std::pair<K, V>& item) {
        std::size_t b = indexFor(item.first, bucketCount_);
        if (auto* existing = buckets_[b].findNode(item.first))
            return {iterator(this, b, existing), false};
        buckets_[b].insertOrAssign(item.first, item.second);
        ++size_;
        rehashIfNeeded();
        return {find(item.first), true};
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        std::pair<K, V> item(std::forward<Args>(args)...);
        return insert(item);
    }

    // Updates an existing key or inserts a new one.
    std::pair<iterator, bool> insert_or_assign(const K& key, const V& value) {
        std::size_t b = indexFor(key, bucketCount_);
        if (auto* existing = buckets_[b].findNode(key)) {
            existing->value = value;
            return {iterator(this, b, existing), false};
        }
        return insert({key, value});
    }

    std::size_t erase(const K& key) {
        std::size_t b = indexFor(key, bucketCount_);
        if (buckets_[b].erase(key)) {
            --size_;
            return 1;
        }
        return 0;
    }

    iterator erase(iterator pos) {
        if (pos == end()) return end();
        K key = pos.node_->key;
        ++pos;
        erase(key);
        return pos;
    }

    void clear() noexcept {
        for (std::size_t i = 0; i < bucketCount_; ++i) buckets_[i].clear();
        size_ = 0;
    }

    std::size_t count(const K& key) const {
        return find(key) == cend() ? 0 : 1;
    }

    bool contains(const K& key) const { return count(key) != 0; }

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    std::size_t bucket_count() const noexcept { return bucketCount_; }
    float load_factor() const noexcept {
        return bucketCount_ ? static_cast<float>(size_) / bucketCount_ : 0.0f;
    }
    float max_load_factor() const noexcept { return maxLoadFactor_; }

    void max_load_factor(float value) {
        if (value <= 0.0f) throw std::invalid_argument("max_load_factor must be positive");
        maxLoadFactor_ = value;
        rehashIfNeeded();
    }

    void rehash(std::size_t newCount) {
        if (newCount < 1) newCount = 1;
        if (static_cast<float>(size_) / newCount > maxLoadFactor_)
            newCount = static_cast<std::size_t>(size_ / maxLoadFactor_) + 1;

        Bucket* fresh = new Bucket[newCount];
        for (std::size_t i = 0; i < bucketCount_; ++i) {
            auto* node = buckets_[i].head();
            while (node) {
                fresh[indexFor(node->key, newCount)].insertOrAssign(node->key, node->value);
                node = node->next;
            }
        }
        delete[] buckets_;
        buckets_ = fresh;
        bucketCount_ = newCount;
    }

    void reserve(std::size_t elementCount) {
        std::size_t needed = static_cast<std::size_t>(elementCount / maxLoadFactor_) + 1;
        if (needed > bucketCount_) rehash(needed);
    }
};

// Compatibility shim: existing solver code can keep writing unordered_map<K,V>.
// Include this header after the project's standard-library headers.
#define unordered_map CustomUnorderedMap

#endif
