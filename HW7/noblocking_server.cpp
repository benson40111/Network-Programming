#include<iostream>
#include<winsock2.h>
#include<list>
#include<client.h>

#pragma comment(lib, "ws2_32.lib")
using namespace std;
typedef list<CClient*> CLIENTLIST;

#define SERVERPORT 5566
#define SERVER_SETUP_FAIL 1
#define TIMEFOR_THREAD_EXIT 5000
#define TIMEFOR_THREAD_HELP 1500
#define TIMEFOR_THREAD_SLEEP 500

HANDLE hThreadAccept;
HANDLE hThreadHelp;
SOCKET sServer;
BOOL bServerRunning;
HANDLE hServerEvent;
CLIENTLIST clientlist;
CRITICAL_SECTION csClientList;

BOOL InitServer(void);
BOOL StartService(void);
void StopService(void);
BOOL CreateHelperAndAcceptThread(void);
void ExitServer(void);

void InitMember(void);
BOOL InitSocket(void);

void ShowTipMsg(BOOL bFirstInput);
void ShowServerStartMsg(BOOL bSuc);
void ShowServerExitMsg(void);

DWORD __stdcall HelperThread(void *pParam);
DWORD __stdcall AcceptThread(void *pParam);
void ShowConnectNum();

int main(){
	if (!InitServer()){
		ExitServer();
		return SERVER_SETUP_FAIL;
	}

	if (!StartService()){
		ShowServerStartMsg(FALSE);
		ExitServer();
		return SERVER_SETUP_FAIL;
	}

	StopService();

	ExitServer();

	return 0;
}

BOOL InitServer(void){
	InitMember();

	if (!InitSocket())
		return FALSE;
	return TRUE;
}

void InitMember(void){
	InitializeCriticalSection(&csClientList);
	hServerEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hThreadAccept = NULL;
	hThreadHelp = NULL;
	sServer = INVALID_SOCKET;
	bServerRunning = FALSE;
	clientlist.clear();
}

BOOL InitSocket(void){
	int retVal;

	WSADATA wsData;
	retVal = WSAStartup(MAKEWORD(2, 2), &wsData);

	sServer = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == sServer)
		return FALSE;
	unsigned long ul = 1;
	retVal = ioctlsocket(sServer, FIONBIO, (unsigned long*)&ul);
	if (SOCK_ERROR == retVal)
		return FALSE;
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERVERPORT);
	servAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	retVal = bind(sServer, (struct sockaddr*)&serAddr, sizeof(serAddr));
	if (SOCK_ERROR == retVal)
		return FALSE;
	retVal = listen(sServer, SOMAXCONN);
	if (SOCKET_ERROR == retVal)
		return FALSE;
	return TRUE;
}

BOOL StartService(void){
	BOOL retVal = TRUE;
	ShowTipMsg(TRUE);
	char cInput;
	do {
		cin >> cInput;
		if ('s' == cInput || 'S' == cInput){
			if (CreateHelperAndAcceptThread())	
				ShowServerStartMsg(TRUE);
			else
				retVal = FALSE;
			break;
		}
		else
			ShowTipMsg(TRUE);
	}while (cInput != 's' && cInput != 'S');
	return retVal;
}

void StopService(void){
	BOOL retVal = TRUE;
	ShowTipMsg(FALSE);
	char cInput;
	for (;bServerRunning;){
		cin >> cInput;
		if (cInput == 'E' || cInput == 'e'){
			if (IDOK == MessageBox(NULL, "Are you sure?", "Server", MB_OKCANCEL))
				break;
			else
				Sleep(TIMEFOR_THREAD_EXIT);
		}
		else
			Sleep(TIMEFOR_THREAD_EXIT);
	}

	bServerRunning = FALSE;
	ShowServerExitMsg();
	Sleep(TIMEFOR_THREAD_EXIT);
	WaitForSingleObject(hServerEvent, INFINITE);
	return;
}

void ShowTipMsg(BOOL bFirstInput){
	if (bFirstInput){
		cout << endl;
		cout << endl;
		cout << "**********************" << endl;
		cout << "*                    *" << endl;
		cout << "* s(S): Start server *" << endl;
		cout << "*                    *" << endl;
		cout << "**********************" << endl;
		cout << "Please input: " << endl;
	}
	else {
		cout << endl;
		cout << endl;
		cout << "*********************" << endl;
		cout << "*                   *" << endl;
		cout << "* e(E): Exit server *" << endl;
		cout << "*                   *" << endl;
		cout << "*********************" << endl;
		cout << "Please input: " << endl;
	}
}

