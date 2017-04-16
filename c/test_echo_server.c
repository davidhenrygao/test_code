#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define BUFFSIZE 64
char buf[BUFFSIZE] = {"\0"};

int main(int argc, char *argv[]) {
  int sockfd, connfd;
  struct sockaddr_in addr;
  struct sockaddr_in cliaddr;
  socklen_t addrlen;
  int i;
  ssize_t rb, wb;

  //ingnore signal PIPE.
  signal(SIGPIPE, SIG_IGN);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(2017);
  addr.sin_addr.s_addr = INADDR_ANY;
  //addr.sin_addr.s_addr = inet_addr("192.168.82.198");
  
  int reuse = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse, sizeof(reuse));

  bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
  listen(sockfd, SOMAXCONN);

  printf("Server start.\n");

  for(;;) {
    printf("accept wait.\n");
    addrlen = sizeof(cliaddr);
    connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &addrlen);
    printf("accept connection from %s:%u\n", 
        inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

    for(;;) {
      rb = read(connfd, (void*)buf, BUFFSIZE-1);
      if (rb < 0) {
        printf("read socket %d error: %s.\n", 
            connfd, strerror(errno));
        break;
      }
      if (rb == 0) {
        printf("socket peer end close the connetion.\n");
        break;
      }
      if (rb > 0) {
        buf[rb] = '\0';
        printf("recv client msg: %s.\n", buf);
      }
      wb = write(connfd, (void*)buf, strlen(buf));
      if (wb < 0) {
        printf("write socket %d error: %s.\n", 
            connfd, strerror(errno));
        break;
      }
    }
    close(connfd);
  }
  close(sockfd);
  return 0;
}
