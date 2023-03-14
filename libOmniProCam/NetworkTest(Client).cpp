/*********************************************************
	ネットワークテスト(クライアント側)
	2014/06/19 by 佐藤俊樹@UECISMS
	注意: ソースコードの外部への配布は厳禁とする。
 *********************************************************/

#include <stdlib.h>
#include <stdlib.h>
#include "./Network/SocketUDP.h"

UDPClient *udpClient = NULL;

typedef struct{
	int value;
}NetworkData;

NetworkData sendBuffer;

/* 初期化 */
int initialize(){

	/* ネットワーク初期化 */
	SocketUDP::initializeSocket();
	udpClient = SocketUDP::createClient("localhost", 31416);

	sendBuffer.value = 123;

	return 0;
} // initialize

/* 解放 */
void finalize(){

	delete udpClient;
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

	/* 送信 */
	while (1){
		int ret = udpClient->send(&sendBuffer, 4);
		Sleep(1000);
		printf("%d\n", ret);
	}
	
	/* 解放 */
	finalize();

	return 0;
}
