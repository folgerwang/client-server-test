

#include <iostream> 
#include <thread> 
#include <string>
#include <cstring>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

char client_message[2000];
char buffer[1024];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void socketThread(void *arg) {
  int new_socket = *((int *)arg);
  recv(new_socket , client_message , 2000 , 0);

  // Send message to the client socket 
  pthread_mutex_lock(&lock);
  std::string message;
  message.reserve(sizeof(client_message)+20);
  message = std::string("Hello Client : ");
  message += client_message;
  message += std::string("\n");
  std::memcpy(buffer, message.c_str(), message.length());
  pthread_mutex_unlock(&lock);
  sleep(1);
  send(new_socket, buffer, 13, 0);
  std::cout << "Exit socketThread" << std::endl;
  close(new_socket);
}

int main() {
  int server_socket, new_socket;
  struct sockaddr_in server_addr;
  struct sockaddr_storage server_storage;
  socklen_t addr_size;

  //Create the socket. 
  server_socket = socket(PF_INET, SOCK_STREAM, 0);

  // Configure settings of the server address struct
  // Address family = Internet 
  server_addr.sin_family = AF_INET;

  //Set port number, using htons function to use proper byte order 
  server_addr.sin_port = htons(7799);

  //Set IP address to localhost 
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  //Set all bits of the padding field to 0 
  memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

  //Bind the address struct to the socket 
  bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));

  //Listen on the socket, with 40 max connection requests queued 
  if(listen(server_socket, 50)==0) {
    std::cout << "Listening" << std::endl;
  }
  else {
    std::cout << "Error" << std::endl;
  }
  pthread_t tid[60];
  int i = 0;
  while(1) {
    //Accept call creates a new socket for the incoming connection
    addr_size = sizeof(server_storage);
    new_socket = accept(server_socket, (struct sockaddr *) &server_storage, &addr_size);

    //for each client request creates a thread and assign the client request to it to process
    //so the main thread can entertain next request
    std::thread thread_object(socketThread, &new_socket); 

    thread_object.join();
  }

  return 0;
}
