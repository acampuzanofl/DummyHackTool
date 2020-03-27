#include <windows.h>
#include <iostream>

DWORD getPID()
{
	int pid;
		while (true)
		{
			std::cout << "Enter Process ID: ";
			std::cin >> pid;
			HANDLE isProcess = OpenProcess(PROCESS_VM_READ, false, pid);
			if (isProcess) { CloseHandle(isProcess); std::cin.ignore(INT_MAX, '\n'); break; }
			std::cout << "Invalid process ID." << std::endl;
			std::cin.clear();
			std::cin.ignore(INT_MAX,'\n');
		}
	return (DWORD)pid;
}

DWORD scanInt( DWORD pid)
{
	//init vars
	int readValue = 0;
	DWORD lpBaseAddress;	
	
	//obtain process handle
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ, false, pid);
	if (hProcess == NULL) { std::cout << "error open process: " << GetLastError(); return 0; }
	
	//input address for scan
	std::cout << "Enter Address: ";
	std::cin >> std::hex >> lpBaseAddress;
	if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(MAXDWORD, '\n'); }

	//read memory
	if (!ReadProcessMemory(hProcess, (LPCVOID)lpBaseAddress, &readValue, sizeof(readValue), NULL)) { std::cout << "error in reading memory: " << GetLastError() << std::endl; return 0; }
	std::cout << "Value = " << readValue;
	

	//cleanup
	if (!CloseHandle(hProcess)) { std::cout << "error in closing handle: " << GetLastError() << std::endl; return 0; }
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------------------------------------------------\n";
	
	return lpBaseAddress;
}

DWORD scanString( DWORD pid )
{
	//init vars
	char readValue = 0;
	DWORD lpBaseAddress;
	std::string storeString = "";
	const int STRING_MAX_SCAN = 128;

	//obtain process handle
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ, false, pid);
	if (hProcess == NULL) { std::cout << "error open process: " << GetLastError(); return 0; }

	//input address for scan
	std::cout << "Enter Address: ";
	std::cin >> std::hex >> lpBaseAddress;
	if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(MAXDWORD, '\n'); }

	//read memory, check for valid characters, terminate on invalid character
	for (int offset = 0; offset < STRING_MAX_SCAN; offset++)
	{	
		bool isValid = false;
		if (!ReadProcessMemory(hProcess, (LPCVOID)(lpBaseAddress+offset), &readValue, sizeof(readValue), NULL)) { std::cout << "error in reading memory: " << GetLastError() << std::endl; return 0; }

		//loop over ascii table
		for (char i = 0x20; i <= 0x7e; i++)
		{	
			if (readValue == i) { storeString = storeString + readValue; isValid = true;  break; }
		}
		if (!isValid) { break; }
	}
	std::cout << "String: " << storeString << std::endl;


	//cleanup
	if (!CloseHandle(hProcess)) { std::cout << "error in closing handle: " << GetLastError() << std::endl; return 0; }
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------------------------------------------------\n";
	
	return lpBaseAddress;
}

DWORD scanPointer(DWORD pid)
{
	//init vars
	int* readValue = 0;
	DWORD lpBaseAddress;

	//obtain process handle
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ, false, pid);
	if (hProcess == NULL) { std::cout << "error open process: " << GetLastError(); return 0; }

	//input address for scan
	std::cout << "Enter Address: ";
	std::cin >> std::hex >> lpBaseAddress;
	if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(MAXDWORD, '\n'); }

	//read memory
	if (!ReadProcessMemory(hProcess, (LPCVOID)lpBaseAddress, &readValue, sizeof(readValue), NULL)) { std::cout << "error in reading memory: " << GetLastError() << std::endl; return 0; }
	std::cout << "Pointer            = 0x" << readValue << std::endl;
	if (!ReadProcessMemory(hProcess, (LPCVOID)readValue,     &readValue, sizeof(readValue), NULL)) { std::cout << "error in reading memory: " << GetLastError() << std::endl; return 0; }
	std::cout << "Value of Pointer   = "   <<  std::dec << (int)readValue << std::endl;

	//cleanup
	if (!CloseHandle(hProcess)) { std::cout << "error in closing handle: " << GetLastError() << std::endl; return 0; }
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------------------------------------------------\n";

	return lpBaseAddress;
}

