///////////////////////////////////////////////////////////////////////////
// シリアル通信支援クラス Ver 0.40 
// cserial.cpp 部分
// 2011.5.18 Wed.
// 田中 基大作
// 0.1 初公開 (2004.5.28 Fri.)
// 0.11 ~CSerialメソッドに間違いがあったので修正 (2004.5.31 Mon.)
// 0.12 ReceiveSerialDataとMakeBufferメソッドに間違いがあったので修正
// 0.20 FIFOメモリの量を見ることが可能になる。(2006.6.9 Fri.)
// 0.3 COM10以降もサポート(2008.5.15 Thr.)
// 0.4 メッセージボックスを表示しない事も可能にした(ヘッダファイルで設定可能)。(209.5.18 Mon.)
// 0.5 UNICODE 対応
///////////////////////////////////////////////////////////////////////////
#include "cserial.h"

//初期化
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

//終了処理
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

//COMポートの設定
//戻り値 成功時 1 、失敗時 0
//第1引数 設定するCOMポートの文字列 例 "COM1" や "COM2" など
BOOL CSerial::SetSerialPortName( TCHAR *comname )
{
	if( _stprintf( m_com , TEXT("\\\\.\\%s") , comname ) )
		return 1;
	return 0;
}

//親ウィンドウの設定
//戻り値 なし
//第1引数 親ウィンドウのハンドル
void CSerial::SetHwnd( HWND hwnd )
{
	m_hwndParent = hwnd;
}

//シリアルポートのタイムアウト設定
//戻り値 成功時 1 、失敗時 0
//第1引数 unsigned int型 タイムアウトまでの時間(msec)
BOOL CSerial::SetSerialTimeOut( unsigned int timeout )
{
	COMMTIMEOUTS timeouts;

	if( !GetCommTimeouts( m_comHandle , &timeouts ) )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		MessageBox( m_hwndParent , TEXT("シリアルポートのタイムアウトの設定を取得できませんでした。") , TEXT("error") , MB_OK );
		#endif
		return 0;
	}

	// シリアルポートのデータを1byte受信するとき待ち時間(単位はms)
	timeouts.ReadIntervalTimeout		=timeout;
	// シリアルポートのデータを2byte以上受信するとき1文字あたりの乗率
	timeouts.ReadTotalTimeoutMultiplier	=0;
	// シリアルポートのデータを受信するときのトータルの待ち時間(単位はms)
	timeouts.ReadTotalTimeoutConstant	=timeout;
	// シリアルポートのデータを2byte以上送信するとき1文字あたりの乗率
	timeouts.WriteTotalTimeoutMultiplier=0;
	// シリアルポートのデータを送信するときのトータルの待ち時間(単位はms)
	timeouts.WriteTotalTimeoutConstant	=timeout;

	if( !SetCommTimeouts( m_comHandle , &timeouts ) )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		MessageBox( m_hwndParent , TEXT("シリアルポートのタイムアウトの設定ができませんでした。") , TEXT("error") , MB_OK );
		#endif
		return 0;
	}

	m_timeout = timeout;

	return 1;
}

//シリアルポートの設定
//戻り値 成功時 1 、失敗時 0
// 第1引数 unsigned int型 シリアル通信のボーレイト(bps)
// 第2引数 unsigned int型 送信用FIFOメモリのサイズ(byte)
// 第3引数 unsigned int型 受信用FIFOメモリのサイズ(byte)
BOOL CSerial::SetSerialPort( unsigned int baudrate , unsigned int send_buffer_size , unsigned int receive_buffer_size )
{
	DCB		dcb;

	// シリアルポートの状態を取得する。
	if( !GetCommState( m_comHandle , &dcb ) )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("シリアルポートの状態を取得できませんでした。") , TEXT("error") , MB_OK );
		#endif
		return 0;
	}

	// ボーレイトの設定(単位はbps{bit per second})
	dcb.BaudRate		=baudrate;
	// データのビット数(単位はbit)
	dcb.ByteSize		=8;
	// パリティの設定
	dcb.Parity			=NOPARITY;
	// ストップビットの数
	dcb.StopBits		=ONESTOPBIT;
	// 後はフロー制御関係
	dcb.fOutX			=false;
	dcb.fInX			=false;
	dcb.fTXContinueOnXoff =false;

	// シリアルポートの状態を設定する。
	if( !SetCommState( m_comHandle , &dcb ) )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("シリアルポートの状態を設定できませんでした。") , TEXT("error") , MB_OK );
		#endif
		return 0;
	}

	m_baudrate = baudrate;

	// シリアルポートのバッファを設定する。
	if( !SetupComm( m_comHandle , receive_buffer_size , send_buffer_size ) )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("シリアルポートのバッファの設定を失敗しました。") , TEXT("error") , MB_OK );
		#endif
		return 0;
	}

	m_sendbuffersize = send_buffer_size;
	m_receivebuffersize = receive_buffer_size;

	// フロー制御を行わないように設定
	EscapeCommFunction( m_comHandle , SETDTR | SETRTS );

	return 1;
}

