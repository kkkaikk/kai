/*********************************************************
	�l�b�g���[�N�e�X�g(�T�[�o��)
	2014/06/19 by �����r��@UECISMS
	����: �\�[�X�R�[�h�̊O���ւ̔z�z�͌��ւƂ���B
 *********************************************************/

#include <stdlib.h>
#include <stdlib.h>
#include "./Network/SocketUDP.h"

UDPServer *udpServer = NULL;
static const int RECEIVE_BUFFER_SIZE = (15 + 1);
char receiveBuffer[RECEIVE_BUFFER_SIZE];

/* ������ */
int initialize(){

	/* �l�b�g���[�N������ */
	SocketUDP::initializeSocket();
	udpServer = SocketUDP::createServer(31416);

	receiveBuffer[RECEIVE_BUFFER_SIZE - 1] = '\0';

	return 0;
} // initialize

/* ��� */
void finalize(){

	delete udpServer;
	SocketUDP::finalizeSocket();

} // finalize

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
int main(){

	/* ������ */
	if ( initialize() < 0 ){
		printf("�������Ɏ��s���܂����B\n");
		return -1;
	}

	/* ��M */
	int counter = 0;
	while (1){
		int ret = udpServer->receive((void*)receiveBuffer, RECEIVE_BUFFER_SIZE);
		if ( ret < 1 ){
			printf("Error: %d\n", WSAGetLastError());
		}else{
			printf("Received: %d\n", counter++);
		}
	}
	
	/* ��� */
	finalize();

	return 0;
}
