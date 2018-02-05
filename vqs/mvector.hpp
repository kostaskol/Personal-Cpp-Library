#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include <iostream>
#include <cstdlib>
#include <sstream>

// Simple resizable array template class
// that includes some of std::vector's basic operations
namespace mstd {
    template <typename T>
    class vector {

    public:
        explicit vector(size_t capacity = 1);

        vector(const vector &other);
        vector(T *arr, size_t arr_size);
        vector(const vector &other, size_t start, size_t end);
        vector(vector &&other) noexcept;

        ~vector();

        void push(const T &ent); 
        void push(T &&ent); 
        T *m_push(T &ent); 
        T *m_insert_at(int index, T &ent); 

        T &back(); 
        void pop_back(); 

        void shrink_to_size(); 

        bool in(const T &ent) const; 

        T &at(size_t index) const; 

        T *at_p(size_t index); 

        void clear(size_t new_cap = 1); 

        void set_at(size_t index, const T &ent); 

        // iterators. allows for foreach loops
        // (for (int i : vec))
        T *begin(); 

        T *end(); 

        void remove_at(size_t index);

        size_t capacity();

        size_t size() const; 

        T &operator[](size_t index) const; 

        // Copy assignment operator
        vector &operator=(const vector &other); 

        // Move assignment operator
        vector &operator=(vector &&other) noexcept; 

        template <typename V>
        friend void _swap_vectors(vector<V> &v1, vector<V> &v2); 

    private:
        size_t _size{};
        size_t _capacity;
        T *_entries;

        void _enlarge(); 
    };
}

template <typename T>
mstd::vector<T>::vector(size_t capacity)
        : _size(0), _capacity(capacity) {
    _entries = new T[_capacity];
}

template <typename T>
mstd::vector<T>::vector(const mstd::vector<T> &other)
        : _size(other._size),
        _capacity(other._capacity) {
    _entries = new T[_capacity];
    std::copy(other._entries, other._entries + other._size, _entries);
}

template <typename T>
mstd::vector<T>::vector(T *arr, size_t arr_size)
        : _capacity(arr_size), _size(arr_size) {
    _entries = new T[_capacity];
    std::copy(arr, arr + arr_size, _entries);
}

template <typename T>
mstd::vector<T>::vector(const mstd::vector<T> &other, size_t start, size_t end) {
    if (start < 0 || end > other._size) return;
    _capacity = other._capacity;
    _size = (size_t) end - start;
    _entries = new T[_capacity];
    std::copy(other._entries + start, other._entries + end, _entries);
}

template <typename T>
mstd::vector<T>::vector(vector<T> &&other) noexcept : vector() {
    // other object is swapped with this (which has been instantiated and is thus safe to delete)
    _swap_vectors(*this, other);
}

template <typename T>
mstd::vector<T>::~vector() {
    delete[] _entries;
}

template <typename T>
void mstd::vector<T>::push(const T &ent) {
    if (_size + 1 > _capacity) {
        _enlarge();
    }

    _entries[_size++] = T(ent);
}

template <typename T>
void mstd::vector<T>::push(T &&ent) {
    if (_size + 1 > _capacity) {
        _enlarge();
    }

    _entries[_size++] = ent;
}

template <typename T>
T *mstd::vector<T>::m_push(T &ent) {
    if (_size + 1 > _capacity) _enlarge();

    _entries[_size] = std::move(ent);
    return &_entries[_size++];
}

template <typename T>
T *mstd::vector<T>::m_insert_at(int index, T &ent) {
    // Changed: Throws an out_of_range exception if
    // an entry is about to be placed over the size of the vector
    // (because it would cause too many other problems)
    if (index > _size + 1) {
        throw std::out_of_range("Index out of range");
    }

    if ((_size == 0) || (index == _size)) {
        return m_push(ent);
    }

    if (_size + 1 >= _capacity) _enlarge();

    for (size_t i = _size - 1; (int) i >= index; i--) {
        _entries[i + 1] = std::move(_entries[i]);
    }

    _entries[index] = std::move(ent);
    _size++;
    return &_entries[index];
}


template <typename T>
T &mstd::vector<T>::back() {
    if (_size - 1 < 0) {
        throw std::out_of_range("Requesting back of empty vector");
    }
    return _entries[_size - 1];
}

template <typename T>
void mstd::vector<T>::pop_back() {
    if (_size == 0) {
        return;
    }
    
    _size--;
}

template <typename T>
void mstd::vector<T>::shrink_to_size() {
    auto *tmp = new T[_size];
    for (size_t i = 0; i < _size; i++) {
        tmp[i] = _entries[i];
    }
    delete[] _entries;
    _entries = tmp;
    _capacity = _size;
}

template <typename T>
bool mstd::vector<T>::in(const T &ent) const {
    for (size_t i = 0; i < _size; i++) {
        if (_entries[i] == ent) {
            return true;
        }
    }
    return false;
}

template <typename T>
T &mstd::vector<T>::at(size_t index) const {
    if (index >= (int) _size) {
        throw std::out_of_range("Index out of range:");
    }

    return _entries[index];
}

template <typename T>
T *mstd::vector<T>::at_p(size_t index) {
    if (index >= _size) {
        throw std::out_of_range("Index out of range");
    }

    return &_entries[index];
}

template <typename T>
void mstd::vector<T>::clear(size_t new_cap) {
    delete[] _entries;
    _size = 0;

    _capacity = new_cap;
    _entries = new T[_capacity];
}

template <typename T>
void mstd::vector<T>::set_at(size_t index, const T &ent) {
    if (index < 0 || index >= _size) {
        throw std::out_of_range("Bad index: " + std::to_string(index));
    }

    _entries[index] = T(ent);
}


// Iterators. Allows for foreach loops
// (for (int i : vec))
template <typename T>
T *mstd::vector<T>::begin() {
    return &_entries[0];
}

template <typename T>
T *mstd::vector<T>::end() {
    return &_entries[_size];
}

template <typename T>
void mstd::vector<T>::remove_at(size_t index) {
    if (index >= _size || index < 0) {
        throw std::out_of_range("Bad index: " + std::to_string(index));
    }

    for (size_t i = index + 1; i < _size; i++) {
        _entries[i - 1] = std::move(_entries[i]);
    }

    _size--;

}
template <typename T>
size_t mstd::vector<T>::capacity() {
    return _capacity;
}
template <typename T>
size_t mstd::vector<T>::size() const { return _size; }

template <typename T>
T &mstd::vector<T>::operator[](size_t index) const {
    return at(index);
}

template <typename T>
mstd::vector<T> &mstd::vector<T>::operator=(const vector &other) {
    vector tmp(other);
    _swap_vectors(*this, tmp);

    return *this;
}

template <typename T>
mstd::vector<T> &mstd::vector<T>::operator=(vector &&other) noexcept {
    _swap_vectors(*this, other);
}

template <typename T>
void mstd::vector<T>::_enlarge() {
    auto *tmp = new T[_capacity << 1];
    for (size_t i = 0; i < _size; i++) {
        tmp[i] = std::move(_entries[i]);
    }

    delete[] _entries;
    _entries = tmp;

    _capacity <<= 1;
}

template <typename T>
void _swap_vectors(mstd::vector<T> &v1, mstd::vector<T> &v2) {
    using std::swap;
    swap(v1._size, v2._size);
    swap(v1._capacity, v2._capacity);
    swap(v1._entries, v2._entries);
}



#endif //MY_VECTOR_H
