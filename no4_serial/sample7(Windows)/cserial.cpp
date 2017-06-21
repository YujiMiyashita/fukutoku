///////////////////////////////////////////////////////////////////////////
// �V���A���ʐM�x���N���X Ver 0.40 
// cserial.cpp ����
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
#include "cserial.h"

//������
CSerial::CSerial( void ):
m_hwndParent( NULL ),
m_comHandle( NULL ),
m_sendbuffersize( 1024 ),
m_receivebuffersize( 1024 ),
m_baudrate( 2400 ),
m_senddata( NULL ),
m_receivedata( NULL ),
m_senddatasize( 1 ),
m_receivedatasize( 1 ),
m_condition( 0 ),
m_timeout( 50 )
{
	_stprintf( m_com , TEXT("\\\\.\\COM1") );
	return;
}

//�I������
CSerial::~CSerial( void )
{
	if( m_senddata )
	{
		delete [ ] m_senddata;
		m_senddata = NULL;
	}
	if( m_receivedata )
	{
		delete [ ] m_receivedata;
		m_receivedata = NULL;
	}
	if( m_condition )
	{
		CloseSerialPort( );
	}
}

//COM�|�[�g�̐ݒ�
//�߂�l ������ 1 �A���s�� 0
//��1���� �ݒ肷��COM�|�[�g�̕����� �� "COM1" �� "COM2" �Ȃ�
BOOL CSerial::SetSerialPortName( TCHAR *comname )
{
	if( _stprintf( m_com , TEXT("\\\\.\\%s") , comname ) )
		return 1;
	return 0;
}

//�e�E�B���h�E�̐ݒ�
//�߂�l �Ȃ�
//��1���� �e�E�B���h�E�̃n���h��
void CSerial::SetHwnd( HWND hwnd )
{
	m_hwndParent = hwnd;
}

//�V���A���|�[�g�̃^�C���A�E�g�ݒ�
//�߂�l ������ 1 �A���s�� 0
//��1���� unsigned int�^ �^�C���A�E�g�܂ł̎���(msec)
BOOL CSerial::SetSerialTimeOut( unsigned int timeout )
{
	COMMTIMEOUTS timeouts;

	if( !GetCommTimeouts( m_comHandle , &timeouts ) )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		MessageBox( m_hwndParent , TEXT("�V���A���|�[�g�̃^�C���A�E�g�̐ݒ���擾�ł��܂���ł����B") , TEXT("error") , MB_OK );
		#endif
		return 0;
	}

	// �V���A���|�[�g�̃f�[�^��1byte��M����Ƃ��҂�����(�P�ʂ�ms)
	timeouts.ReadIntervalTimeout		=timeout;
	// �V���A���|�[�g�̃f�[�^��2byte�ȏ��M����Ƃ�1����������̏旦
	timeouts.ReadTotalTimeoutMultiplier	=0;
	// �V���A���|�[�g�̃f�[�^����M����Ƃ��̃g�[�^���̑҂�����(�P�ʂ�ms)
	timeouts.ReadTotalTimeoutConstant	=timeout;
	// �V���A���|�[�g�̃f�[�^��2byte�ȏ㑗�M����Ƃ�1����������̏旦
	timeouts.WriteTotalTimeoutMultiplier=0;
	// �V���A���|�[�g�̃f�[�^�𑗐M����Ƃ��̃g�[�^���̑҂�����(�P�ʂ�ms)
	timeouts.WriteTotalTimeoutConstant	=timeout;

	if( !SetCommTimeouts( m_comHandle , &timeouts ) )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		MessageBox( m_hwndParent , TEXT("�V���A���|�[�g�̃^�C���A�E�g�̐ݒ肪�ł��܂���ł����B") , TEXT("error") , MB_OK );
		#endif
		return 0;
	}

	m_timeout = timeout;

	return 1;
}

