/*********************************************************
	ネットワークテスト(サーバ側)
	2014/06/19 by 佐藤俊樹@UECISMS
	注意: ソースコードの外部への配布は厳禁とする。
 *********************************************************/

#include <stdlib.h>
#include <stdlib.h>
#include "./Network/SocketUDP.h"

UDPServer *udpServer = NULL;
static const int RECEIVE_BUFFER_SIZE = (15 + 1);
char receiveBuffer[RECEIVE_BUFFER_SIZE];

/* 初期化 */
int initialize(){

	/* ネットワーク初期化 */
	SocketUDP::initializeSocket();
	udpServer = SocketUDP::createServer(31416);

	receiveBuffer[RECEIVE_BUFFER_SIZE - 1] = '\0';

	return 0;
} // initialize

/* 解放 */
void finalize(){

	delete udpServer;
	SocketUDP::finalizeSocket();

} // finalize

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
int main(){

	/* 初期化 */
	if ( initialize() < 0 ){
		printf("初期化に失敗しました。\n");
		return -1;
	}

	/* 受信 */
	int counter = 0;
	while (1){
		int ret = udpServer->receive((void*)receiveBuffer, RECEIVE_BUFFER_SIZE);
		if ( ret < 1 ){
			printf("Error: %d\n", WSAGetLastError());
		}else{
			printf("Received: %d\n", counter++);
		}
	}
	
	/* 解放 */
	finalize();

	return 0;
}
