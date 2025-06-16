// passserver.h
#include <unistd.h> // for crypt(), must link with -lcrypt
#include <string>
#include "hashtable.h"
using namespace std;

class PassServer
// Key: Username, and Value: Encrypted Password
{
public:
   PassServer(size_t size = 101);
   // creates a hash table of size. Pass size to tash table constructor.
   ~PassServer();
   // NO MEMORY LEAKS!
   bool load(const char *filename);
   // load a password file to hashTable object.
   // take filename and pass it to hashtable func, it will do the rest.
   bool addUser(std::pair<string, string> & kv);
   // username first, then password, which needs to be encrypted before adding
   bool addUser(std::pair<string, string> &&kv);	// move version
   bool removeUser(const string & k);	// delete user with username k
   bool changePassword
   (const std::pair<string, string> &p, const string & newpassword);
   // replace the pair with new pair with newpassword
   bool find(const string & user);
   // check if user (key) exists in hash table
   void dump();
   std::size_t size();
   bool write_to_file(const char* filename) const;
private:
   string encrypt(const string& str);

   // Member data:
   cop4530::HashTable<string, string>* server;
};
