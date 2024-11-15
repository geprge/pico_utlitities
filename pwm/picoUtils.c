#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"

#define USB_RX_BUFFER_LENGTH	256

#define IS_NUM(C) (((C)>=0x30)&&((C)<=0x39))?1:0

int string2Int(char *str,int len){
	int retVal=0,posWeight=1;
	while(len){
		if(IS_NUM(str[len-1])){
			retVal+=(str[len-1]-0x30)*posWeight;
			posWeight=posWeight*10;
			len--;
		}
		else
			return -1;		
	}
	return(retVal);
}


int chkUsbCmd(char *retBuff){
	static char usbBuff[USB_RX_BUFFER_LENGTH];
	static short usbBuffIdx=0;
	int retVal=0,i;
	int c=getchar_timeout_us(0);	//Unblocked call
	if(c!=PICO_ERROR_TIMEOUT){
		switch(c&0xff){
			case 0x0d:
			case 0x0a:
				for(i=0;i<usbBuffIdx;i++){
					retBuff[i]=usbBuff[i];
				}
				retBuff[i]=0x00;	//end of string
				retVal=usbBuffIdx;
				usbBuffIdx=0;
			break;
			default:
				usbBuff[usbBuffIdx]=c&0xff;
				usbBuffIdx++;
				usbBuffIdx=usbBuffIdx%USB_RX_BUFFER_LENGTH;
				printf("%c",c&0xff);	//to give an echo
				retBuff[0]=0x00;		//to empty the buffer
				retVal=0;
		}//Switch char
	}//new char from usb
	return(retVal);
}
