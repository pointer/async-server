// hashtable.hpp
using namespace cop4530;

template <typename K, typename V>
HashTable<K, V>::HashTable(size_t size)
{
   theLists.resize(prime_below(size));
   currentSize = 0;
}

template <typename K, typename V>
HashTable<K, V>::~HashTable()
{
   makeEmpty();
}

template <typename K, typename V>
bool HashTable<K, V>::contains(const K & k) const
// check if key k is in the hash table
{
   auto & whichList = theLists[myhash(k)];
   auto start = whichList.begin();
   while (start != whichList.end()) {
      if (std::get<0>(*start) == k)
	 return true;
      ++start;
   }
   return false;
}

template <typename K, typename V>
bool HashTable<K, V>::match(const std::pair<K, V>&kv) const
// check for key & val pair
{
   auto & whichList = theLists[myhash(std::get<0>(kv))];
   return std::find(whichList.begin(), whichList.end(), kv) != whichList.end();
}

template <typename K, typename V>
bool HashTable<K, V>::insert(const std::pair<K, V> & kv)
// If already in hash table or not added, return false.
// If found, then update it, or insert if not found, and return true.
{
   auto k = std::get<0>(kv);
   auto & whichList = theLists[myhash(k)];
   typename std::list< std::pair<K,V> >::iterator itr; 
   						
   if (contains(std::get<0>(kv))) {
      if (match(kv)) {
	 std::cout << "\n*****Error: Ignoring duplicate entries for " << k;
	 return false;
      }
      else 
	 remove(k);
   }  // Entry removed here, and then added with new value below
   
   whichList.push_back(kv);
   if (++currentSize > theLists.size())
      rehash();
   return true;
}

template <typename K, typename V>
bool HashTable<K, V>::insert(const std::pair<K, V> && kv)
// move version of insert
{
   auto k = std::get<0>(kv);
   auto & whichList = theLists[myhash(k)];
   typename std::list< std::pair<K,V> >::iterator itr; 
   						
   if (contains(std::get<0>(kv))) {
      if (match(kv)) {
	 std::cout << "\n*****Error: Ignoring duplicate entries for " << k;
	 return false;
      }
      else 
	 remove(k);
   }  // Entry removed here, and then added with new value below
   
   whichList.push_back(kv);
   if (++currentSize > theLists.size())
      rehash();
   return true;
}

template <typename K, typename V>
bool HashTable<K, V>::remove(const K & k)
{
   auto & whichList = theLists[myhash(k)];
   auto itr = whichList.begin();

   while (std::get<0>(*itr) != k && itr != whichList.end()) 
      ++itr;

   if (itr == whichList.end())
      return false;

   whichList.erase(itr);
   --currentSize;
   return true;
}

template <typename K, typename V>
void HashTable<K, V>::clear()
// delete all hash table elements
{
   makeEmpty();
}

template <typename K, typename V>
bool HashTable<K, V>::load(const char *filename)
// load elements from file into hash table
{
   std::ifstream myFile;
   K key;
   V value;
   bool failed = false;

   myFile.open(filename);
   if (!myFile.good()) {
      myFile.close();
      return false;
   }
   while (myFile >> key >> value) 
      if (!insert(std::make_pair(key, value))) 
	      failed = true;
   myFile.close();

   if (failed)
      std::cout << "Error adding atleast one pair to table" << std::endl;

   return true;
}

template <typename K, typename V>
void HashTable<K, V>::dump() const
// displays all entries of a hash table
{
   bool first;
   typename std::list< std::pair<K,V> >::iterator itr;
   std::list< std::pair<K, V>> whichList;

   for (int i = 0; i < theLists.size(); i++) {
      whichList = theLists[i];
      std::cout << "v[" << i << "]: ";
      for (itr = whichList.begin(); itr != whichList.end(); ++itr) {
	 std::cout << std::get<0>(*itr) << " " << std::get<1>(*itr) << "\t";
      }
      std::cout << std::endl;
   }
}

template <typename K, typename V>
size_t HashTable<K, V>::size() const
// returns the size of hash table
{
   return currentSize;
}

template <typename K, typename V>
bool HashTable<K, V>::write_to_file(const char *filename) const
//write all elements from hash table to file
{
   std::ofstream myFile;
   typename std::list< std::pair<K,V> >::iterator itr;
   std::list< std::pair<K, V>> whichList;

   myFile.open(filename);
   if (!myFile.good()) {
      myFile.close();
      return false;
   }

   for (int i = 0; i < theLists.size(); i++) {
      whichList = theLists[i];
      for (itr = whichList.begin(); itr != whichList.end(); ++itr) {
	 myFile << std::get<0>(*itr) << " " << std::get<1>(*itr) << "\n";
      }
   }
   myFile.close();

   return true;
}

// Helper functions
template <typename K, typename V>
void HashTable<K, V>::makeEmpty()
// called from public, delete hash table elements
{
   for(auto & tempList : theLists) {
      tempList.clear();
   }
}

template <typename K, typename V>
void HashTable<K, V>::rehash()
// rehash function, for if table elements > vector size
{
   auto oldLists = theLists;	// hash table copy

   theLists.resize(prime_below(2 * theLists.size()));
   for (auto & thisList : theLists)
      thisList.clear();

   // copy table
   currentSize = 0;
   for (auto & thisList : oldLists)
      for (auto & x : thisList)
	 insert(std::move(x));
}

template <typename K, typename V>
size_t HashTable<K, V>::myhash(const K &k) const
// return the index of the vector entry where k should be stored
{
   static std::hash<K> hf;
   return hf(k) % theLists.size();
}

// Pre-defined functions by (my liege) Bobject himself
template <typename K, typename V>
unsigned long HashTable<K, V>::prime_below (unsigned long n) const
{
  if (n > max_prime)
    {
      std::cerr << "** input too large for prime_below()\n";
      return 0;
    }
  if (n == max_prime)
    {
      return max_prime;
    }
  if (n <= 1)
    {
		std::cerr << "** input too small \n";
      return 0;
    }

  // now: 2 <= n < max_prime
  std::vector <unsigned long> v (n+1);
  setPrimes(v);
  while (n > 2)
    {
      if (v[n] == 1)
	return n;
      --n;
    }

  return 2;
}

//Sets all prime number indexes to 1. Called by method prime_below(n) 
template <typename K, typename V>
void HashTable<K, V>::setPrimes(std::vector<unsigned long>& vprimes) const
{
  int i = 0;
  int j = 0;

  vprimes[0] = 0;
  vprimes[1] = 0;
  int n = vprimes.capacity();

  for (i = 2; i < n; ++i)
    vprimes[i] = 1;

  for( i = 2; i*i < n; ++i)
    {
      if (vprimes[i] == 1)
        for(j = i + i ; j < n; j += i)
          vprimes[j] = 0;
    }
}

