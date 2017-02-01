#ifndef H_SOCKET
#define H_SOCKET

#define ADDRESS "127.0.0.1"
#define PORT 1337

namespace net {

  int createSocket(SOCKET * s) {
    WSADATA WSAData;

    if (WSAStartup(MAKEWORD(2,0), &WSAData) != 0) {
      printf("WSAStartup error\n");
      WSACleanup();
      return 1;
    }

    *s = socket(AF_INET, SOCK_STREAM, 0);
    if (*s == INVALID_SOCKET) {
      printf("Cant create socket\n");
      WSACleanup();
      return 1;
    }

    printf("\nsocket created\n");
    return 0;
  }

  int connect(SOCKET * s) {
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ADDRESS);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (connect(*s , (struct sockaddr *)&server , sizeof(server)) < 0) {
      printf("Conn error\n");
      closesocket(*s);
      WSACleanup();
      *s = INVALID_SOCKET;
      return 1;
    } else {
      printf("connected\n");
    }

    return 0;
  }

  int write(SOCKET * s, char *msg) {
    if(send(*s , msg , strlen(msg) , 0) < 0) {
      return 1;
    }
    return 0;
  }

  void close(SOCKET * s) {
    closesocket(*s);
    WSACleanup();
  }
}

#endif