void ExitServer(void){
	DeleteCritalSection(&csClientList);
	CloseHandle(hServerEvent);
	closesocket(sServer);
	WSACleanup();
}

BOOL CreateHelperAndAcceptThread(void) {
	bServerRunning = TRUE;

	unsigned long ulThreadId;
	hThreadHelp = CreateThread(NULL, 0, HelperThread, NULL, 0, &ulThreadId);
	if (NULL == hThreadHelp) {
		bServerRunning = FALSE;
		return FALSE;
	}
	else 
		CloseHandle(hThreadHelp);

	hThreadAccept = CreateThread(NULL, 0, AcceptThread, NULL, 0, &ulThreadId);
	if (NULL == hThreadAccept) {
		bServerRunning = FALSE;
		return FALSE;
	}
	else 
		CloseHandle(hThreadAccept);
	return TRUE;
}

void ShowServerStartMsg(BOOL bSuc) {
	if (bSuc) {
		cout << "*********************" << endl;
		cout << "*                   *" << endl;
		cout << "* Server succeeded! *" << endl;
		cout << "*                   *" << endl;
		cout << "*********************" << endl;
	}
	else {
		cout << "*********************" << endl;
		cout << "*                   *" << endl;
		cout << "*   Server Failed!  *" << endl;
		cout << "*                   *" << endl;
		cout << "*********************" << endl;
	}
}

void ShowServerExitMsg(void) {
	cout << "*********************" << endl;
	cout << "*                   *" << endl;
	cout << "*   Server Exit...  *" << endl;
	cout << "*                   *" << endl;
	cout << "*********************" << endl;
}

DWORD __stdcall AcceptThread(void* pParam) {
	SOCKET sAccept;
	sockaddr_in addrClient;

	for (;bServerRunning;) {
		memset(&addrClient, 0, sizeof(sockaddr_in));
		int lenClient = sizeof(sockaddr_in);
		sAccept = accept(sServer, (sockaddr*)&addrClient, &lenClient);

		if (INVALID_SOCKET == sAccept) {
			int nErrCode = WSAGetLastError();
			if (nErrCode == WSAEWOULDBLOCK) {
				Sleep(TIMEFOR_THREAD_SLEEP);
				continue;
			}
			else 
				return 0;
		}
		else {
			CClient *pClient = new CClient(sAccept, addrClient);
			EnterCriticalSection(&csClientList);
			clientlist.push_back(pClient);
			LeaveCriticalSection(&csClientList);

			pClient -> StartRunning();
		}
	}
	return 0;
}

DWORD __stdcall HelperThread(void* pParam) {
	for (;bServerRunning;) {
		EnterCriticalSection(&csClientList);

		CLIENTLIST::iterator iter = clientlist.begin();
		for (iter ; iter != clientlist.end() ; ) {
			CClient *pClient = (CClient*)*iter;
			if (pClient -> IsExit()) {
				clientlist.erase(iter++);
				delete pClient;
				pClient = NULL;
			}
			else
				iter++;
		}
		LeaveCriticalSection(&csClientList);
		Sleep(TIMEFOR_THREAD_HELP);
	}

	if (!bServerRunning) {
		EnterCriticalSection(&csClientList);
		CLIENTLIST::iterator iter = clientlist.begin();
		for (iter ; iter != clientlist.end() ; ) {
			CClient *pClient = (CClient*)*iter;
			if (pClient -> IsConning())
				pClient -> DisConning();
			++iter;
		}
		LeaveCriticalSection(&csClientList);
		Sleep(TIMEFOR_THREAD_SLEEP);
		EnterCriticalSection(&csClientList);

		while (0 != clientlist.size()) {
			iter = client.begin();
			for (iter ; iter != clientlist.end() ; ) {
				CClient *pClient = (CClient*)*iter;
				if (pClient -> IsExit()) {
					clientlist.erase(iter++);
					delete pClient;
					pClient = NULL;
				}
				else 
					iter++;
			}
			Sleep(TIMEFOR_THREAD_SLEEP);
		}
		LeaveCriticalSection(&csClientList);
	}
	clientlist.clear();
	SetEvent(hServerEvent);
	return 0;
}



