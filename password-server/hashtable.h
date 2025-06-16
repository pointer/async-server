#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <utility>   // for pair<T1, T2>
#include <algorithm>	// necessary for the find function
#include <functional>	// necessary for hash functor

namespace cop4530 {

   template <typename K, typename V>
   class HashTable
   {
   public:
      HashTable(size_t size = 101);
      // vector size = prime_below(size)
      ~HashTable();
      bool contains(const K & k) const;	// check if key k is in hash table
      bool match(const std::pair<K, V>&kv) const;	// check for key & val pair
      bool insert(const std::pair<K, V> & kv);
      // If already in hash table or not added, return false.
      // If found, then update it, or insert if not found, and return true.
      bool insert(const std::pair<K, V> && kv);	// move version of insert
      bool remove(const K & k);
      void clear();	// delete all hash table elements
      bool load(const char *filename);
      // load elements from file into hash table
      void dump() const;	// displays all entries of a hash table
      size_t size() const;	// returns the size of hash table
      bool write_to_file(const char *filename) const;
      // write all elements from hash table to file


   private:
      // max_prime is used by the helpful functions provided
      // to you.
      static const unsigned int max_prime = 1301081;

      // the default_capacity is used if the initial capacity 
      // of the underlying vector of the hash table is zero. 
      static const unsigned int default_capacity = 11;

      // Helper functions
      void makeEmpty();	// called from public, delete hash table elements
      void rehash();	// rehash function, for if table elements > vector size
      size_t myhash(const K &k) const;
      // return the index of the vector entry where k should be stored

      // functions pre-defined by Lord Bob himself
      unsigned long prime_below (unsigned long) const;
      void setPrimes(std::vector<unsigned long>&) const;

      // Private Member Data:
      std::vector<std::list<std::pair<K, V>>> theLists;
      size_t currentSize;
   };
};

#include "hashtable.hpp"
#endif
