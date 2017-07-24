#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define BCAST_IP "192.168.1.255"
#define BCAST_PORT 1234
#define MSG_SIZE 100

int main(size_t sockaddr_len) {
  int sockfd;
  struct sockaddr_in broadcast_addr;
  int bcflag = 1;
  char message[MSG_SIZE];

  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (sockfd < 0) {
    perror("Socket error");
    exit(-1);
  }

  broadcast_addr.sin_family = AF_INET;
  broadcast_addr.sin_port = htons(BCAST_PORT);
  inet_aton(BCAST_IP, &broadcast_addr.sin_addr);

  if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &bcflag, sizeof(int)) < 0) {
    perror("Setsockopt error");
    exit(-1);
  }

  sprintf(message, "Hello, broadcast World!");

  if (sendto(sockfd, message, MSG_SIZE, 0, (struct sockaddr *) &broadcast_addr, sizeof(struct sockaddr_in)) < 0) {
    perror("Sendto error");
    exit(-1);
  }

  close(sockfd);

  return 0;
}