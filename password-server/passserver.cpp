// passserver.cpp
#include "passserver.h"

// Key: Username, Value: Password
// member data: HashTable* server
PassServer::PassServer(size_t size)
// creates a hash table of size. Pass size to hash table constructor.
{
   server = new cop4530::HashTable<string, string>(size);
}

PassServer::~PassServer()
// NO MEMORY LEAKS!
{
   delete server;
}

bool PassServer::load(const char *filename)
// load a password file to hashTable object.
// take filename and pass it to hashtable func, it will do the rest.
{
   return (*server).load(filename);
}

bool PassServer::addUser(std::pair<string, string> & kv)
// username first, then password, which needs to be encrypted before adding
{
   if ((*server).contains(std::get<0>(kv))) {
      std::cout << "Username already taken" << std::endl;
      return false;
   }
      
   auto p = std::make_pair(std::get<0>(kv), encrypt(std::get<1>(kv)));
   return (*server).insert(p);
}

bool PassServer::addUser(std::pair<string, string> &&kv)
{
   auto p = std::make_pair(std::get<0>(kv), encrypt(std::get<1>(kv)));
   return (*server).insert(p);
}

bool PassServer::removeUser(const string & k)
// delete user with username k
{
   return (*server).remove(k);
}

bool PassServer::changePassword
(const std::pair<string, string> &p, const string & newpassword)
// replace the pair with new pair with newpassword
{
   auto oldPair = std::make_pair(std::get<0>(p), encrypt(std::get<1>(p)));
   if (!(*server).match(oldPair)) {
      std::cout << "User password incorrect or user not found" << std::endl;
      return false;
   }
   auto kv = std::make_pair(std::get<0>(p), encrypt(newpassword));
   return (*server).insert(kv);
}

bool PassServer::find(const string & user)
// check if user (key) exists in hash table
{
   return (*server).contains(user);
}

void PassServer::dump()
{
   (*server).dump();
}

size_t PassServer::size()
{
   return (*server).size();
}

bool PassServer::write_to_file(const char* filename) const
{
   return (*server).write_to_file(filename);
}

string PassServer::encrypt(const string& str)
{
   return crypt(str.c_str(),"$1$########$");
}
