#ifndef HASH_MAP_HPP
#define HASH_MAP_HPP

#include <string>
#include "mvector.hpp"
#include "mstack.hpp"

namespace hashmap_constants {
    const int max_bucket_size = 4;
    const int initial_size = 8;
    const int load_factor = 85;
}

template <typename T, typename V>
class entry {
public:
    entry();
    entry(T &&key, V &&value);
    entry(const entry &other);

    ~entry()=default;

    entry &operator=(const entry &other); 

    bool operator==(const entry &other); 

    bool operator==(const T &key); 

    const T &get_key();

    V &get_value();
private:
    T _key;
    V _value;
};

template <typename T, typename V>
class hash_map {
public:
    explicit hash_map(size_t initial_size = hashmap_constants::initial_size);
    hash_map(const hash_map &other);
    hash_map(hash_map &&other);

    ~hash_map();

    void insert(T ent, V value);

    // If we're searching for plain trie nodes, we use bsearch_children
    V &get(const T &key) const;

    void delete_word(const T &key);

    size_t get_num_items() const;

    size_t size() const;

    bool empty() const;
private:
    // We do not keep a <current size> variable, since we can always calculate it as the
    // size of the table at the start of each run + p (the next bucket to be split)
    size_t _size;
    size_t _num_items;
    size_t _p;

    mstd::vector<entry<T, V>> **_entries;

    void _resize();

    int _calculate_load() const;

};

template <typename T, typename V>
entry<T, V>::entry() { }

template <typename T, typename V>
entry<T, V>::entry(T &&key, V &&value) : _key(key), _value(value) { }

template <typename T, typename V>
entry<T, V>::entry(const entry<T, V> &other) : _key(other._key), _value(other._value) { }

template <typename T, typename V>
const T &entry<T, V>::get_key() {
    return _key;
}

template <typename T, typename V>
V &entry<T, V>::get_value() {
    return _value;
}

template <typename T, typename V>
entry<T, V> &entry<T, V>::operator=(const entry<T, V> &other) {
    _key = other._key;
    _value = other._value;
    return *this;
}

template <typename T, typename V>
bool entry<T, V>::operator==(const entry<T, V> &other) {
    return _key == other._key && _value = other._value;
}

template <typename T, typename V>
bool entry<T, V>::operator==(const T &key) {
    return _key == key;
}



template <typename T, typename V>
hash_map<T, V>::hash_map(size_t initial_size) : _size(initial_size),
                                                   _num_items(0),
                                                   _p(0)
                                                        {
    _entries = new mstd::vector<entry<T, V>>*[_size];
    for (size_t i = 0; i < initial_size; i++) {
        _entries[i] = new mstd::vector<entry<T, V>>(hashmap_constants::max_bucket_size);
    }
}

template <typename T, typename V>
hash_map<T, V>::~hash_map() {
    for (size_t i = 0; i < _size + _p; i++) {
        delete _entries[i];
    }
    delete[] _entries;
}

template <typename T, typename V>
void hash_map<T, V>::insert(T key, V value) {

    int hash = key.hash();
    size_t index = hash % _size;
    if (index < _p) {
        index = hash % (2 * _size);
    }

    entry<T, V> new_entry(std::move(key), std::move(value));

    for (int i = 0; i < _entries[index]->size(); i++) {
        if (_entries[index]->at(i) == new_entry.get_key()) {
            throw std::runtime_error("Key already exists");
        }
    }
    _entries[index]->push(new_entry);

    int load = _calculate_load();

    bool split = false;

    if (load > hashmap_constants::load_factor) {
        split = true;
        // Load factor has been reached. Split at _p

        mstd::vector<entry<T, V>> *tmp_storage = _entries[_p]; // Keep a temporary storage

        _resize(); // Allocate one more bucket at the end of the table

        _entries[_p] = new mstd::vector<entry<T, V>>(hashmap_constants::max_bucket_size); // And create a new bucket

        // Rehash each element in tmp_storage to either _p or _size + _p
        for (int i = 0; i < (int) tmp_storage->size(); i++) {
            entry<T, V> &tmp = tmp_storage->at(i);
            int hash = tmp.get_key().hash();

            int index = hash % (2 * _size);
            _entries[index]->push(tmp);
        }

        // Delete the temporary object
        delete tmp_storage;

        // Increase p after the split operation
        if ((++_p) == _size) {
            // Next hashing phase
            // If the hash table has doubled in size, reset p and double the size
            _size *= 2;
            _p = 0;
        }
    }

    _num_items++;
}

template <typename T, typename V>
V &hash_map<T, V>::get(const T &key) const {

    int hash = key.hash();
    
    size_t index = hash % _size;
    if (index < _p) {
        index = hash % (_size * 2);
    }

    int child_index;
    mstd::vector<entry<T, V>> *v = _entries[index];
    for (size_t i = 0; i < v->size(); i++) {
        if (v->at(i) == key) {
            return v->at((size_t) i).get_value();
        }
    }

    throw std::runtime_error("Key does not exist");

}

template <typename T, typename V>
size_t hash_map<T, V>::get_num_items() const {
    return _num_items;
}

template <typename T, typename V>
size_t hash_map<T, V>::size() const {
    return _size + _p;
}

template <typename T, typename V>
bool hash_map<T, V>::empty() const {
    return _num_items == 0;
}

template <typename T, typename V>
void hash_map<T, V>::_resize() {
    auto tmp = new mstd::vector<entry<T, V>>*[_size + _p + 1];
    for (size_t i = 0; i < _size + _p; i++) {
        tmp[i] = _entries[i];
    }

    // Initialise the newly created bucket
    tmp[_size + _p] = new mstd::vector<entry<T, V>>(hashmap_constants::max_bucket_size);

    delete[] _entries;
    _entries = tmp;
}

template <typename T, typename V>
int hash_map<T, V>::_calculate_load() const {
    return (int) ((_num_items + 1) / (double) ((_size + _p) * hashmap_constants::max_bucket_size) * 100);
}


#endif //HASH_MAP_HPP
