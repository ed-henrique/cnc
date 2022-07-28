<<<<<<< HEAD
int change_port(char*);
char* change_host(char*);
int connect_socket(int, char*);
=======
// Returns new_port as int.
int change_port_client(char* new_port);

// Returns new_host as char* with appropriate size allocated.
char* change_host(char* new_host);

// Connects to server through socket using HOST for ip and PORT for port, returning the socket.
int connect_socket_client(int PORT, char* HOST);
>>>>>>> dev
