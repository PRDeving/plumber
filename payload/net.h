#ifndef H_SOCKET
#define H_SOCKET

#define BUFFER_LENGTH 1024

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
    }

    printf("connected\n");
    return 0;
  }

  int write(SOCKET *s, char *msg) {
    char buffer[1024];
    sprintf(buffer, "%d~~%s||", G_UID, msg);
    if(send(*s, buffer, strlen(buffer), 0) < 0) {
      return 1;
    }
    return 0;
  }

  void close() {
    WSACleanup();
  }

  void listen(SOCKET *s, BOOL *run, void (*handle)(char*, BOOL*, BOOL*)) {
    BOOL listening = TRUE;
    do {
      int bc = 0;
      char buff[BUFFER_LENGTH];
      bc = recv(*s, buff, BUFFER_LENGTH, 0);
      if (bc > 0 ) {
        printf("Bytes received: %d  _  %s\n", bc, buff);
        handle(buff, &listening, run);
      } else if ( bc == 0 ) {
        printf("Connection closed\n");
        listening = FALSE;
      } else {
        printf("peer disconnected: %d\n", WSAGetLastError());
        listening = FALSE;
      }
    } while(listening);
  }

  void test() {
    SOCKET sock;
    createSocket(&sock);
    if(connect(&sock) == 0) {
      // TODO smthng here
    }
    closesocket(sock);
    close();
  }
}

#endif
