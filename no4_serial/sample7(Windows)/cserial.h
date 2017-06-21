///////////////////////////////////////////////////////////////////////////
// �V���A���ʐM�x���N���X Ver 0.50 
// cserial.h ����
// 2011.5.18 Wed.
// �c�� ����
// 0.1 �����J (2004.5.28 Fri.)
// 0.11 ~CSerial���\�b�h�ɊԈႢ���������̂ŏC�� (2004.5.31 Mon.)
// 0.12 ReceiveSerialData��MakeBuffer���\�b�h�ɊԈႢ���������̂ŏC��
// 0.20 FIFO�������̗ʂ����邱�Ƃ��\�ɂȂ�B(2006.6.9 Fri.)
// 0.3 COM10�ȍ~���T�|�[�g(2008.5.15 Thr.)
// 0.4 ���b�Z�[�W�{�b�N�X��\�����Ȃ������\�ɂ���(�w�b�_�t�@�C���Őݒ�\)�B(209.5.18 Mon.)
// 0.5 UNICODE �Ή�
///////////////////////////////////////////////////////////////////////////
#ifndef __CSERIAL_H__
#define __CSERIAL_H__

//������L���ɂ���ƃ��b�Z�[�W�{�b�N�X���\������Ȃ��Ȃ�܂��B
#define __NO_SRIAL_MESSAGE_BOX__

//windows.h��include����ĂȂ����windows.h��include����B
#ifndef _WINDOWS_
#include < windows.h >
#endif

//stdio.h��include����ĂȂ����stdio.h��include����B
#ifndef _INC_STDIO
#include < stdio.h >
#endif

#include < tchar.h >

class CSerial
{
	// �{�N���X�𑕔�����e�n���h�� �����l0
	HWND m_hwndParent;
	// �{�N���X�Ŏg�p���Ă���n���h�� �����l0
	HANDLE m_comHandle;
	// ���M�p�o�b�t�@�T�C�Y(byte�P��) �����l1024
	unsigned int m_sendbuffersize;
	// ��M�p�o�b�t�@�T�C�Y(byte�P��) �����l1024
	unsigned int m_receivebuffersize;
	// �ʐM�{�[���C�g�p�ϐ�(bps) �����l2400
	unsigned int m_baudrate;
	// com�|�[�g�I��p������ �����l"COM1"
	TCHAR m_com[ 20 ];
	// �V���A���ʐM�p�N���X�̏�Ԃ�\���ϐ� �����l0
	unsigned char m_condition;
	// �V���A������M�̃^�C���A�E�g�܂ł̎���(msec) �����l50
	unsigned int m_timeout;

public:
	// �V���A�����M�p�f�[�^(�|�C���^) �����l0
	unsigned char * m_senddata;
	// �V���A����M�p�f�[�^(�|�C���^) �����l0
	unsigned char * m_receivedata;
	// �V���A�����M�p�f�[�^�̐� �����l1
	unsigned int m_senddatasize;
	// �V���A����M�p�f�[�^ �����l1
	unsigned int m_receivedatasize;

	// ������
	CSerial( void );
	// �I������
	virtual ~CSerial( void );

	// �V���A���|�[�g�̖��O�̐ݒ�
	// �߂�l ������ 1 �A���s�� 0
	// ��1���� �ݒ肷��COM�|�[�g�̕����� �� "COM1" �� "COM2" �Ȃ�
	BOOL CSerial::SetSerialPortName( TCHAR * );
	// �e�E�B���h�E�̐ݒ�
	// �߂�l �Ȃ�
	// ��1���� �e�E�B���h�E�̃n���h��
	void CSerial::SetHwnd( HWND );
	// COM�|�[�g�̃^�C���A�E�g�ݒ�
	// �߂�l ������ 1 �A���s�� 0
	// ��1���� unsigned int�^ �^�C���A�E�g�܂ł̎���(msec)
	BOOL CSerial::SetSerialTimeOut( unsigned int );
	// COM�|�[�g�̐ݒ�
	// �߂�l ������ 1 �A���s�� 0
	// ��1���� unsigned int�^ �V���A���ʐM�̃{�[���C�g(bps)
	// ��2���� unsigned int�^ ���M�pFIFO�������̃T�C�Y(byte)
	// ��3���� unsigned int�^ ��M�pFIFO�������̃T�C�Y(byte)
	BOOL CSerial::SetSerialPort( unsigned int , unsigned int , unsigned int );
	// �V���A���|�[�g���I�[�v������B
	// �߂�l ������ 1 �A���s�� 0
	// ��1���� �Ȃ�
	BOOL CSerial::OpenSerialPort( void );
	// �V���A���|�[�g���N���[�Y����B
	// �߂�l �Ȃ�
	// ���� �Ȃ�
	void CSerial::CloseSerialPort( void );
	// �V���A���|�[�g�̃f�[�^�̑��M(��������)
	// �߂�l �V���A���|�[�g���J���ĂȂ��Ƃ�-1 ,���M����f�[�^�̃o�b�t�@������Ȃ����-2,����ȊO�͎��ۂɑ��M�����o�C�g��
	// ������ unsigned int�^ �������ރf�[�^�̃o�C�g��
	int CSerial::SendSerialData( unsigned int );
	// �V���A���|�[�g�̃f�[�^�̎�M(�ǂݍ���)
	// �߂�l �V���A���|�[�g���J���ĂȂ��Ƃ�-1 ,���M����f�[�^�̃o�b�t�@������Ȃ����-2,����ȊO�͎��ۂɓǂݍ��񂾃o�C�g��
	// ������ unsigned int�^ �ǂݍ��ރf�[�^�̃o�C�g��
	int CSerial::ReceiveSerialData( unsigned int );
	// �V���A���|�[�g�̃o�b�t�@�̃N���A
	// �߂�l �V���A���|�[�g���J���ĂȂ��Ƃ�-1 ���s�Ȃ�0 ����������0�ȊO
	// ���� �Ȃ�
	int CSerial::SerialPortBufferClear( void );
	// ����M���邽�߂̃o�b�t�@���쐬����B
	// �߂�l ������1�A���s��0
	// ��1���� ���M���邽�߂̃o�b�t�@�T�C�Y
	// ��2���� ��M���邽�߂̃o�b�t�@�T�C�Y
	BOOL CSerial::MakeBuffer( unsigned int , unsigned int );
	// ��M�pFIFO�������̗ʂ��擾�B
	// �߂�l �����݂�FIFO��M�������̗�(byte���P��)
	// ���� �Ȃ�
	int CSerial::GetInputFIFO( void );
	// ���M�pFIFO�������̗ʂ��擾�B
	// �߂�l �����݂�FIFO���M�������̗�(byte���P��)
	// ���� �Ȃ�
	int CSerial::GetOutputFIFO( void );
};

#endif