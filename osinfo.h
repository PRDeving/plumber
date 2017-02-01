#ifndef H_OSINFO
#define H_OSINFO

// FIX this
BOOL is64bits() {
  return true;
}

void reconOS(TCHAR *ID, OSVERSIONINFOEX * info) {
  TCHAR tag[128];

  if (info -> dwMajorVersion == 10) {
    if (info -> wProductType == VER_NT_WORKSTATION){
      sprintf(tag, "10 (%d.%d) %s", info -> dwMajorVersion, info -> dwMinorVersion, is64bits() ? "64bits" : "32bits");
    } else {
      sprintf(tag, "Server 2016 (%d.%d)", info -> dwMajorVersion, info -> dwMinorVersion);
    }
  } else if (info -> dwMajorVersion == 6) {
    switch (info -> dwMinorVersion) {
      case 3:
      case 2:
        if (info -> wProductType == VER_NT_WORKSTATION){
          sprintf(tag, "8%s (%d.%d) %s",
              info -> dwMinorVersion == 3 ? ".1" : "",
              info -> dwMajorVersion,
              info -> dwMinorVersion,
              is64bits() ? "64bits" : "32bits");
        } else {
          sprintf(tag, "Server 2012%s (%d.%d)",
              info -> dwMinorVersion == 3 ? " R2" : "",
              info -> dwMajorVersion,
              info -> dwMinorVersion);
        }
        break;
      case 1:
      case 0:
        if (info -> wProductType == VER_NT_WORKSTATION){
          sprintf(tag, "%s (%d.%d) %s",
              info -> dwMinorVersion == 1 ? "7" : "Vista",
              info -> dwMajorVersion,
              info -> dwMinorVersion,
              is64bits() ? "64bits" : "32bits");
        } else {
          sprintf(tag, "Server 2008%s (%d.%d)",
              info -> dwMinorVersion == 1 ? " R2" : "",
              info -> dwMajorVersion,
              info -> dwMinorVersion);
        }
        break;
    }
  } else if (info -> dwMajorVersion == 5) {
    switch (info -> dwMinorVersion) {
      case 2:
        if (info -> wProductType == VER_NT_WORKSTATION){
          sprintf(tag, "XP Professional x64 (%d.%d) %s",
              info -> dwMajorVersion,
              info -> dwMinorVersion,
              is64bits() ? "64bits" : "32bits");
        } else {
          sprintf(tag, "Server 2003 (%d.%d)",
              info -> dwMajorVersion,
              info -> dwMinorVersion);
        }
        break;
      case 1:
      case 0:
        sprintf(tag, "%s (%d.%d) %s",
            info -> dwMinorVersion == 1 ? "XP" : "2000",
            info -> dwMajorVersion,
            info -> dwMinorVersion,
            is64bits() ? "64bits" : "32bits");
        break;
    }
  }
  sprintf(ID, "Windows %s\0", tag);
}

OSVERSIONINFOEX getOSInfo() {
  OSVERSIONINFOEX osvi;
  ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  GetVersionEx((LPOSVERSIONINFO)&osvi);

  return osvi;
}
#endif
