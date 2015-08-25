#ifndef ASSIST_MAP_ADAPTER_HPP
#define ASSIST_MAP_ADAPTER_HPP

/*
 * assist/map_adapter.hpp
 *
 * Copyright (c) 2006 Scott McMurray
 *
 * Licensed under the Open Software License version 3.0
 * ( See http://opensource.org/licenses/osl-3.0.php )
 *
 */

#include "set_adapter.hpp"

namespace assist {

template < typename base_type,
            typename CMP = std::less<typename base_type::value_type
                                                         ::first_type> >
class map_adapter {
  public:
    // Types
    typedef typename base_type::value_type value_type;
    typedef typename value_type::first_type key_type;
    typedef typename value_type::second_type mapped_type;
    typedef CMP key_compare;
    struct value_compare {
        key_compare key_comparator;
        value_compare(key_compare kcmp) : key_comparator(kcmp) {}
        bool operator()(value_type const &lhs,
                         value_type const &rhs) const {
            return key_comparator(lhs.first, rhs.first);
        }
        bool operator()(key_type const &lhs,
                         value_type const &rhs) const {
            return key_comparator(lhs, rhs.first);
        }
        bool operator()(value_type const &lhs,
                         key_type const &rhs) const {
            return key_comparator(lhs.first, rhs);
        }
    };

  private:
    typedef set_adapter< base_type, value_compare > set_type; 
//    typedef std::set< value_type, value_compare, typename base_type::allocator_type > set_type; 
    set_type c;
    // c holds the comparator

  public:
    // Types
    typedef typename set_type::allocator_type allocator_type;
    typedef typename set_type::reference reference;
    typedef typename set_type::const_reference const_reference;
    typedef typename set_type::size_type size_type;
    typedef typename set_type::difference_type difference_type;
    typedef typename set_type::pointer pointer;
    typedef typename set_type::const_pointer const_pointer;
    typedef typename set_type::iterator iterator;
    typedef typename set_type::const_iterator const_iterator;
    typedef typename set_type::reverse_iterator reverse_iterator;
    typedef typename set_type::const_reverse_iterator const_reverse_iterator;
//    typedef typename set_type:: ;

    // Construct/Copy/Destroy
    explicit map_adapter(const key_compare &cmp = key_compare(), 
                          const allocator_type &alloc = allocator_type ())
     : c(cmp, alloc) {}
    template <class InputIterator>
    map_adapter(InputIterator b, InputIterator e, 
                const key_compare &cmp = key_compare(), 
                const allocator_type &alloc = allocator_type ())
     : c(b, e, cmp, alloc) {}
    // default copy ctr
    // default destructor
    // default assignment
    // Extra
    explicit map_adapter(base_type const &b,
                          const key_compare &cmp = key_compare())
     : c(b,cmp) {}
    map_adapter &operator=(base_type const &b) {
        c = b;
        return *this;
    }

    set_type const &set() const { return c; }
    base_type const &base() const { return c.base(); }

    // Iterators
    iterator begin() { return c.begin(); }
    const_iterator begin() const { return c.begin(); }
    iterator end() { return c.end(); }
    const_iterator end() const { return c.end(); }
    reverse_iterator rbegin() { return c.rbegin(); }
    const_reverse_iterator rbegin() const { return c.rbegin(); }
    reverse_iterator rend() { return c.rend(); }
    const_reverse_iterator rend() const { return c.rend(); }
  
    // Capacity
    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }
    size_type max_size() const { return c.max_size(); }
    // Extra
    size_type capacity() const { return c.capacity(); }
    void reserve(size_type n) { return c.reserve(n); }

