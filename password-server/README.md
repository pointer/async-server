# Cpp-Password-Server
C++ password server built to demonstrate the nature of using a hashtable for random access of key/value pairs, where keys are usernames and the value is an encrypted password.

Using <i>make all</i>, two executable files will be made: <b>driver.x</b> and <b>sdriver.x</b>. The latter will hide password input while its being entered.
I've written in the following functions: load users from a file, add users from a file, write users to a file, remove a user, change a user's password (requires user's old password), find if a user is in the password server, and "dump" hashtable to show all users, encrypted passwords, and their addresses in the hashtable.

This project was built by me for Data Structures II (COP4530) at Florida State University.
