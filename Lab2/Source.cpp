#include "stdafx.h"
#include "hexioctrl.h"
#include "(PCI_DEVS)pci_codes.h"
#include <conio.h>
#include <stdlib.h>

unsigned long readReg(unsigned long configAddress) {
	unsigned long regData = 0;
	//int error = 0;
	//unsigned port1 = 0xCF8;
	//unsigned port2 = 0xCFC;
	//_outp(port1, configAddress);
	//error = regData = _inp(port2);
	//if(error != 0) return -1;
	__asm {
		mov eax, configAddress
		mov dx, 0CF8h
		out dx, eax
		mov dx, 0CFCh
		in eax, dx 
		mov regData, eax
	}
	return regData;
}

unsigned long calculateAddress(int bus, int device, int function) {
	unsigned long address = 1;
	int reg = 0x00;
	address = address << 15;
	address += bus;
	address = address << 5;      
	address += device;
	address = address << 3;
	address += function;
	address = address << 8;
	address += reg;
	return address;
}

void findNames(unsigned long DeviceId, unsigned long VendorId) {
	for(int i = 0; i < PCI_DEVTABLE_LEN; i++) {
		if(PciDevTable[i].VenId == VendorId && PciDevTable[i].DevId == DeviceId) {		
			printf("%s, %s, ", PciDevTable[i].Chip, PciDevTable[i].ChipDesc);
		}
	}
	for(int i = 0; i < PCI_VENTABLE_LEN; i++) {
		if(PciVenTable[i].VenId == VendorId) {		
			printf("%s\n", PciVenTable[i].VenFull);	
		}
	}
}

void checkDev(int busnum, int devnum, int funcnum) {
	unsigned long configAddress = calculateAddress(busnum, devnum, funcnum);
	unsigned long RegData = readReg(configAddress);   
	if (RegData == -1) return;
	unsigned long DeviceID = RegData >> 16;
	unsigned long VendorID = RegData - (DeviceID << 16);
	printf("Device ID: %x, Vendor ID: %x\n", DeviceID, VendorID);
	findNames(DeviceID, VendorID);
	printf("\n");	
}

int main() {
	ALLOW_IO_OPERATIONS;
	system("CLS");
	for(int busnum = 0; busnum < 256; busnum++) {
		for(int devnum = 0; devnum < 32; devnum++) {
			for(int funcnum = 0; funcnum < 8; funcnum++) {
				checkDev(busnum, devnum, funcnum);		
			}
		}
	}
	system("pause");
	return 0;
}
