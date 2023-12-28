#include <windows.h>
#include <powrprof.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <batclass.h>
#include <poclass.h>
#include <setupapi.h>
#include <devguid.h>

#pragma comment(lib, "powrprof.lib")
#pragma comment (lib, "setupapi.lib")

void printBatteryChemistry()
{
    std::string batteryType;

    HDEVINFO DeviceInfoSet = SetupDiGetClassDevs(
        &GUID_DEVCLASS_BATTERY,
        NULL,
        NULL,          
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
        
    if (DeviceInfoSet)
    {
        SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
        DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);

        SetupDiEnumDeviceInterfaces(DeviceInfoSet, 0, &GUID_DEVCLASS_BATTERY, 0, &DeviceInterfaceData);

        DWORD cbRequired = 0;
        SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, NULL, 0, &cbRequired, 0);

        PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, cbRequired); 
        if (pdidd)
        {
            pdidd->cbSize = sizeof(*pdidd);
           
            SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, pdidd, cbRequired, &cbRequired, 0);

            HANDLE batteryHandle = CreateFile(pdidd->DevicePath,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,	
                OPEN_EXISTING,	
                FILE_ATTRIBUTE_NORMAL,
                NULL);

            BATTERY_QUERY_INFORMATION bqi = { 0 };
            
            DeviceIoControl(batteryHandle,
                IOCTL_BATTERY_QUERY_TAG,
                NULL, NULL, &bqi.BatteryTag, sizeof(bqi.BatteryTag), NULL, NULL);

            BATTERY_INFORMATION bi;
            DeviceIoControl(batteryHandle, IOCTL_BATTERY_QUERY_INFORMATION, &bqi, sizeof(bqi), &bi, sizeof(bi), NULL, NULL);

            batteryType = reinterpret_cast<char*>(bi.Chemistry);

            CloseHandle(batteryHandle);
            LocalFree(pdidd);
        }
        SetupDiDestroyDeviceInfoList(DeviceInfoSet);
    }

    std::cout << batteryType.substr(0, 4) << std::endl;

}

void printBattery(int second) {
    system("cls");
    std::cout << second << std::endl;
    SYSTEM_POWER_STATUS powerStatus;
    if (!GetSystemPowerStatus(&powerStatus))
    {
        std::cout << "Failed to get system power status." << std::endl;
        return;
    }
    std::cout << "Power source: ";
    switch (powerStatus.ACLineStatus)
    {
    case AC_LINE_OFFLINE:
        std::cout << "Battery";
        break;
    case AC_LINE_ONLINE:
        std::cout << "AC power";
        break;
    case AC_LINE_BACKUP_POWER:
        std::cout << "Backup (UPS)";
        break;
    case AC_LINE_UNKNOWN:
    default:
        std::cout << "Unknown";
        break;
    }
    std::cout << std::endl;

    std::cout << "Battery state: ";
    switch (powerStatus.BatteryFlag)
    {
    case BATTERY_FLAG_HIGH:
        std::cout << "High";
        break;
    case 0:
        std::cout << "Normal";
        break;
    case BATTERY_FLAG_LOW:
        std::cout << "Low";
        break;
    case BATTERY_FLAG_CRITICAL:
        std::cout << "Critical";
        break;
    case BATTERY_FLAG_CHARGING:
        std::cout << "Charging";
        break;
    case 9:
        std::cout << "Charging";
        break;
    case 10:
        std::cout << "Charging";
        break;
    case BATTERY_FLAG_NO_BATTERY:
        std::cout << "No battery";
        break;
    case BATTERY_FLAG_UNKNOWN:
    default:
        std::cout << "Unknown";
        break;
    }
    std::cout << std::endl;

    std::cout << "Battery level: " << static_cast<int>(powerStatus.BatteryLifePercent) << "%" << std::endl;

    std::cout << "Power saving mode: ";
    switch (powerStatus.SystemStatusFlag)
    {
    case 0:
        std::cout << "Off";
        break;
    case 1:
        std::cout << "On";
        break;
    }
    std::cout << std::endl;

    std::cout << "Battery time remaining: ";
    if (powerStatus.BatteryLifeTime == BATTERY_LIFE_UNKNOWN)
    {
        std::cout << "Unknown";
    }
    else
    {
        int hours = powerStatus.BatteryLifeTime / 3600;
        int minutes = (powerStatus.BatteryLifeTime % 3600) / 60;
        int seconds = powerStatus.BatteryLifeTime % 60;
        std::cout << hours << "h " << minutes << "m " << seconds << "s";
    }
    std::cout << std::endl;

    std::cout << "Full battery time : ";
    if (powerStatus.BatteryFullLifeTime == BATTERY_LIFE_UNKNOWN)
    {
        std::cout << "Unknown";
    }
    else
    {
        int hours = powerStatus.BatteryFullLifeTime / 3600;
        int minutes = (powerStatus.BatteryFullLifeTime % 3600) / 60;
        std::cout << hours << "h " << minutes << "m";
    }
    std::cout << std::endl;

    std::cout << "Battery type : ";
    printBatteryChemistry();

}

int main() {
    while (1) {
        system("cls");
        std::cout << "0 - monitor battery, 1 - sleep, 2 - hibernate, 3 - end" << std::endl;

        int choice = 4;
        std::cin >> choice;

        if (choice == 0) {
            int amount;
            std::cout << "Enter amount of seconds: ";
            std::cin >> amount;
            for (int i = 0; i < amount; i++) {
                printBattery(amount - i);
                Sleep(1000);
            }
        }

        if (choice == 1) {
            if (IsPwrSuspendAllowed()) {
                if (!SetSuspendState(FALSE, FALSE, FALSE))
                {
                    std::cout << "Failed to enter sleep mode" << std::endl;
                    continue;
                }
            }
            else std::cout << "Sleep mode not allowed";
        }

        if (choice == 2) {
            if (IsPwrHibernateAllowed()) {
                if (!SetSuspendState(TRUE, FALSE, FALSE))
                {
                    std::cout << "Failed to enter hibernation" << std::endl;
                    continue;
                }
            }
            else std::cout << "Hibernation not allowed";
        };

        if (choice == 3) break;
    };

    return 0;
};
