// ClientFTP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ClientFTP.h"
#include "afxsock.h"
#include<iostream>
#include<string> 
#include<list>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SIZE 2048
// The one and only application object

CWinApp theApp;

using namespace std;


string tok1(const string& a)
{
	string results;
	int i = 0;
	while (a[i] != ' '&& i!=a.length())
	{
		results.push_back(a[i++]);
	}
	return results;
}


string tok2(const string& a)
{
	string results;
	int i = 0;
	while (a[i] != ' ') i++;
	while (a[i]) results.push_back(a[i++]);
	return results;
}


string ItoStr(const int& src)
{
	list<char> dest;
	int temp = src;
	while (temp > 0)
	{
		dest.push_front(temp % 10 + '0');
		temp /= 10;
	}
	string result(dest.begin(), dest.end());
	return result;
}

string Ip(const string& src)
{
	string result = src;
	for (int i = 0; i < result.length(); i++)
	{
		if (result[i] == '.') result[i] = ',';
	}
	return result;
}

int main()
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			wprintf(L"Fatal Error: MFC initialization failed\n");
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			AfxSocketInit(NULL);
			CSocket client;
			string req;// request message
			char rsp[SIZE];// respond message
			bool active = TRUE;
			//char *c;
			if (!client.Create())
			{
				cout << "ERROR, DON'T CREATE SOCKET" << endl;
				return 1;
			}
			char c[100];
			if (!client.Connect(_T("127.0.0.1"), 21))
			{
				cout << "ERROR, DON'T CONCECT TO SERVER" << endl;
				return 1;
			}
			memset(rsp, 0, SIZE);
			client.Receive(rsp, SIZE);
			if (strstr(rsp, "220") == NULL) {
				cout << "Server don't accept connect" << endl;
				return 1;
			}
			cout << rsp;
			cout << "connect success, please enter User and Password" << endl;
			//Login
			cout << "User: ";
			getline(cin, req);
			req = "USER " + req + "\n";
			client.Send(req.c_str(), req.length());
			memset(rsp, 0, SIZE);
			client.Receive(rsp, SIZE);
			cout << rsp;
			cout << "Password: ";
			getline(cin, req);
			req = "PASS " + req + "\n";
			client.Send(req.c_str(), req.length());
			memset(rsp, 0, SIZE);
			client.Receive(rsp, SIZE);
			if (strstr(rsp, "230") == NULL) {
				//cout << "Login failed" << endl;
				cout << rsp;
				return 1;
			}
			//	cout << "230 Login successfully" << endl;
			cout << rsp;
			//End login
			while (1)
			{

				getline(cin, req);// Nhận lệnh
				//15)	Thoát khỏi Server (quit, exit)
				if (req == "quit")
				{
					req = "QUIT\n";
					client.Send(req.c_str(), req.length());
					memset(rsp, 0, SIZE);
					client.Receive(rsp, SIZE);
					cout << rsp;
					break;
				}
				if (req == "dir" || req == "ls" || req == "get" || req == "put" || req == "mget" || req == "mput")
				{
					string temp = req;
					if (active)
					{
						CSocket data;
						if (!data.Create()) {
							cout << "Don't create soket in active mode" << endl;
							break;
						}
						data.Listen();
						UINT port;
						CString host;
						data.GetSockName(host, port);
						string div = ItoStr(int(port) / 256);
						string mod = ItoStr(port % 256);
						string ip = Ip("127.0.0.1");
						req = "PORT " + ip + "," + div + "," + mod + "\n";
						client.Send(req.c_str(), req.length());
						memset(rsp, 0, SIZE);
						client.Receive(rsp, SIZE);
						cout << rsp;
						if (temp == "dir") 	req = "LIST\n";
						else if (temp == "ls") req = "NLST\n";
						client.Send(req.c_str(), req.length());
						memset(rsp, 0, SIZE);
						//client.Receive(rsp, SIZE);
						CSocket tranfer;
						data.Accept(tranfer);
						while (1)
						{
							int byteRec;
							memset(rsp, 0, SIZE);
							byteRec = tranfer.Receive(rsp, SIZE);
							if (byteRec == 0 || byteRec == SOCKET_ERROR)
							{

								break;

							}
							else cout << rsp;
						}
						
						memset(rsp, 0, SIZE);
						client.Receive(rsp, SIZE);
						cout << rsp;
					//	data.Receive(rsp, SIZE);
						tranfer.Close();
						data.Close();




					}
				}
				//Thay đổi đường dẫn trên Server 
				if (tok1(req) == "cd")
				{
					if (req == "cd") {
						string file;
						cout << "Remote directory ";
						getline(cin, file);
						req = "CWD " + file + "\n";
					}
					else {
						req = "CWD" + tok2(req) + "\n";
					}

					client.Send(req.c_str(), req.length());
					memset(rsp, 0, SIZE);
					client.Receive(rsp, SIZE);
					cout << rsp;
					continue;
				}
				//Thay đổi đường dẫn dưới client (lcd)
				if (tok1(req) == "cd")
				{
					if (req == "cd") {
						string file;
						cout << "Remote directory ";
						getline(cin, file);
						req = "CWD " + file + "\n";
					}
					else {
						req = "CWD" + tok2(req) + "\n";
					}

					client.Send(req.c_str(), req.length());
					memset(rsp, 0, SIZE);
					client.Receive(rsp, SIZE);
					cout << rsp;
					continue;
				}
			}






			client.Close();

		}
	}
	else
	{
		// TODO: change error code to suit your needs
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}

	return nRetCode;
}
