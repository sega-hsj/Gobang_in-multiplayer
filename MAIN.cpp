#include <stdio.h> 
#include "winsock2.h"  
#include <iostream>  
#include<string>
#pragma comment(lib, "ws2_32.lib")  

using namespace std;

void printtitle();
void printmenu();


class gomap{
public:
	int map[15][15];//0未落子，1白子，2黑子
	int gamecondition = 0;//0未胜负，1白方胜，2黑方胜

	gomap(int i){
		for (int i = 0; i < 15; i++){
			for (int j = 0; j < 15; j++){
				map[i][j] = 0;
			}
		}
	}

	int judge(int x, int y, int gotype, int xder, int yder){
		for (int i = 0; i < 5; i++){
			if (map[x + (xder)*i][y + (yder)*i] != gotype){
				return 0;
			}
			if (i == 4 && map[x + (xder)*i][y + (yder)*i] == gotype){
				return 1;
			}
		}
	}

	void judgegame(int gotype){
		for (int x = 4; x <= 10; x++){
			for (int y = 4; y <= 10; y++){
				if (judge(x, y, gotype, 0, -1) || judge(x, y, gotype, -1, 0) || judge(x, y, gotype, 0, 1) || judge(x, y, gotype, 1, 0) || judge(x, y, gotype, -1, -1) || judge(x, y, gotype, -1, 1) || judge(x, y, gotype, 1, 1) || judge(x, y, gotype, 1, -1)){
					gamecondition = gotype;
					return;
				}
			}
		}
	}

	int putgo(char goplace[], int gotype){
		int a;
		int b;
		a = (goplace[0] - '0') * 10 + (goplace[1] - '0');
		b = (goplace[3] - '0') * 10 + (goplace[4] - '0');
		if (map[a][b] != 0||a>14||a<0||b>14||b<0){ return 0; }
		map[a][b] = gotype;

		judgegame(gotype);
	}

	void print(){
		system("cls");
		printtitle();
		for (int i = 0; i < 15; i++){
			if (i < 10){ cout << i << " "; }
			else{ cout << i; }
		}
		cout << endl;
		for (int x = 0; x < 15; x++){
			for (int y = 0; y < 15; y++){
				if (map[x][y] == 0){ cout << "■"; }
				else if (map[x][y] == 1){ cout << "★"; }
				else if (map[x][y] == 2){ cout << "☆"; }
			}
			cout << x << endl;
		}

	}
};


