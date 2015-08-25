#ifndef ASSIST_MULTISET_ADAPTER_HPP
#define ASSIST_MULTISET_ADAPTER_HPP

/*
 * assist/multiset_adapter.hpp
 *
 * Copyright (c) 2006 Scott McMurray
 *
 * Licensed under the Open Software License version 3.0
 * ( See http://opensource.org/licenses/osl-3.0.php )
 *
 */

/* WARNING: Not exception-safe in the face of
 * ordering predicates that throw exceptions.
 */

#include <utility> // pair
#include <algorithm> // sort, swap, inplace_merge,
                     // equal_range, lower_bound, upper_bound
#include <functional> // less,

namespace assist {

template < typename base_type,
            typename CMP = std::less<typename base_type::value_type> >
class multiset_adapter {
    base_type c;
    CMP comparator;

  public:
    // Types
    typedef typename base_type::value_type key_type;
    typedef key_type value_type;
    typedef CMP key_compare;
    typedef key_compare value_compare;
    typedef typename base_type::allocator_type allocator_type;
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    typedef typename base_type::reverse_iterator reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
//    typedef typename base_type:: ;

    // Construct/Copy/Destroy
    explicit multiset_adapter(const key_compare &cmp = key_compare(), 
                          const allocator_type &alloc = allocator_type ())
     : c(alloc), comparator(cmp) {}
    template <class InputIterator>
    multiset_adapter(InputIterator b, InputIterator e, 
                const key_compare &cmp = key_compare(), 
                const allocator_type &alloc = allocator_type ())
     : c(b, e, alloc), comparator(cmp) {
        std::sort(begin(), end(), comparator);
    }
    // default copy ctr
    // default destructor
    // default assignment
    // Extra
    explicit multiset_adapter(base_type const &b,
                          const key_compare &cmp = key_compare())
     : c(b), comparator(cmp) {
        std::sort(begin(), end(), comparator);
    }
    multiset_adapter &operator=(base_type const &b) {
        c = b;
        std::sort(begin(), end(), comparator);
        return *this;
    }

    base_type const &base() const { return c; }

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
    iterator insert(const value_type &v) {
        iterator it = lower_bound(v);
        return c.insert(it, v);
    }
/*
    iterator insert(iterator hint, const value_type &v) {
    ******FIX******
        if ( hint >= end()-1 ) {
            if ( empty() || !comparator(v,*(end()-1)) ) {
                c.push_back(v);
                return end();
            } else {
                return insert(v).first;
            }
        }
        if ( !comparator( v, *hint ) ) {
            if ( comparator( v, *(hint+1) ) ) {
                // right place
                return c.insert(hint, v);       
            } else {
                // *hint equivalent to v
                // nothing to do
                return hint;
            }
        } else {
            // hint was wrong
            return insert(v).first;
        }
    }
*/
    template <class InputIterator>
    void insert(InputIterator b, InputIterator const e) {
    /*
        // Simple, obvious, but often slow
        // basic exception safe.
        while (b != e) insert(*b++);
    */
        // More complex, but likely faster.
        // *Not* safe if the comparator throws, though :|
        size_type const before_size = size();
        try {
            c.insert(c.end(), b, e);
        } catch (...) {
            assert( size() >= before_size );
            if ( size() != before_size ) c.resize(before_size);
            throw;
        }
        std::sort(begin()+before_size, end(), comparator);
        std::inplace_merge(begin(), begin()+before_size, end(), comparator);
    }
    void erase(iterator it) { c.erase(it); }
    size_type erase(const key_type &k) { c.erase(find(k)); }
    void erase(iterator b, iterator e) { c.erase(b, e); }
    void swap(multiset_adapter &other) {
        using namespace std;
        // swap comparator first for exception safety
        swap( comparator, other.comparator );
        c.swap(other.c); // swap( c, other.c );
    }
    void clear() { c.clear(); }
    // Extra
    // useful for building a base_type unsorted, then swapping it in
    // WARNING: Not O(1)
    void swap(base_type &other) {
        using namespace std;
        c.swap(other); // swap( c, other.c );
        std::sort(begin(), end(), comparator);
    }

    // Observers
    key_compare key_comp() const { return comparator; }
    value_compare value_comp() const { return key_comp(); }

    // Set operations
    size_type count(const key_type &k) const {
        std::pair<const_iterator, const_iterator> edges = equal_range(k);
        return edges.second-edges.first;
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
    bool operator==(multiset_adapter const &other) { return c == other.c; }
    bool operator!=(multiset_adapter const &other) { return c != other.c; }
    bool operator<(multiset_adapter const &other) { return c < other.c; }
    bool operator<=(multiset_adapter const &other) { return c <= other.c; }
    bool operator>(multiset_adapter const &other) { return c > other.c; }
    bool operator>=(multiset_adapter const &other) { return c >= other.c; }
};

// Overloaded Algorithms
template < typename base_type,
            typename CMP >
void swap(multiset_adapter<base_type,CMP> const &lhs,
          multiset_adapter<base_type,CMP> const &rhs) {
    lhs.swap(rhs);
}
// WARNING: Not O(1)
template < typename base_type,
            typename CMP >
void swap(base_type const &lhs,
          multiset_adapter<base_type,CMP> const &rhs) {
    lhs.swap(rhs);
}
// WARNING: Not O(1)
template < typename base_type,
            typename CMP >
void swap(multiset_adapter<base_type,CMP> const &lhs,
          base_type const &rhs) {
    lhs.swap(rhs);
}

} // namespace assist

#ifndef ASSIST_NO_NAMESPACE_STD_ADDITIONS
namespace std {

// Overloaded Algorithms
template < typename base_type,
            typename CMP >
void swap(assist::multiset_adapter<base_type,CMP> const &lhs,
          assist::multiset_adapter<base_type,CMP> const &rhs) {
    assist::swap(lhs,rhs);
}
// WARNING: Not O(1)
template < typename base_type,
            typename CMP >
void swap(base_type const &lhs,
          assist::multiset_adapter<base_type,CMP> const &rhs) {
    assist::swap(lhs,rhs);
}
// WARNING: Not O(1)
template < typename base_type,
            typename CMP >
void swap(assist::multiset_adapter<base_type,CMP> const &lhs,
          base_type const &rhs) {
    assist::swap(lhs,rhs);
}

} // namespace std
#endif

#endif
