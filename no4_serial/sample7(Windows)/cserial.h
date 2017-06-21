///////////////////////////////////////////////////////////////////////////
// シリアル通信支援クラス Ver 0.50 
// cserial.h 部分
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
#ifndef __CSERIAL_H__
#define __CSERIAL_H__

//ここを有効にするとメッセージボックスが表示されなくなります。
#define __NO_SRIAL_MESSAGE_BOX__

//windows.hがincludeされてなければwindows.hをincludeする。
#ifndef _WINDOWS_
#include < windows.h >
#endif

//stdio.hがincludeされてなければstdio.hをincludeする。
#ifndef _INC_STDIO
#include < stdio.h >
#endif

#include < tchar.h >

class CSerial
{
	// 本クラスを装備する親ハンドル 初期値0
	HWND m_hwndParent;
	// 本クラスで使用しているハンドル 初期値0
	HANDLE m_comHandle;
	// 送信用バッファサイズ(byte単位) 初期値1024
	unsigned int m_sendbuffersize;
	// 受信用バッファサイズ(byte単位) 初期値1024
	unsigned int m_receivebuffersize;
	// 通信ボーレイト用変数(bps) 初期値2400
	unsigned int m_baudrate;
	// comポート選択用文字列 初期値"COM1"
	TCHAR m_com[ 20 ];
	// シリアル通信用クラスの状態を表す変数 初期値0
	unsigned char m_condition;
	// シリアル送受信のタイムアウトまでの時間(msec) 初期値50
	unsigned int m_timeout;

public:
	// シリアル送信用データ(ポインタ) 初期値0
	unsigned char * m_senddata;
	// シリアル受信用データ(ポインタ) 初期値0
	unsigned char * m_receivedata;
	// シリアル送信用データの数 初期値1
	unsigned int m_senddatasize;
	// シリアル受信用データ 初期値1
	unsigned int m_receivedatasize;

	// 初期化
	CSerial( void );
	// 終了処理
	virtual ~CSerial( void );

	// シリアルポートの名前の設定
	// 戻り値 成功時 1 、失敗時 0
	// 第1引数 設定するCOMポートの文字列 例 "COM1" や "COM2" など
	BOOL CSerial::SetSerialPortName( TCHAR * );
	// 親ウィンドウの設定
	// 戻り値 なし
	// 第1引数 親ウィンドウのハンドル
	void CSerial::SetHwnd( HWND );
	// COMポートのタイムアウト設定
	// 戻り値 成功時 1 、失敗時 0
	// 第1引数 unsigned int型 タイムアウトまでの時間(msec)
	BOOL CSerial::SetSerialTimeOut( unsigned int );
	// COMポートの設定
	// 戻り値 成功時 1 、失敗時 0
	// 第1引数 unsigned int型 シリアル通信のボーレイト(bps)
	// 第2引数 unsigned int型 送信用FIFOメモリのサイズ(byte)
	// 第3引数 unsigned int型 受信用FIFOメモリのサイズ(byte)
	BOOL CSerial::SetSerialPort( unsigned int , unsigned int , unsigned int );
	// シリアルポートをオープンする。
	// 戻り値 成功時 1 、失敗時 0
	// 第1引数 なし
	BOOL CSerial::OpenSerialPort( void );
	// シリアルポートをクローズする。
	// 戻り値 なし
	// 引数 なし
	void CSerial::CloseSerialPort( void );
	// シリアルポートのデータの送信(書き込み)
	// 戻り値 シリアルポートが開いてないとき-1 ,送信するデータのバッファが足りなければ-2,それ以外は実際に送信したバイト数
	// 第一引数 unsigned int型 書き込むデータのバイト数
	int CSerial::SendSerialData( unsigned int );
	// シリアルポートのデータの受信(読み込み)
	// 戻り値 シリアルポートが開いてないとき-1 ,送信するデータのバッファが足りなければ-2,それ以外は実際に読み込んだバイト数
	// 第一引数 unsigned int型 読み込むデータのバイト数
	int CSerial::ReceiveSerialData( unsigned int );
	// シリアルポートのバッファのクリア
	// 戻り値 シリアルポートが開いてないとき-1 失敗なら0 成功したら0以外
	// 引数 なし
	int CSerial::SerialPortBufferClear( void );
	// 送受信するためのバッファを作成する。
	// 戻り値 成功時1、失敗時0
	// 第1引数 送信するためのバッファサイズ
	// 第2引数 受信するためのバッファサイズ
	BOOL CSerial::MakeBuffer( unsigned int , unsigned int );
	// 受信用FIFOメモリの量を取得。
	// 戻り値 今現在のFIFO受信メモリの量(byte数単位)
	// 引数 なし
	int CSerial::GetInputFIFO( void );
	// 送信用FIFOメモリの量を取得。
	// 戻り値 今現在のFIFO送信メモリの量(byte数単位)
	// 引数 なし
	int CSerial::GetOutputFIFO( void );
};

#endif