
#include < stdio.h >
#include "cserial.h" //�V���A���ʐM�p�N���X�̃w�b�_�t�@�C��

void main( void )
{
	int data = 0;
	CSerial *cserial; 								// CSerial�N���X�̃|�C���^�̐錾
	cserial = new CSerial; 							// Cserial�N���X���擾
	cserial->MakeBuffer( 1 , 1 ); 					// ���M�p�f�[�^��1byte�A��M�p�f�[�^��1byte�p�ӂ���B
	cserial->SetSerialPortName("COM3");			// COM�̐ݒ�
	cserial->OpenSerialPort( ); 					// �V���A���|�[�g���I�[�v������B
	cserial->SetSerialPort( 1200 , 1024 , 1024 );	// �{�[���C�g�̐ݒ�
	cserial->SerialPortBufferClear( ); 				// �V���A���|�[�g�̑���MFIFO���������N���A����B
	while( data >= 0 )
	{
		printf( "�f�[�^�����ĉ������B\n" );
		scanf( "%d" , &data );
		cserial->m_senddata[ 0 ] = ( unsigned char ) data; 	// ���M�p�f�[�^����
		cserial->SendSerialData( 1 ); 						// �p�\�R������dsPIC��1byte�̃f�[�^�𑗐M
		cserial->ReceiveSerialData( 1 ); 					// �p�\�R���ɗ��Ă���V���A���f�[�^��1byte��M
		printf( "�f�[�^�� %d �ł��B" , cserial->m_receivedata[ 0 ] ); // ��M�����f�[�^���o��
	}
	cserial->CloseSerialPort( ); 					// �V���A���|�[�g���N���[�Y����B
	delete cserial; 								// CSerial�N���X���J��
}