void writeInt(DWORD pid, DWORD lpBaseAddress)
{
	//init vars
	int writeValue = 0;

	//obtain process handle
	HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, pid);
	if (hProcess == NULL) { std::cout << "error open process: " << GetLastError(); return; }

	//input value for overwrite
	std::cout << "Enter Value to Overwrite: ";
	std::cin >> std::dec >> writeValue;
	if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(INT_MAX, '\n'); }

	//write memory
	if (!WriteProcessMemory(hProcess, (LPVOID)lpBaseAddress, &writeValue, sizeof(writeValue), NULL)) { std::cout << "error in writting memory: " << GetLastError() << std::endl; return; }
	std::cout << "Overwrite Succesful ";


	//cleanup
	if (!CloseHandle(hProcess)) { std::cout << "error in closing handle: " << GetLastError() << std::endl; return; }
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------------------------------------------------\n";
}

void writeString(DWORD pid, DWORD lpBaseAddress)
{
	//init vars 
	std::string writeValue = "";

	//obtain process handle
	HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, pid);
	if (hProcess == NULL) { std::cout << "error open process: " << GetLastError(); return; }

	//input value for overwrite
	std::cout << "Enter Value to Overwrite: ";
	std::cin >> std::noskipws >> writeValue;

	for (int offset = 0; offset < writeValue.size(); offset++)
	{
		//write memory
		if (!WriteProcessMemory(hProcess, (LPVOID)(lpBaseAddress + offset), &writeValue[offset], sizeof(char), NULL)) { std::cout << "error in writting memory: " << GetLastError() << std::endl; return; }
	}
	std::cout << "Overwrite Succesful ";


	//cleanup
	if (!CloseHandle(hProcess)) { std::cout << "error in closing handle: " << GetLastError() << std::endl; return; }
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------------------------------------------------\n";
}

int main()
{	

	DWORD lpBaseAddress = 0;
	char userAnswer = 0;

	//attach to process
	DWORD pid = getPID();

	while (true)
	{
		//choose scan type
		int scanType = 0;
		std::cout << "Enter Type to scan for(1=Int 2=String 3=Pointer): ";
		std::cin >> scanType;
		std::cout << std::endl;

		switch (scanType)
		{
			case 1:
				std::cin.ignore(INT_MAX, '\n');
				std::cout << "Int Scan\n";
				lpBaseAddress = scanInt(pid);
				std::cout << "Would you like to overwrite this value (y/n)? ";
				std::cin >> userAnswer;
				if (userAnswer == 'y' || userAnswer == 'Y') { std::cin.ignore(CHAR_MAX, '\n'); writeInt(pid, lpBaseAddress); }
				std::cin.ignore(CHAR_MAX, '\n');
				break;
			case 2:
				std::cin.ignore(INT_MAX, '\n');
				std::cout << "String Scan\n";
				lpBaseAddress = scanString(pid);
				std::cout << "Would you like to overwrite this value (y/n)? ";
				std::cin >> userAnswer;
				if (userAnswer == 'y' || userAnswer == 'Y') { std::cin.ignore(CHAR_MAX, '\n');  writeString(pid, lpBaseAddress); }
				std::cin.ignore(CHAR_MAX, '\n');
				break;
			case 3:
				std::cin.ignore(INT_MAX, '\n');
				std::cout << "Pointer Scan\n";
				lpBaseAddress = scanPointer(pid);
				break;
			default:
				std::cout << "Not a valid option\n";
				std::cin.clear();
				std::cin.ignore(INT_MAX, '\n');
		}

	}
}
	
	


