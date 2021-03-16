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

void cientThread(void *arg)
{
  std::cout << "In thread" << std::endl;
  char message[1000];
  char buffer[1024];
  int clientSocket;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  // Create the socket. 
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);

  //Configure settings of the server address
  // Address family is Internet 
  serverAddr.sin_family = AF_INET;

  //Set port number, using htons function 
  serverAddr.sin_port = htons(7799);

  //Set IP address to localhost
  serverAddr.sin_addr.s_addr = inet_addr("localhost");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  //Connect the socket to the server using the address
  addr_size = sizeof serverAddr;
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
  strcpy(message,"Hello");

  if( send(clientSocket , message , strlen(message) , 0) < 0)
  {
    std::cout << "Send failed" << std::endl;
  }

  //Read the message from the server into the buffer
  if(recv(clientSocket, buffer, 1024, 0) < 0)
  {
    std::cout << "Receive failed" << std::endl;
  }
  //Print the received message
  std::cout << "Data received: " << buffer << std::endl;
  close(clientSocket);
}

int main(){
  int i = 0;
  std::vector<std::thread> threads;
  while (i< 50)
  {
    std::thread cur_thread(cientThread, nullptr); 
    threads.push_back(std::move(cur_thread));
  }
  sleep(20);
  i = 0;
  while(threads.size() > 0)
  {
     threads.back().join();
     threads.pop_back();
     std::cout << i << std::endl;
     i++;
  }
  return 0;
}