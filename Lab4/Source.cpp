#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <windows.h>
#include <devguid.h>
#include <setupapi.h>
#include "deviceInfo.h"
#include <cstdlib>
#include <ctime>
#pragma comment(lib, "setupapi.lib")

using namespace cv;
using namespace std;

void hideConsole();
void takeVideo();
LRESULT CALLBACK KeyboardFunc(int nCode, WPARAM wParam, LPARAM lParam);
HHOOK keyboardHook;
VideoWriter videoWriter;

int main()
{
    setlocale(LC_ALL, "Russian");

    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardFunc, NULL, 0);

    if (keyboardHook == NULL) {
        cerr << "Не удалось установить хук клавиатуры." << endl;
        return 1;
    }

    deviceInfo data;
    data.getData();
    data.showData(wcout);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);

    return 0;
}

LRESULT CALLBACK KeyboardFunc(int nCode, WPARAM wParam, LPARAM lParam) {

    KBDLLHOOKSTRUCT* keyCode = (KBDLLHOOKSTRUCT*)lParam;

    if (nCode >= 0) {
        if (keyCode->vkCode == VK_F1) {
            hideConsole();
            takeVideo();
        }

        if (keyCode->vkCode == VK_F2) {
            exit(0);
        }
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

void hideConsole() {
    HWND console = GetConsoleWindow();
    SetForegroundWindow(console);
    ShowWindow(GetForegroundWindow(), SW_HIDE);
}

string generateVideoName() {

    srand(static_cast<unsigned>(time(nullptr)));

    int randomNumber1 = rand() % 1000;
    string randomString1 = to_string(randomNumber1);

    string fileName = "D:/IpU/" + randomString1 + ".avi";

    return fileName;
}

void takeVideo() {
    VideoCapture camera(0);
    Mat frame;

    if (!camera.isOpened()) {
        return;
    }

    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
    string fileName = generateVideoName();
    Size frameSize(camera.get(CAP_PROP_FRAME_WIDTH), camera.get(CAP_PROP_FRAME_HEIGHT));
    videoWriter.open(fileName, codec, 30, frameSize);

    if (!videoWriter.isOpened()) {
        return;
    }

    while (true) {
        camera >> frame;
        if (frame.empty()) {
            break;
        }
        videoWriter.write(frame);
        if (waitKey(1) == 113) {
            break;
        }
    }

    camera.release();
    videoWriter.release();
}