int main(){
	//---------------------------------------------------------------
	const int BUF_SIZE = 6;
	WSADATA         wsd;            //WSADATA变量  
	SOCKET          sServer;        //服务器套接字  
	SOCKET          sClient;        //客户端套接字  
	SOCKADDR_IN     addrServ;      //服务器地址  
	char            buf[BUF_SIZE];  //接收数据缓冲区  
	char            sendBuf[BUF_SIZE];//返回给客户端得数据  
	int             retVal;         //返回值  

	//初始化套结字动态库  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return 1;
	}
	//创建套接字  
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sServer)
	{
		cout << "socket failed!" << endl;
		WSACleanup();//释放套接字资源;  
		return  -1;
	}

	//----------------------------------------------------------------

	printtitle();
	printmenu();
	gomap Go(0);
	int choosenum;
	cin >> choosenum;
	while (choosenum != 1 && choosenum != 2){
		cout << "please input the right num:";
		cin >> choosenum;
	}
	cin.get();

	//————————————————————————————————————————————————————————	

	if (choosenum == 1){ //作为服务器
		//服务器套接字地址   
		addrServ.sin_family = AF_INET;
		addrServ.sin_port = htons(4999);
		addrServ.sin_addr.s_addr = INADDR_ANY;

		//将套接字与主机地址绑定  
		retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
		if (SOCKET_ERROR == retVal)
		{
			cout << "bind failed!" << endl;
			closesocket(sServer);   //关闭套接字  
			WSACleanup();           //释放套接字资源;  
			return -1;
		}

		//开始监听   
		cout << "请将你的地址告诉另一位玩家，等待另一位玩家接入" << endl;
		retVal = listen(sServer, 1);
		if (SOCKET_ERROR == retVal)
		{
			cout << "listen failed!" << endl;
			closesocket(sServer);   //关闭套接字  
			WSACleanup();           //释放套接字资源;  
			return -1;
		}

		//接受客户端请求  
		sockaddr_in addrClient;//客户端地址
		int addrClientlen = sizeof(addrClient);//客户端地址长度
		sClient = accept(sServer, (sockaddr FAR*)&addrClient, &addrClientlen);
		if (INVALID_SOCKET == sClient)
		{
			cout << "accept failed!" << endl;
			closesocket(sServer);   //关闭套接字  
			WSACleanup();           //释放套接字资源;  
			return -1;
		}
		cout << "玩家2加入，游戏开始！" << endl;
		Go.print();
		cout << "对方回合";
		while (true)
		{
			//接收客户端数据  
			ZeroMemory(buf, BUF_SIZE);
			retVal = recv(sClient, buf, BUF_SIZE, 0);
			if (SOCKET_ERROR == retVal)
			{
				cout << "recv failed!" << endl;
				closesocket(sServer);   //关闭套接字  
				closesocket(sClient);   //关闭套接字       
				WSACleanup();           //释放套接字资源;  
				return -1;
			}

			Go.putgo(buf,1);

			Go.print();
			if (Go.gamecondition != 0){
				cout << "对方赢了！！！" << endl;
				system("pause");
				break;
			}

			cout << "到你了，请输入落子位置：";
			cin.getline(sendBuf, BUF_SIZE);
			while (Go.putgo(sendBuf, 2) == 0){
				cout << "这个位置已经有棋子了，请重新输入：";
				cin.getline(sendBuf, BUF_SIZE);
			}
			Go.print();

			send(sClient, sendBuf, strlen(sendBuf), 0);

			if (Go.gamecondition != 0){
				cout << "你赢了！！！" << endl;
				system("pause");
				break;
			}
			cout << "对方回合";
		}

		//退出  
		closesocket(sServer);   //关闭套接字  
		closesocket(sClient);   //关闭套接字  
		WSACleanup();           //释放套接字资源;  

		return 0;

		//————————————————————————————————————————————————

	}
	else if (choosenum == 2){ //作为客户端
		char ip[40];
		cout << "请输入对方ip地址：";
		cin >> ip;
		cin.get();
		//设置服务器地址  
		addrServ.sin_family = AF_INET;//TCP/IP协议
		addrServ.sin_addr.s_addr = inet_addr(ip);//IP地址-inet_addr函数用于把ip地址转换为网络字节顺序
		addrServ.sin_port = htons((short)4999);//端口号-htons函数用于网络字节顺序
		int nServAddlen = sizeof(addrServ);//暂时没用到

		//连接服务器  
		retVal = connect(sServer, (LPSOCKADDR)&addrServ, sizeof(addrServ));
		if (SOCKET_ERROR == retVal)
		{
			cout << "connect failed!" << endl;
			system("pause");
			closesocket(sServer); //关闭套接字  
			WSACleanup(); //释放套接字资源  
			return -1;
		}
		cout << "成功加入游戏！";
		Go.print();

		while (true)
		{
			//向服务器发送数据  
			ZeroMemory(buf, BUF_SIZE);
			cout << "到你了，请输入落子位置：";

			cin.getline(buf, BUF_SIZE);
			while (Go.putgo(buf, 1)==0){
				cout << "这个位置已经有棋子了，请重新输入：";
				cin.getline(buf, BUF_SIZE);

			}
			Go.print();


			retVal = send(sServer, buf, strlen(buf), 0);

			if (SOCKET_ERROR == retVal)
			{
				cout << "send failed!" << endl;
				closesocket(sServer); //关闭套接字  
				WSACleanup(); //释放套接字资源  
				return -1;
			}
			if (Go.gamecondition != 0){
				cout << "你赢了！！！" << endl;
				system("pause");
				break;
			}
			cout << "对方回合";


			ZeroMemory(sendBuf, BUF_SIZE);
			recv(sServer, sendBuf, BUF_SIZE, 0);

			Go.putgo(sendBuf, 2);
			Go.print();
			if (Go.gamecondition != 0){
				cout << "对方赢了！！！" << endl;
				system("pause");
				break;
			}

		}


		//退出  
		closesocket(sServer); //关闭套接字  
		WSACleanup(); //释放套接字资源  
		return 0;

	}

}