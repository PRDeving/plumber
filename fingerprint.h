#ifndef H_OSINFO
#define H_OSINFO

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

namespace fingerprint {
  BOOL is64bits() {
    LPFN_ISWOW64PROCESS fnIsWow64Process;
    BOOL bIsWow64 = FALSE;
    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

    if(NULL != fnIsWow64Process) {
      if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64)) {
        //handle error
      }
    }
    return bIsWow64;
  }

  // FIX this
  // BOOL is64bits() {
  //   return true;
  // }

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

  unsigned int getVolumeHash() {
    DWORD serialNum = 0;
    GetVolumeInformation( "c:\\", NULL, 0, &serialNum, NULL, NULL, NULL, 0 );
    unsigned int hash = (unsigned int)(( serialNum + ( serialNum >> 16 )) & 0xFFFF );

    return hash;
  }

  unsigned int getCPUHash() {
    int cpuinfo[4] = { 0, 0, 0, 0 };
    __cpuid(cpuinfo, 0);
    unsigned int hash = 0;
    unsigned int* ptr = (unsigned int*)(&cpuinfo[0]);
    for (unsigned int i = 0; i < 4; i++)
      hash += ptr[i];

    return hash;
  }

  const char* getMachineName() {
    static char computerName[1024];
    DWORD size = 1024;
    GetComputerName( computerName, &size );
    return &(computerName[0]);
  }

  unsigned int getUID() {
    return (getVolumeHash() | getCPUHash() | (unsigned int) getMachineName());
  }

  void test() {
    printf("\nUID: 0x%x\nMachine: %s\nVol H: 0X%x\nCPU H: 0x%x\n",
        getUID(),
        getMachineName(),
        getVolumeHash(),
        getCPUHash());
  }
}

#endif
