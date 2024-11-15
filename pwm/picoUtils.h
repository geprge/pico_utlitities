
#ifndef USB_UTILS_H_INCLUDED
#define USB_UTILS_H_INCLUDED

#include <stdio.h>
#include "pico/stdlib.h"

#define USB_RX_BUFFER_LENGTH	256

#define IS_NUM(C) (((C)>=0x30)&&((C)<=0x39))?1:0

// return true, if there is a new comd; else false
bool chkUsbCmd(char *retBuff);
int string2Int(char *buff, int len);

#endif
