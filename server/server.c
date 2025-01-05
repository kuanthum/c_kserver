//Inports

//main loop
#include "server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>

void start_server(const char *host, uint16_t port) {

  printf("Startin server at: %s:%d", host, port);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in sockfd_addr;

  sockfd_addr.sin_family        = AF_INET;
  sockfd_addr.sin_port          = htons(port);
  sockfd_addr.sin_addr.s_addr   = htonl(*host);
  
  if (bind(sockfd, (const struct sockaddr*)&sockfd_addr, sizeof(sockfd_addr)) <0 ) {
    perror("Error binding socket (server.c)\n");
  } else {
    printf("Bind socket succsessfully!\n");
  }

}
