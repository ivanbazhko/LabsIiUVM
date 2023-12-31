#include "stdafx.h"
#include "hexioctrl.h"
#include <conio.h>
#include <iostream>
#include <stdlib.h>

const int IDENTIFY_DEVICE = 0xEC;
const int IDENTIFY_PACKET_DEVICE = 0xA1;
 
const int dataRegister[2] = {0x1F0, 0x170};
const int DH_register[2] = {0x1F6, 0x176};
const int StateCommandRegister[2] = {0x1F7, 0x177};
const int altStateRegister[2] = {0x3F6, 0x376};
 
unsigned short data[256];
 
void WaitDeviceBusy(int channelNum);
bool getDeviceInfo(int devNum, int channelNum);
void showTable();
bool waitReady(int channelNum);
  
int main() {
    ALLOW_IO_OPERATIONS;
	system("CLS");
	for(int channel = 0; channel <= 1; channel++){
        for(int device = 0; device <= 1; device++) {
			if(getDeviceInfo(device, channel)) {
                printf("\nChannel %d, Device %d\n", channel, device);
                showTable();
            };
        };
	};
    system("pause");
    return 0;
}
 
bool waitReady(int channelNum) {
    for (int i = 0; i < 1000; i++) {
        unsigned char state = _inp(altStateRegister[channelNum]);
		if(state & (1 << 6)) {
			return true; 
		};
    };
    return false;
}
 
void WaitDeviceBusy(int channelNum) {
    unsigned char state;
	do {
		state = _inp(altStateRegister[channelNum]);
	} while (state & (1 << 7));
}

bool getDeviceInfo(int devNum, int channelNum) {   
    const int commands[2] = {IDENTIFY_PACKET_DEVICE, IDENTIFY_DEVICE};
    for (int i = 0; i < 2; i++) {
        WaitDeviceBusy(channelNum);
        unsigned char regData = (devNum << 4) + (7 << 5);
        _outp(DH_register[channelNum], regData);
		if(!waitReady(channelNum)) {
			return false;    
		};
        _outp(StateCommandRegister[channelNum], commands[i]);
 
        WaitDeviceBusy(channelNum);
                
        unsigned char curState = _inp(StateCommandRegister[channelNum]);
		if(!(curState & (1 << 3))) {
            if(i == 1) return false;
            continue;
        } else break;
    };

	for(int i = 0; i < 256; i++) {
        data[i] = _inpw(dataRegister[channelNum]);
	};
    return true;
}
 
void showTable() {
    printf("Device Model: ");
	for(int i = 27; i <= 46; i++) {
        printf("%c%c", data[i] >> 8, data[i] & 0x00FF); 
	};
	printf("\n");
 
    printf("Serial Number: ");
	for(int i = 10; i <= 19; i++) {
        printf("%c%c", data[i] >> 8, data[i] & 0x00FF);
	};
	printf("\n");
 
    printf("Version: ");
	for(int i = 23; i <= 26; i++) {
        printf("%c%c", data[i] >> 8, data[i] & 0x00FF);
	}
	printf("\n");
 
    printf("Interface: ");
	if(data[0] & (1 << 15)) {
		printf("ATAPI\n");
	} else printf("ATA\n");
 
    if(!(data[0] & (1 << 15))){ 
		printf("Size: %.2lf GB\n", (long double)(((unsigned long *)data)[30]) * 512 / (1024 * 1024 * 1024));
    };
 
	printf("\n");
    printf("PIO:\n");
	if(data[64] & 1) printf("%s", "PIO 3\n");
	if(data[64] & 2) printf("%s", "PIO 4\n");
 
	printf("\n");
    printf("Multiword DMA:\n");
	if(data[63] & 1) printf("%s", "MWDMA 0\n");
	if(data[63] & 2) printf("%s", "MWDMA 1\n");
	if(data[63] & 4) printf("%s", "MWDMA 2\n");
 
	printf("\n");
    printf("Ultra DMA:\n");
	if(data[88] & 1) printf("%s", "UDMA Mode 0\n" );
	if(data[88] & (1 << 1)) printf("%s", "UDMA 1\n");
	if(data[88] & (1 << 2)) printf("%s", "UDMA 2\n");
	if(data[88] & (1 << 3)) printf("%s", "UDMA 3\n");
	if(data[88] & (1 << 4)) printf("%s", "UDMA 4\n");
	if(data[88] & (1 << 5)) printf("%s", "UDMA 5\n");

	printf("\n");
    printf("ATA Versions:\n");
	if(data[80] & (1 << 1)) printf("%s", "ATA 1\n");
	if(data[80] & (1 << 2)) printf("%s", "ATA 2\n");
	if(data[80] & (1 << 3)) printf("%s", "ATA 3\n");
	if(data[80] & (1 << 4)) printf("%s", "ATA 4\n");
	if(data[80] & (1 << 5)) printf("%s", "ATA 5\n");
	if(data[80] & (1 << 6)) printf("%s", "ATA 6\n");
	if(data[80] & (1 << 7)) printf("%s", "ATA 7\n");
    
    printf("______________________________________________\n\n");
}