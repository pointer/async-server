import socket

HOST = '127.0.0.1'  # Server address
PORT = 12345        # Server port

# Credentials for authentication
default_username = 'user'
default_password = 'pass'


def print_help():
    print("""
Available commands:
  <text>           Echo text to server
  QUIT             Close the session
  LIST_SESSIONS    Print session info to server console
  HELP             Show this help message
""")


def main():
    with socket.create_connection((HOST, PORT)) as s:
        # Receive Username prompt
        prompt = s.recv(1024).decode()
        print(prompt, end='')
        s.sendall((default_username + '\n').encode())

        # Receive Password prompt
        prompt = s.recv(1024).decode()
        print(prompt, end='')
        s.sendall((default_password + '\n').encode())

        # Receive authentication result
        result = s.recv(1024).decode()
        print(result, end='')
        if 'failed' in result:
            return

        print_help()
        # Echo loop
        while True:
            try:
                msg = input('> ')
                if msg.strip().upper() == 'HELP':
                    print_help()
                    continue
                s.sendall((msg + '\n').encode())
                reply = s.recv(1024)
                if not reply:
                    print('Server closed the connection.')
                    break
                print('Server:', reply.decode().strip())
                if msg.strip().upper() == 'QUIT':
                    break
            except (KeyboardInterrupt, EOFError):
                print('\nExiting.')
                break
            except Exception as e:
                print(f'Error: {e}')
                break

if __name__ == '__main__':
    main()
