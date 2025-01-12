/*
 * serial.h
 *
 *  Created on: 12 янв. 2025 г.
 *      Author: Solderingiron
 */

#ifndef INC_SERIAL_H_
#define INC_SERIAL_H_

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdint.h>



int serial_init(char *com_port_name, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits);
void serial_info_status(DCB dcb);
DWORD WINAPI thread_serial_read_data(LPVOID lpParam);
int create_thread_serial_read_data(void);
void serial_rx_data_processing(void);
int serial_write_data(HANDLE *hSerial, uint16_t size);

#endif /* INC_SERIAL_H_ */
