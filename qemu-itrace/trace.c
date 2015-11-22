#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

int send_packet(char *data, int sockfd) {
  char str[1024];
  int i;
  unsigned char cs = 0;
  for(i = 0; i < strlen(data); i++) cs += data[i];
  sprintf(str, "$%s#%02x\0", data, cs);
  write(sockfd, str, strlen(str));
}

int print_reg(char *str, char *buf) {
  printf("%s: %c%c%c%c%c%c%c%c\n", str, buf[6], buf[7], buf[4], buf[5], buf[2], buf[3], buf[0], buf[1]);
}

int recv_packet(int sockfd, int type) {
  int ba, br;
  char *buf, ob[1024], bp;
  buf = ob;
  do {
    br = read(sockfd, buf, 1023);
    if(br < 1) exit(1);
    buf[br] = 0;
  } while(buf[br - 3] != '#');
  while(buf[0] == '+') buf++;
  switch(type) {
  case 0:
    break;
  case 1:
    printf("%s\n", buf);
    break;
  case 2:
    //    print_reg("eax", buf + 1);
    //    print_reg("ecx", buf + 8 + 1);
    //    print_reg("edx", buf + 8*2 + 1);
    //    print_reg("ebx", buf + 8*3 + 1);
    //    print_reg("esp", buf + 8*4 + 1);
    //    print_reg("ebp", buf + 8*5 + 1);
    //    print_reg("esi", buf + 8*6 + 1);
    //    print_reg("edi", buf + 8*7 + 1);
    print_reg("eip", buf + 8*8 + 1);
    //    print_reg("efl", buf + 8*9 + 1);
    break;
  }
}

int main(void) {
  int sockfd, n;
  struct sockaddr_in s_addr;

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons(1234);
  s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if(connect(sockfd, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0) {
    perror("connect");
    exit(1);
  }
    send_packet("Z0,00100150", sockfd);
  recv_packet(sockfd,0);
  send_packet("vCont;c:1", sockfd);
  recv_packet(sockfd, 0);
  send_packet("z0,00100150", sockfd);
  recv_packet(sockfd, 0);
  while(1) {
    send_packet("vCont;s:1", sockfd);
    recv_packet(sockfd,0);
    send_packet("g", sockfd);
    recv_packet(sockfd,2);
    //    getchar();
  }
}
  