    // Modifiers
    std::pair<iterator, bool> insert(const value_type &v) {
        return c.insert(v);
    }
    iterator insert(iterator hint, const value_type &v) {
        return c.insert(hint, v);
    }
    template <class InputIterator>
    void insert(InputIterator b, InputIterator const e) {
        return c.insert(b,e);
    }
    void erase(iterator it) { c.erase(it); }
    size_type erase(const key_type &k) { c.erase(k); }
    void erase(iterator b, iterator e) { c.erase(b, e); }
    void swap(map_adapter &other) {
        using namespace std;
        c.swap(other.c); // swap( c, other.c );
    }
    void clear() { c.clear(); }
    // Extra
    // useful for building a base_type unsorted, then swapping it in
    void swap(base_type &other) {
        using namespace std;
        c.swap(other); // swap( c, other.c );
    }

    // Observers
    value_compare value_comp() const { return c.value_comp(); }
    key_compare key_comp() const { return value_comp().key_comparator; }

    // map operations
    mapped_type &operator[](const key_type &k) {
        return insert(value_type(k,mapped_type())).first->second;
    }
    size_type count(const key_type &k) const {
        return contains(k)?1:0;
    }
    std::pair<iterator, iterator> equal_range(const key_type &k) {
        return std::equal_range(begin(), end(), k, value_comp());
    }
    std::pair<const_iterator, const_iterator> equal_range(const key_type &k) const {
        return std::equal_range(begin(), end(), k, value_comp());
    }
    iterator find(const key_type &k) {
        iterator it = lower_bound(k);
        if ( it == end() || comparator(k, *it) ) {
            return end();
        } else {
            return it;
        }
    }
    const_iterator find(const key_type &k) const {
        const_iterator it = lower_bound(k);
        if ( it == end() || comparator(k, *it) ) {
            return end();
        } else {
            return it;
        }
    }
    iterator lower_bound(const key_type &k) {
        return std::lower_bound(begin(), end(), k, value_comp());
    }
    const_iterator lower_bound(const key_type &k) const {
        return std::lower_bound(begin(), end(), k, value_comp());
    }
    iterator upper_bound(const key_type &k) {
        return std::upper_bound(begin(), end(), k, value_comp());
    }
    const_iterator upper_bound(const key_type &k) const {
        return std::upper_bound(begin(), end(), k, value_comp());
    }
    bool contains(const key_type &k) const {
        return std::binary_search(begin(), end(), k, value_comp());
    }
  
    // Comparison Operators
    bool operator==(map_adapter const &other) { return c == other.c; }
    bool operator!=(map_adapter const &other) { return c != other.c; }
    bool operator<(map_adapter const &other) { return c < other.c; }
    bool operator<=(map_adapter const &other) { return c <= other.c; }
    bool operator>(map_adapter const &other) { return c > other.c; }
    bool operator>=(map_adapter const &other) { return c >= other.c; }
};

// Overloaded Algorithms
template < typename base_type,
            typename CMP >
void swap(map_adapter<base_type,CMP> const &lhs,
          map_adapter<base_type,CMP> const &rhs) {
    lhs.swap(rhs);
}
template < typename base_type,
            typename CMP >
void swap(base_type const &lhs,
          map_adapter<base_type,CMP> const &rhs) {
    lhs.swap(rhs);
}
template < typename base_type,
            typename CMP >
void swap(map_adapter<base_type,CMP> const &lhs,
          base_type const &rhs) {
    lhs.swap(rhs);
}

} // namespace assist

#ifndef ASSIST_NO_NAMESPACE_STD_ADDITIONS
namespace std {

// Overloaded Algorithms
template < typename base_type,
            typename CMP >
void swap(assist::map_adapter<base_type,CMP> const &lhs,
          assist::map_adapter<base_type,CMP> const &rhs) {
    assist::swap(lhs,rhs);
}
template < typename base_type,
            typename CMP >
void swap(base_type const &lhs,
          assist::map_adapter<base_type,CMP> const &rhs) {
    assist::swap(lhs,rhs);
}
template < typename base_type,
            typename CMP >
void swap(assist::map_adapter<base_type,CMP> const &lhs,
          base_type const &rhs) {
    assist::swap(lhs,rhs);
}

} // namespace std
#endif

#endif
