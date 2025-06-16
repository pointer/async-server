#include <iostream>
#include <fstream>
#include <string>
using namespace std;

string encrypt(string word, string key)
{
    for (int i = 0; i < word.length(); i++)
    {
        int encryptedItem = (int) word[i] + key[i];
        word[i] = (char) encryptedItem;
    }
    return word;
}

string decrypt(string password, string key)
{
    for (int i = 0; i < password.length(); i++)
    {
        int encryptedItem = (int) password[i] - key[i];
        password[i] = (char) encryptedItem;
    }
    return password;
}

void showPasswords(string key)
{
    fstream passwords;
    string word;
    int i{0};
    passwords.open("passwords.txt");
    while (passwords >> word) {
        cout << decrypt(word, key);
        i++;
        if (i%2) cout << " ";
        else cout << "\n";
    }
    passwords.close();
}

bool verifyLength(string word, string key)
{
    int verify;
    if (word.length() > key.length())
    {
        cout << "The text is longer than the key,\nThis will mean you cannot decrypt if you continue\n1. Continue (NOT RECOMMENDED), Press Any Other Key to Exit Program\n";
        cin >> verify;
        if (verify != 1)
        {
            return false;
        }
        else return true;
    }
    else return true;
}

// int main()
// {
//     string key{};
//     cout << "What is the encryption key you are using?\n";
//     cin >> key;

//     int select{0};
//     while (select != 3)
//     {
//         cout << "Do you want to 1. Find a Password or 2. Add a Password or 3. Exit\n";
//         cin >> select;
//         fstream passwords;
//         if (!passwords)
//             cout << "No such file found";
//         else {
//             string user{};
//             string pw{};
//             string verify{};
//             switch(select)
//             {
//                 case 1:
//                     showPasswords(key);
//                     break;
//                 case 2:
//                     passwords.open("passwords.txt", ios::app);
//                     cout << "What is the username/identifier of the account?";
//                     cin >> user;
//                     if (!verifyLength(user, key))
//                     {
//                         passwords.close();
//                         return 1;
//                     }
//                     passwords << encrypt(user, key) << endl;
//                     cout << "What is the password of the account?";
//                     cin >> pw;
//                     if (!verifyLength(pw, key))
//                     {
//                         passwords.close();
//                         return 1;
//                     }
//                     passwords << encrypt(pw, key) << endl << endl;
//                     cout << "Data appended successfully\n";
//                     passwords.close();
//                     break;
//                 case 3:
//                     cout << "Program Exiting";
//                     break;
//                 case 0:
//                     cout << "Are you sure?";
//                     cin >> verify;
//                     if (verify == "01234567890")
//                     {
//                         ofstream file("passwords.txt");
//                         file.close();
//                     }
//             }
//         }
//     }
//     return 0;
// }
