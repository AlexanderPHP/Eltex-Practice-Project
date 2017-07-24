#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MCAST_PORT 1234
#define MCAST_IP "225.111.111.111"
#define LOCAL_IP "192.168.1.101"
#define MSG_SIZE 100

int main() {
  int sockfd;
  struct sockaddr_in multicast_addr;
  struct sockaddr_in server_addr;
  struct ip_mreq req;
  socklen_t sockaddr_len;
  char message[MSG_SIZE];

  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (sockfd < 0) {
    perror("Socket error");
    exit(EXIT_FAILURE);
  }

  multicast_addr.sin_family = AF_INET;
  multicast_addr.sin_port = htons(MCAST_PORT);
  inet_aton(LOCAL_IP, &multicast_addr.sin_addr);

  sockaddr_len = sizeof(multicast_addr);
  if (bind(sockfd, (struct sockaddr *) &multicast_addr, sockaddr_len) < 0) {
    perror("Bind error");
    exit(EXIT_FAILURE);
  }

  inet_aton(MCAST_IP, &req.imr_multiaddr);
  inet_aton(LOCAL_IP, &req.imr_interface);

  if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &req, sizeof(req))) {
    perror("Setsockopt error");
    exit(EXIT_FAILURE);
  }

  sockaddr_len = sizeof(server_addr);
  if (recvfrom(sockfd, message, MSG_SIZE, 0, (struct sockaddr *) &server_addr, &sockaddr_len) < 0) {
    perror("Recvfrom error");
    exit(EXIT_FAILURE);
  }

  printf("%s(%d): %s\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), message);

  close(sockfd);
  return EXIT_SUCCESS;
}