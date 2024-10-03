#include <windows.h>
#include <gdiplus.h>
#include <iostream>

#pragma comment (lib,"Gdiplus.lib")

using namespace Gdiplus;
using namespace std;


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size)); // malloc pour une allocation dynamique qui dépend des configs de la machine
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

void CaptureScreenshot(const wchar_t* filename)
{
    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Get the desktop window handle
    HWND hwndDesktop = GetDesktopWindow();

    // Get the size of the desktop
    RECT desktopRect;
    GetWindowRect(hwndDesktop, &desktopRect);
    int width = desktopRect.right;
    int height = desktopRect.bottom;

    // Create a device context for the entire screen
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMemory = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);

    // Select the bitmap into the memory device context
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmap);

    // Copy the screen content into the memory device context
    BitBlt(hdcMemory, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

    // Create a GDI+ Bitmap from the HBITMAP
    Bitmap bitmap(hBitmap, NULL);

    // Get the CLSID of the PNG encoder
    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);

    // Save the screenshot to a file
    bitmap.Save(filename, &pngClsid, NULL);

    // Cleanup
    SelectObject(hdcMemory, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMemory);
    ReleaseDC(NULL, hdcScreen);

    // Shutdown GDI+
    GdiplusShutdown(gdiplusToken);
}

int main() //g++ -o capture_screenshot.exe main.cpp -lgdi32 -lgdiplus
{
    CaptureScreenshot(L"screenshot.png");
    cout << L"Screenshot saved as screenshot.png" << endl;
    return 0;
}