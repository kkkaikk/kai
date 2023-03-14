/*********************************************************
	�l�b�g���[�N�e�X�g(�N���C�A���g��)
	2014/06/19 by �����r��@UECISMS
	����: �\�[�X�R�[�h�̊O���ւ̔z�z�͌��ւƂ���B
 *********************************************************/

#include <stdlib.h>
#include <stdlib.h>
#include "./Network/SocketUDP.h"

UDPClient *udpClient = NULL;

typedef struct{
	int value;
}NetworkData;

NetworkData sendBuffer;

/* ������ */
int initialize(){

	/* �l�b�g���[�N������ */
	SocketUDP::initializeSocket();
	udpClient = SocketUDP::createClient("localhost", 31416);

	sendBuffer.value = 123;

	return 0;
} // initialize

/* ��� */
void finalize(){

	delete udpClient;
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

	/* ���M */
	while (1){
		int ret = udpClient->send(&sendBuffer, 4);
		Sleep(1000);
		printf("%d\n", ret);
	}
	
	/* ��� */
	finalize();

	return 0;
}
