/*
 * serial.c
 *
 *  Created on: 12 янв. 2025 г.
 *      Author: Solderingiron
 */
#include "serial.h"

DCB dcb;
HANDLE hSerial;
BOOL fSuccess;
HANDLE hThread;

//Исходящие данные
uint8_t Tx_Data[1024];	//Буфер под исходящие байты
DWORD bytesWritten;		//Сколько байт записано
//Входящие данные
uint8_t Rx_Data[1024]; 	//Буфер под входящие байты
DWORD recieve_bytes; 	//Сколько байт прочитано

// Функция инициализации последовательного порта
int serial_init(char *com_port_name, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits) {
	hSerial = CreateFile(com_port_name,
	GENERIC_READ | GENERIC_WRITE, 0,
	NULL,
	OPEN_EXISTING, 0,
	NULL);

	if (hSerial == INVALID_HANDLE_VALUE) {
		printf("CreateFile failed with error %ld.\n", GetLastError());
		return (1);
	}

	SecureZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	fSuccess = GetCommState(hSerial, &dcb);
	if (!fSuccess) {
		printf("GetCommState failed with error %ld.\n", GetLastError());
		return (2);
	}

	dcb.BaudRate = BaudRate;
	dcb.ByteSize = ByteSize;
	dcb.Parity = Parity;
	dcb.StopBits = StopBits;

	fSuccess = SetCommState(hSerial, &dcb);
	if (!fSuccess) {
		printf("SetCommState failed with error %ld.\n", GetLastError());
		return (3);
	}

	fSuccess = GetCommState(hSerial, &dcb);
	if (!fSuccess) {
		printf("GetCommState failed with error %ld.\n", GetLastError());
		return (2);
	}

	serial_info_status(dcb);
	printf("%s успешно сконфигурирован\r\n", com_port_name);

	// Настройка таймаутов
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutConstant = 10;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 10;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	SetCommTimeouts(hSerial, &timeouts);

	return 0; // Успешная инициализация
}

// Функция для вывода параметров COM-порта
void serial_info_status(DCB dcb) {
	printf("Параметры COM порта:\nBaudRate = %ld\nByteSize = %d\n", dcb.BaudRate, dcb.ByteSize);
	printf("Parity = ");
	switch (dcb.Parity) {
	case 0:
		printf("NOPARTITY\r\n");
		break;
	case 1:
		printf("ODDPARTITY\r\n");
		break;
	case 2:
		printf("EVENPARTITY\r\n");
		break;
	case 3:
		printf("MARKPARTITY\r\n");
		break;
	case 4:
		printf("SPACEPARTITY\r\n");
		break;
	}
	printf("StopBits = ");
	switch (dcb.StopBits) {
	case 0:
		printf("ONESTOPBIT\r\n");
		break;
	case 1:
		printf("ONE5STOPBITS\r\n");
		break;
	case 2:
		printf("TWOSTOPBITS\r\n");
		break;
	}

}

// Функция потока для приема данных из последовательного порта
DWORD WINAPI thread_serial_read_data(LPVOID lpParam) {
	while (1) {
		if (ReadFile(hSerial, Rx_Data, sizeof(Rx_Data), &recieve_bytes, NULL)) {
			if (recieve_bytes > 0) {

				/*=================Здесь работаем с принятыми данными=================*/
				serial_rx_data_processing();
				/*=================Здесь работаем с принятыми данными=================*/
			}
		} else {
			// Обработка ошибок чтения
			DWORD dwError = GetLastError();
			if (dwError != ERROR_IO_PENDING) {
				printf("Ошибка чтения из последовательного порта: %ld\n", dwError);
			}
		}
	}
	return 0;
}

__attribute__((weak)) void serial_rx_data_processing(void) {

}

int create_thread_serial_read_data(void) {
	// Создание потока для чтения данных
	hThread = CreateThread(NULL, 0, thread_serial_read_data, NULL, 0, NULL);
	if (hThread == NULL) {
		printf("Не удалось создать поток: %ld\n", GetLastError());
		CloseHandle(hSerial);
		return 1;
	}
	return 0;
}

int serial_write_data(HANDLE *hSerial, uint16_t size) {
	if (!WriteFile(hSerial, Tx_Data, size, &bytesWritten, NULL)) {
		printf("Ошибка записи в последовательный COM порт\n");
		CloseHandle(hSerial);
		return 1;
	}
	return 0;
}