//�V���A���|�[�g�̐ݒ�
//�߂�l ������ 1 �A���s�� 0
// ��1���� unsigned int�^ �V���A���ʐM�̃{�[���C�g(bps)
// ��2���� unsigned int�^ ���M�pFIFO�������̃T�C�Y(byte)
// ��3���� unsigned int�^ ��M�pFIFO�������̃T�C�Y(byte)
BOOL CSerial::SetSerialPort( unsigned int baudrate , unsigned int send_buffer_size , unsigned int receive_buffer_size )
{
	DCB		dcb;

	// �V���A���|�[�g�̏�Ԃ��擾����B
	if( !GetCommState( m_comHandle , &dcb ) )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("�V���A���|�[�g�̏�Ԃ��擾�ł��܂���ł����B") , TEXT("error") , MB_OK );
		#endif
		return 0;
	}

	// �{�[���C�g�̐ݒ�(�P�ʂ�bps{bit per second})
	dcb.BaudRate		=baudrate;
	// �f�[�^�̃r�b�g��(�P�ʂ�bit)
	dcb.ByteSize		=8;
	// �p���e�B�̐ݒ�
	dcb.Parity			=NOPARITY;
	// �X�g�b�v�r�b�g�̐�
	dcb.StopBits		=ONESTOPBIT;
	// ��̓t���[����֌W
	dcb.fOutX			=false;
	dcb.fInX			=false;
	dcb.fTXContinueOnXoff =false;

	// �V���A���|�[�g�̏�Ԃ�ݒ肷��B
	if( !SetCommState( m_comHandle , &dcb ) )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("�V���A���|�[�g�̏�Ԃ�ݒ�ł��܂���ł����B") , TEXT("error") , MB_OK );
		#endif
		return 0;
	}

	m_baudrate = baudrate;

	// �V���A���|�[�g�̃o�b�t�@��ݒ肷��B
	if( !SetupComm( m_comHandle , receive_buffer_size , send_buffer_size ) )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("�V���A���|�[�g�̃o�b�t�@�̐ݒ�����s���܂����B") , TEXT("error") , MB_OK );
		#endif
		return 0;
	}

	m_sendbuffersize = send_buffer_size;
	m_receivebuffersize = receive_buffer_size;

	// �t���[������s��Ȃ��悤�ɐݒ�
	EscapeCommFunction( m_comHandle , SETDTR | SETRTS );

	return 1;
}

//�V���A���|�[�g���I�[�v������B
//�߂�l ������ 1 �A���s�� 0
//���� �Ȃ�
BOOL CSerial::OpenSerialPort( void )
{

	// �V���A���|�[�g���J��
	m_comHandle = CreateFile(	m_com,
								GENERIC_READ | GENERIC_WRITE,
								0,
								0,
								OPEN_EXISTING,
								0,
								0);

	// �V���A���|�[�g���J���̂����s�����ꍇ
	if( m_comHandle == INVALID_HANDLE_VALUE )
	{
		m_comHandle = NULL;
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("�V���A���|�[�g���J�����Ƃ��ł��܂���ł����B") , NULL , MB_OK );
		#endif
		return 0;
	}

	// �V���A���|�[�g�̃^�C���A�E�g�̐ݒ�
	if( !SetSerialTimeOut( m_timeout ) )
	{
		// �V���A���|�[�g�����B
		CloseSerialPort( );
		return 0;
	}

	// �V���A���|�[�g�̏�Ԃ̐ݒ�
	if( !SetSerialPort( m_baudrate , m_sendbuffersize , m_receivebuffersize ) )
	{
		// �V���A���|�[�g�����B
		CloseSerialPort( );
		return 0;
	}
	if( !m_senddata || !m_receivedata )
	{
		if( MakeBuffer( m_senddatasize , m_receivedatasize ) == 0 )
		{
			#ifndef __NO_SRIAL_MESSAGE_BOX__
			::MessageBox( m_hwndParent , TEXT("�V���A���ʐM�p����M�o�b�t�@���擾�ł��܂���ł����B") , NULL , MB_OK );
			#endif
			CloseSerialPort( );
			return 0;
		}
	}
	m_condition = 1;
	return 1;
}

//�V���A���|�[�g���N���[�Y����B
//�߂�l �Ȃ�
//���� �Ȃ�
void CSerial::CloseSerialPort( void )
{
	CloseHandle( m_comHandle );
	m_comHandle = NULL;
	m_condition = 0;
}

