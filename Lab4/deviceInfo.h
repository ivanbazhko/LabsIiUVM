#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <setupapi.h>
#include <devguid.h>
#include <cwchar>

using namespace std;

class deviceInfo {

private:
    WCHAR friendlyName[255];
    WCHAR deviceDesc[255];
    WCHAR mfg[255];
    WCHAR hardwareID[255];
    WCHAR instanceID[255];

public:
    deviceInfo() {
        this->deviceDesc[0] = L'\0';
        this->friendlyName[0] = L'\0';
        this->mfg[0] = L'\0';
        this->hardwareID[0] = L'\0';
        this->instanceID[0] = L'\0';
    }

    void getData() {
        HDEVINFO hDevInfo;
        SP_DEVINFO_DATA DeviceInfoData;

        WCHAR d_friendlyName[255];
        WCHAR d_deviceDesc[255];
        WCHAR d_mfg[255];
        WCHAR d_hardwareID[255];
        WCHAR d_instanсeID[255];

        DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        hDevInfo = SetupDiGetClassDevsW(&GUID_DEVCLASS_CAMERA, 0, 0, DIGCF_PRESENT);
        SetupDiEnumDeviceInfo(hDevInfo, 0, &DeviceInfoData);

        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData,
            SPDRP_FRIENDLYNAME, NULL, (PBYTE)d_friendlyName, 255, NULL)) {
            wcscpy_s(this->friendlyName, d_friendlyName);
        }

        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData,
            SPDRP_DEVICEDESC, NULL, (PBYTE)d_deviceDesc, 255, NULL)) {
            wcscpy_s(this->deviceDesc, d_deviceDesc);
        }

        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData,
            SPDRP_MFG, NULL, (PBYTE)d_mfg, 255, NULL)) {
            wcscpy_s(this->mfg, d_mfg);
        }

        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData,
            SPDRP_HARDWAREID, NULL, (PBYTE)d_hardwareID, 255, NULL)) {
            wcscpy_s(this->hardwareID, d_hardwareID);
        }

        if (SetupDiGetDeviceInstanceIdW(hDevInfo, &DeviceInfoData,
            (PWSTR)d_instanсeID, 255, NULL)) {
            wcscpy_s(this->instanceID, d_instanсeID);
        }

        SetupDiDestroyDeviceInfoList(hDevInfo);
    };

    void showData (std::wostream& out) {
        if (this->friendlyName[0] != L'\0') out << L"Название: " << this->friendlyName << endl;
        if (this->deviceDesc[0] != L'\0') out << L"Описание устройства: " << this->deviceDesc << endl;
        if (this->mfg[0] != L'\0') out << L"Производитель: " << this->mfg << endl;
        if (this->hardwareID[0] != L'\0') out << L"Hardware ID: " << this->hardwareID << endl;
        if (this->instanceID[0] != L'\0') out << L"Instance ID: " << this->instanceID << endl;
    };

};
