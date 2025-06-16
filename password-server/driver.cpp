#include <iostream>
#include "passserver.h"
#include <cstring>

void Menu();
void Load(PassServer&);
void Add(PassServer&);
void Remove(PassServer&);
void ChangePass(PassServer&);
void FindUser(PassServer&);
void Write(PassServer&);


int main() {
   char input = '/';
   size_t size;

   cout << "Enter preferred hash table capacity: ";
   cin >> size;
   PassServer table{size};

   while (input != 'x') {
      Menu();
      cin >> input;
      switch (input) {
         case 'l':
            Load(table);
            break;
         case 'a':
            Add(table);
            break;
         case 'r':
            Remove(table);
            break;
         case 'c':
            ChangePass(table);
            break;
         case 'f':
            FindUser(table);
            break;
         case 'd':
            table.dump();
            break;
         case 's':
            cout << "Size of hashtable: " << table.size();
            break;
         case 'w':
            Write(table);
            break;
         case 'x':
            return 0;
      }
   }
}

void FindUser(PassServer& table) {
   string user;

   cout << "Enter username\t:";
   cin >> user;
   if (table.find(user))
      cout << "User '" << user << "' found";
   else
      cout << "User '" << user << "' not found";
}

void ChangePass(PassServer& table){
   string user, pass, newPass;

   cout << "Enter username\t: ";
   cin >> user;
   cout << "Enter password\t: ";
   cin >> pass;
   cout << "\nEnter new password: ";
   cin >> newPass;

   if (table.changePassword(std::make_pair(user, pass), newPass))
      cout << "Password for user " << user << " changed";
   else
      cout << "*****Error: Could not change user password";
}

void Remove(PassServer& table) {
   string user;

   cout << "Enter username: ";
   cin >> user;
   if (table.removeUser(user))
      cout << "User " << user << " deleted";
   else
      cout << "*****Error: User not found.  Could not delete user";
}

void Load(PassServer& table) {
   char filename[30];

   cout << "Enter password file name to load from: ";
   cin >> filename;
   if (table.load(filename))
      cout << "File has been loaded";
   else
      cout << "Error: Cannot open file " << filename;
}

void Add(PassServer& table) {
   string user, pass;

   cout << "Enter username: ";
   cin >> user;
   cout << "Enter password: ";
   cin >> pass;
   auto p = std::make_pair(user, pass);
   if (table.addUser(p))
      cout << "\nUser " << user << " added";
   else
      cout << "\nUser " << user << " not added";
}

void Write(PassServer& table) {
   char filename[30];

   cout << "Enter password file name to write to: ";
   cin >> filename;
   if (table.write_to_file(filename))
      cout << "File has been written";
   else
      cout << "File has not been written";
}

void Menu()
{
   cout << "\n\n";
   cout << "l - Load From File" << endl;
   cout << "a - Add User" << endl;
   cout << "r - Remove User" << endl;
   cout << "c - Change User Password" << endl;
   cout << "f - Find User" << endl;
   cout << "d - Dump HashTable" << endl;
   cout << "s - HashTable Size" << endl;
   cout << "w - Write to Password File" << endl;
   cout << "x - Exit program" << endl;
   cout << "\nEnter choice : ";
}