//シリアルポートをオープンする。
//戻り値 成功時 1 、失敗時 0
//引数 なし
BOOL CSerial::OpenSerialPort( void )
{

	// シリアルポートを開く
	m_comHandle = CreateFile(	m_com,
								GENERIC_READ | GENERIC_WRITE,
								0,
								0,
								OPEN_EXISTING,
								0,
								0);

	// シリアルポートを開くのを失敗した場合
	if( m_comHandle == INVALID_HANDLE_VALUE )
	{
		m_comHandle = NULL;
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("シリアルポートを開くことができませんでした。") , NULL , MB_OK );
		#endif
		return 0;
	}

	// シリアルポートのタイムアウトの設定
	if( !SetSerialTimeOut( m_timeout ) )
	{
		// シリアルポートを閉じる。
		CloseSerialPort( );
		return 0;
	}

	// シリアルポートの状態の設定
	if( !SetSerialPort( m_baudrate , m_sendbuffersize , m_receivebuffersize ) )
	{
		// シリアルポートを閉じる。
		CloseSerialPort( );
		return 0;
	}
	if( !m_senddata || !m_receivedata )
	{
		if( MakeBuffer( m_senddatasize , m_receivedatasize ) == 0 )
		{
			#ifndef __NO_SRIAL_MESSAGE_BOX__
			::MessageBox( m_hwndParent , TEXT("シリアル通信用送受信バッファが取得できませんでした。") , NULL , MB_OK );
			#endif
			CloseSerialPort( );
			return 0;
		}
	}
	m_condition = 1;
	return 1;
}

//シリアルポートをクローズする。
//戻り値 なし
//引数 なし
void CSerial::CloseSerialPort( void )
{
	CloseHandle( m_comHandle );
	m_comHandle = NULL;
	m_condition = 0;
}

// シリアルポートのデータの送信(書き込み)
// 戻り値 シリアルポートが開いてないとき-1 ,送信するデータのバッファが足りなければ-2,それ以外は実際に送信したバイト数
// 第一引数 unsigned int型 書き込むデータのバイト数
int CSerial::SendSerialData( unsigned int data_size )
{
	// 実際に送信したデータ数の宣言
	DWORD dumy;

	// シリアルポートが開いてない場合
	if( !m_comHandle )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("シリアルポートを開いてません。") , NULL , MB_OK );
		#endif
		return -1;
	}

	// 送信データバッファが送信したいbyte数より少ない場合
	if( m_senddatasize < data_size )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("送信データ数が多すぎます。") , NULL , MB_OK );
		#endif
		return -2;
	}

	// データを送信する。
	WriteFile( m_comHandle , m_senddata , data_size , &dumy , NULL );

	// 実際に送信したバイト数を返す。
	return ( int ) dumy;
}

// シリアルポートのデータの受信(読み込み)
// 戻り値 シリアルポートが開いてないとき-1 ,送信するデータのバッファが足りなければ-2,それ以外は実際に読み込んだバイト数
// 第一引数 unsigned int型 読み込むデータのバイト数
int CSerial::ReceiveSerialData( unsigned int data_size )
{
	// 実際に受信したデータ数の宣言
	DWORD dumy;

	// シリアルポートが開いてない場合
	if( !m_comHandle )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("シリアルポートを開いてません。") , NULL , MB_OK );
		#endif
		return -1;
	}

	// 受信データバッファが受信したいbyte数より少ない場合
	if( m_receivedatasize < data_size )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("受信データ数が多すぎます。") , NULL , MB_OK );
		#endif
		return -2;
	}

	// データを受信する。
	ReadFile( m_comHandle , m_receivedata , data_size , &dumy , NULL );

	// 実際に受信したバイト数を返す。
	return ( int ) dumy;
}

// シリアルポートのバッファのクリア
// 戻り値 シリアルポートが開いてないとき-1 失敗なら0 成功したら0以外
// 引数 なし
int CSerial::SerialPortBufferClear( void )
{
	// シリアルポートが開いてない場合
	if( !m_comHandle )
	{
		#ifndef __NO_SRIAL_MESSAGE_BOX__
		::MessageBox( m_hwndParent , TEXT("シリアルポートを開いてません。") , NULL , MB_OK );
		#endif
		return -1;
	}

	// バッファをクリアする(送信、受信の両方のバッファ)。
	return PurgeComm( m_comHandle , PURGE_RXCLEAR | PURGE_TXCLEAR );
}

// 送受信するためのバッファを作成する。
// 戻り値 成功時1、失敗時0
// 第1引数 送信するためのバッファサイズ
// 第2引数 受信するためのバッファサイズ
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

// ここからVer.0.2用
// 受信用FIFOメモリの量を取得。
// 戻り値 今現在のFIFO受信メモリの量(byte数単位)
// 引数 なし
int CSerial::GetInputFIFO( void )
{
	DWORD data;
	COMSTAT		comstat;
	ClearCommError(m_comHandle , &data , &comstat);
	return ( int ) comstat.cbInQue;
}

// 送信用FIFOメモリの量を取得。
// 戻り値 今現在のFIFO送信メモリの量(byte数単位)
// 引数 なし
int CSerial::GetOutputFIFO( void )
{
	DWORD data;
	COMSTAT		comstat;
	ClearCommError(m_comHandle , &data , &comstat);
	return ( int ) comstat.cbOutQue;
}
