#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MCAST_PORT 1234
#define MCAST_IP "225.111.111.111"
#define MSG_SIZE 100

int main() {
  int sockfd;
  struct sockaddr_in multicast_addr;
  char message[MSG_SIZE];

  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (sockfd < 0) {
    perror("Socket error");
    exit(EXIT_FAILURE);
  }

  multicast_addr.sin_family = AF_INET;
  multicast_addr.sin_port = htons(MCAST_PORT);
  inet_aton(MCAST_IP, &multicast_addr.sin_addr);

  sprintf(message, "Hello, Multicast World!\n");

  if (sendto(sockfd, message, MSG_SIZE, 0, (struct sockaddr *) &multicast_addr, sizeof(multicast_addr)) < 0) {
    perror("Sendto error");
    exit(EXIT_FAILURE);
  }

  close(sockfd);

  return EXIT_SUCCESS;
}