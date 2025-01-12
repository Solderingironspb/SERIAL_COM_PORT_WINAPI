#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "serial.h"

extern HANDLE hSerial;
extern uint8_t Tx_Data[1024];	//Буфер под исходящие байты
extern uint8_t Rx_Data[1024]; 	//Буфер под входящие байты
extern DWORD recieve_bytes; 	//Сколько байт прочитано
extern HANDLE hThread;

/*Обработка входящих данных*/
void serial_rx_data_processing(void) {
	printf("Принято %ld байт: ", recieve_bytes);
	for (DWORD i = 0; i < recieve_bytes; i++) {
		printf("%02X ", Rx_Data[i]);
	}
	printf("\r\n");
}

int main(void) {
	SetConsoleOutputCP(CP_UTF8); // Установим кодировку UTF-8

	//Подключимся к COM порту
	if (serial_init("COM3", 115200, 8, NOPARITY, ONESTOPBIT) != 0) {
		return 1; // Завершение программы при ошибке инициализации
	}

	//Создадим поток для обработки входящих данных
	if (create_thread_serial_read_data() != 0) {
		return 1; //Завершение программы при ошибке создания потока
	};

	Tx_Data[0] = 4;
	Tx_Data[1] = 8;
	Tx_Data[2] = 15;
	Tx_Data[3] = 16;
	Tx_Data[4] = 23;
	Tx_Data[5] = 42;

	while (1) {

		//Отправим данные
		serial_write_data(hSerial, 6);
		Tx_Data[5]++;
		Sleep(5);

	}

	WaitForSingleObject(hThread, INFINITE); // Ожидание завершения потока
	CloseHandle(hThread);					//Закрытие потока
	CloseHandle(hSerial);					//Закрытие hSerial
	return 0;
}

