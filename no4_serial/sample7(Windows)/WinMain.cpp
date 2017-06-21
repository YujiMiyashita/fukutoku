
#include < stdio.h >
#include "cserial.h" //シリアル通信用クラスのヘッダファイル

void main( void )
{
	int data = 0;
	CSerial *cserial; 								// CSerialクラスのポインタの宣言
	cserial = new CSerial; 							// Cserialクラスを取得
	cserial->MakeBuffer( 1 , 1 ); 					// 送信用データを1byte、受信用データを1byte用意する。
	cserial->SetSerialPortName("COM3");			// COMの設定
	cserial->OpenSerialPort( ); 					// シリアルポートをオープンする。
	cserial->SetSerialPort( 1200 , 1024 , 1024 );	// ボーレイトの設定
	cserial->SerialPortBufferClear( ); 				// シリアルポートの送受信FIFOメモリをクリアする。
	while( data >= 0 )
	{
		printf( "データを入れて下さい。\n" );
		scanf( "%d" , &data );
		cserial->m_senddata[ 0 ] = ( unsigned char ) data; 	// 送信用データを代入
		cserial->SendSerialData( 1 ); 						// パソコンからdsPICに1byteのデータを送信
		cserial->ReceiveSerialData( 1 ); 					// パソコンに来ているシリアルデータを1byte受信
		printf( "データは %d です。" , cserial->m_receivedata[ 0 ] ); // 受信したデータを出力
	}
	cserial->CloseSerialPort( ); 					// シリアルポートをクローズする。
	delete cserial; 								// CSerialクラスを開放
}
