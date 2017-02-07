#ifndef H_SOCKET
#define H_SOCKET

#define BUFFER_LENGTH MAX_PATH

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
    std::string buffer;
    buffer += std::to_string(G_UID);
    buffer += "~~";
    buffer += msg;
    buffer += "||";
    if(send(*s, buffer.c_str(), buffer.length(), 0) < 0) {
      return 1;
    }
    return 0;
  }

  int recieveFile(unsigned int size, char *path) {
    SOCKET sock;
    createSocket(&sock);

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ADDRESS);
    server.sin_family = AF_INET;
    server.sin_port = htons(1338);

    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
      printf("dataTCP Conn error\n");
      closesocket(sock);
      return 1;
    }

    printf("dataTCP Connected\n");

    BOOL listening = TRUE;
    char *buff = (char*)malloc(sizeof(char) * size);
    int bc = recv(sock, buff, size, 0);

    if (bc > 0 ) {
      printf("Bytes received: %d/%d\n", bc, size);
    } else if ( bc == 0 ) {
      printf("DataTCP Connection closed\n");
      closesocket(sock);
      return 1;
    } else {
      printf("DataTCP peer disconnected: %d\n", WSAGetLastError());
      closesocket(sock);
      return 2;
    }

    FILE *fileptr;
    char *buffer;
    long filelen;

    fileptr = fopen(path, "wb");
    fwrite(buff, size, 1, fileptr);
    fclose(fileptr);
    closesocket(sock);
    free(buff);

    return 0;
  }

  int sendFile(SOCKET *s, char *path) {

    SOCKET sock;
    createSocket(&sock);

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ADDRESS);
    server.sin_family = AF_INET;
    server.sin_port = htons(1338);

    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
      printf("dataTCP Conn error\n");
      closesocket(sock);
      return 1;
    }

    printf("dataTCP Connected\n");

    FILE *fileptr;
    char *buffer;
    long filelen;

    fileptr = fopen(path, "rb");
    fseek(fileptr, 0, SEEK_END); 
    filelen = ftell(fileptr);
    rewind(fileptr);

    buffer = (char *)malloc((filelen + 1) * sizeof(char));
    fread(buffer, filelen, 1, fileptr);
    fclose(fileptr);

    int sent = send(sock, buffer, filelen, 0);
    printf("\n%d/%d bytes sent\n", sent, filelen);
    closesocket(sock);
    free(buffer);

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