// �V���A���|�[�g�̃f�[�^�̑��M(��������)
// �߂�l �V���A���|�[�g���J���ĂȂ��Ƃ�-1 ,���M����f�[�^�̃o�b�t�@������Ȃ����-2,����ȊO�͎��ۂɑ��M�����o�C�g��
// ������ unsigned int�^ �������ރf�[�^�̃o�C�g��
int CSerial::SendSerialData( unsigned int data_size )
{
	// ���ۂɑ��M�����f�[�^���̐錾
	DWORD dumy;

	// �V���A���|�[�g���J���ĂȂ��ꍇ
	if( !m_comHandle )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("�V���A���|�[�g���J���Ă܂���B") , NULL , MB_OK );
		#endif
		return -1;
	}

	// ���M�f�[�^�o�b�t�@�����M������byte����菭�Ȃ��ꍇ
	if( m_senddatasize < data_size )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("���M�f�[�^�����������܂��B") , NULL , MB_OK );
		#endif
		return -2;
	}

	// �f�[�^�𑗐M����B
	WriteFile( m_comHandle , m_senddata , data_size , &dumy , NULL );

	// ���ۂɑ��M�����o�C�g����Ԃ��B
	return ( int ) dumy;
}

// �V���A���|�[�g�̃f�[�^�̎�M(�ǂݍ���)
// �߂�l �V���A���|�[�g���J���ĂȂ��Ƃ�-1 ,���M����f�[�^�̃o�b�t�@������Ȃ����-2,����ȊO�͎��ۂɓǂݍ��񂾃o�C�g��
// ������ unsigned int�^ �ǂݍ��ރf�[�^�̃o�C�g��
int CSerial::ReceiveSerialData( unsigned int data_size )
{
	// ���ۂɎ�M�����f�[�^���̐錾
	DWORD dumy;

	// �V���A���|�[�g���J���ĂȂ��ꍇ
	if( !m_comHandle )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("�V���A���|�[�g���J���Ă܂���B") , NULL , MB_OK );
		#endif
		return -1;
	}

	// ��M�f�[�^�o�b�t�@����M������byte����菭�Ȃ��ꍇ
	if( m_receivedatasize < data_size )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("��M�f�[�^�����������܂��B") , NULL , MB_OK );
		#endif
		return -2;
	}

	// �f�[�^����M����B
	ReadFile( m_comHandle , m_receivedata , data_size , &dumy , NULL );

	// ���ۂɎ�M�����o�C�g����Ԃ��B
	return ( int ) dumy;
}

// �V���A���|�[�g�̃o�b�t�@�̃N���A
// �߂�l �V���A���|�[�g���J���ĂȂ��Ƃ�-1 ���s�Ȃ�0 ����������0�ȊO
// ���� �Ȃ�
int CSerial::SerialPortBufferClear( void )
{
	// �V���A���|�[�g���J���ĂȂ��ꍇ
	if( !m_comHandle )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("�V���A���|�[�g���J���Ă܂���B") , NULL , MB_OK );
		#endif
		return -1;
	}

	// �o�b�t�@���N���A����(���M�A��M�̗����̃o�b�t�@)�B
	return PurgeComm( m_comHandle , PURGE_RXCLEAR | PURGE_TXCLEAR );
}

// ����M���邽�߂̃o�b�t�@���쐬����B
// �߂�l ������1�A���s��0
// ��1���� ���M���邽�߂̃o�b�t�@�T�C�Y
// ��2���� ��M���邽�߂̃o�b�t�@�T�C�Y
BOOL CSerial::MakeBuffer( unsigned int send_size , unsigned int receive_size )
{
	if( m_senddata )
	{
		delete [ ] m_senddata;
		m_senddata = NULL;
	}
	if( m_receivedata )
	{
		delete [ ] m_receivedata;
		m_receivedata = NULL;
	}
	m_senddata = new unsigned char[ send_size ];
	if( !m_senddata )
	{
		return 0;
	}
	m_senddatasize = send_size;
	m_receivedata = new unsigned char[ receive_size ];
	if( !m_receivedata )
	{
		return 0;
	}
	m_receivedatasize = receive_size;
	return 1;
}

// ��������Ver.0.2�p
// ��M�pFIFO�������̗ʂ��擾�B
// �߂�l �����݂�FIFO��M�������̗�(byte���P��)
// ���� �Ȃ�
int CSerial::GetInputFIFO( void )
{
	DWORD data;
	COMSTAT		comstat;
	ClearCommError(m_comHandle , &data , &comstat);
	return ( int ) comstat.cbInQue;
}

// ���M�pFIFO�������̗ʂ��擾�B
// �߂�l �����݂�FIFO���M�������̗�(byte���P��)
// ���� �Ȃ�
int CSerial::GetOutputFIFO( void )
{
	DWORD data;
	COMSTAT		comstat;
	ClearCommError(m_comHandle , &data , &comstat);
	return ( int ) comstat.cbOutQue;
}
