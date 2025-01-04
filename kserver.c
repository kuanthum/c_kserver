#include <stdio.h>
//#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Check with lsof -i :8080
#define PORT 8080
#define BUFFER_SIZE 2048

int main() {

  char buffer[BUFFER_SIZE];
  char resp[] = "HTTP/1.0 200 OK\r\n"
  "Server: webserver-c\r\n"
  "Content-type: text/html\r\n\r\n"
  "<html>hello, world</html>\r\n";

	//  Descriptor table indx (see man sys_socket.h)
  //  See fcntl() para configurar socket qu eno se bloquee
	int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_descriptor == -1) {
		perror("webserver (socket)");
		return 1;
	}

	printf("Socket created successfully. Descriptor: %d\n", socket_descriptor);

	// Create addrs (see man 7 ip)
  struct sockaddr_in host_addr;
  int host_addr_len = sizeof(host_addr);

  host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  host_addr.sin_port = htons(PORT);
  host_addr.sin_family = AF_INET;

  //Creat client addrs
  struct sockaddr_in client_addr;
  int client_addrlen = sizeof(client_addr);

  printf("Socket adress addr: %d\n", host_addr.sin_addr.s_addr);
  printf("Socket adress port: %d\n", host_addr.sin_port);
  printf("Socket adress family: %d\n", host_addr.sin_family);

	// Bind socket to interface addrs (see man bind). This function return 0 if ok
  /* Personal Note.
   * Explicit cast here, in rust this is avoided using enum container.
   * Also uses traitas like From Into and TryFrom
  */
  if (bind(socket_descriptor, (struct sockaddr*)&host_addr, host_addr_len) != 0) {
    perror("webserver(bind)");
    //ss -tuln | grep 8080 -> PID then kill it
    return 1;
  } else {
    printf("Binding succes!\n");
  }

  //Set socket to passibly listen
  //listen tambien lockea?
  if (listen(socket_descriptor, SOMAXCONN) != 0) {
    perror("webserver(listen)");
  } else {
    printf("Socket listening in: \n");
  }
  
  //Accept conections. This will create another fd and let the previous so it can accept new conections.
  for(;;) {
    //accept() function blocks the thread (see select() poll() epoll() for multiplexing)
    //CODE ISNT PRINTING THIS
    printf("Running accept fn...\n\n");
    //Creats a new socket leaving previus for listening
    int newsockfd = accept(socket_descriptor, (struct sockaddr*)&host_addr, (socklen_t*)&host_addr_len);
    if (newsockfd < 0) {
      perror("Error accepting conections");
      continue;
    } else {
      printf("Incoming connection accepted!\n");
    }
    /* El código hasta acá falla de la siguiente manera
     * curl: (56) Recv failure: Connection reset by peer
     * */

    //Get client address
    int sockn = getsockname(newsockfd, (struct sockaddr*)&client_addr, (socklen_t*)&client_addrlen);
    if (sockn < 0) {
      perror("webserver (getsocketname)");
      continue;
    } else {
      printf("Client sock addr: %d\n", sockn);
    }

    int valread = read(newsockfd, buffer, BUFFER_SIZE);
    if (valread < 0) {
      perror("Error readeing ");
      continue;
    } else {
      printf("Bytes readed: %d\n\n", valread);
    }

    //Read request
    char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
    //     getbuf->put this-> into this?
    printf("Buffer: \n\n%s\n", buffer);
    
    printf("Sscanf result:\n");
    sscanf(buffer, "%s %s %s", method, uri, version);
    printf("[%s:%u]\n %s\n %s\n %s\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, uri, version);
    /* Hasta acá falla con lo suiguiente:
     *curl: (52) Empty reply from server
     */

    //Esto se va a mostrar en el cliente
    int valwrite = write(newsockfd, resp, strlen(resp));
    if (valwrite < 0) {
      perror("webserver (write)");
      continue;
    }
    
    printf("Closing socket!");
    close(newsockfd);
  }
	return 0;
}
