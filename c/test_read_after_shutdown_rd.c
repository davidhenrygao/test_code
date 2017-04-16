#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define BUFFERSIZE 64

void sigpipe_handler(int sig) {
  printf("catch signal %d.\n", sig);
}

int main(int argc, char *argv[]) {
  int ret;
  int sockfd;
  struct sockaddr_in addr;
  char rbuf[BUFFERSIZE];
  char wbuf[BUFFERSIZE];
  ssize_t rb, wb;

  //ignore SIGPIPE
  signal(SIGPIPE, &sigpipe_handler);

  printf("client start.\n");
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(2017);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  //addr.sin_addr.s_addr = inet_addr("192.168.82.198");
  ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
  if (ret != 0) {
    printf("connect server error: %s.\n", strerror(errno));
    return -1;
  }
  
  strncpy(wbuf, "Fuck you!", 10);
  wb = write(sockfd, (void*)wbuf, strlen(wbuf));
  if (wb < 0) {
    printf("send msg to server error: %s.\n", strerror(errno));
    goto err;
  }
  printf("send msg : %ld bytes.\n", wb);
  rb = read(sockfd, (void*)rbuf, BUFFERSIZE-1);
  if (rb < 0) {
    printf("recv msg to server error: %s.\n", strerror(errno));
    goto err;
  }
  if (rb == 0) {
    printf("socket peer end close.\n");
    goto err;
  }
  rbuf[rb] = '\0';
  printf("recv msg: %s.\n", rbuf);
  printf("shtudown read.\n");
  shutdown(sockfd, SHUT_RD);
  rb = read(sockfd, (void*)rbuf, BUFFERSIZE);
  if (rb < 0) {
    printf("recv msg to server error: %s.\n", strerror(errno));
    goto err;
  }
  if (rb == 0) {
    printf("socket peer end close.\n");
    goto err;
  }
err:
  close(sockfd);
  return 0;
}
