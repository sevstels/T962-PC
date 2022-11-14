/*
 * BLELabs BLE112-Protostick Demo Application for Tutorial #3
 * Contact: support@blelabs.com
 * Author: PeterVanPansen
 * Date: 12.08.2013
 * License: See license file in package
 */

#ifndef _BT_UTILS_H_
#define _BT_UTILS_H_

#include <stdint.h>

void printHexdump(unsigned char *data, int len, int offset);
void printAddr6(unsigned char *addr, char token);
void printUUID(unsigned char *uuid, int len);
unsigned char *reverseArray(unsigned char *array, size_t len);
void uuid128StrToArray(char *str_uuid, unsigned char *arr_uuid);

#endif
