#include <p33FJ128MC802.h>

_FOSCSEL(FNOSC_FRCPLL)//内部オシレータ使用
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_NONE ) //クロック切り替えモードOFF、OSCOは通常のI/Oピン、主発振器無効(内部オシレータ使用のため)
_FWDT(FWDTEN_OFF) //ウォッチドッグタイマーOFF
_FGS(GCP_OFF) //コードプロテクトOFF

#define FCY 30401250UL
void Delay_ms(unsigned int ms)  { while(ms--) __delay32(FCY / 1000); }

// 非同期シリアル送信用の関数(dataは送信するデータ)
void put_serial_data( int data )
{
  //データを送信するまで永久に待つ。
  do{                 // 無限ループ
    if( !U1STAbits.UTXBF ){     // 送信用バッファが空いているかを調べる
      U1TXREG = data;       // 送信用バッファにデータを代入して送信開始
      return;           // 送信処理終了
    }
  }while( 1 );            // 送信可能でなければ送信可能になるまで待つ。
}

// 非同期シリアル受信用の関数(戻り値がデータ)
int get_serial_data( void )
{
  int data; // 受信データを宣言

  // データがくるまで永久に待つ。
  do{                 // 無限ループ
    while( U1STAbits.URXDA ){     // 新しい受信データが来ているか判断する
      if( U1STAbits.FERR ){     // フレーミングエラーのチェック
        data = U1RXREG;     // データをロード
        U1STAbits.FERR = 0;   // フレーミングエラーのクリア
        if( U1STAbits.OERR ){   // オーバーランエラーのチェック
          U1STAbits.OERR = 0; // オーバーランエラーをクリア            }
          break;        // 無限ループに戻る
        }
      }
      if( U1STAbits.OERR ){     // オーバーランエラーのチェック
        U1STAbits.OERR = 0;   // オーバーランエラーをクリア
        break;          // 無限ループに戻る
      }
      data = U1RXREG;       // エラーが無ければ受信データを読む。
      return data;        // 受信したデータを返して受信処理終了。
    }
  }while( 1 );            // ここまで無限ループ
}

//main文
void main( void ){
  // クロックの設定 30.40125MIPS
  CLKDIVbits.PLLPRE = 0;        // クロック設定N2=2
  CLKDIVbits.PLLPOST = 0;       // クロック設定N1=2
  PLLFBD = 31;            // クロック設定M=33
  RCONbits.SWDTEN = 0;        // ウォッチドッグタイマーオフ

  // 入出力設定
  TRISA = 0x001f;           // Aポートの設定
  TRISB = 0x0203;           // Bポートの設定
  AD1PCFGL = 0xfffc;          // アナログ・ディジタル入力の設定

  // UART1の設定
  U1MODE = 0x8808;          // UART通信モードの設定
  U1STA  = 0x0400;          // UARTの状態と制御の設定
  U1BRG  = 6333;            // ボーレイトを2400→1200bpsに変更

  // 入出力ポートの設定

  //ピンをシリアル通信の受信として設定
  //受診の場合、U1RXR = (ピンの番号) とする
  _U1RXR = 9;   

  //RPINR19bits.U2RXR = 9;
  //ピンをシリアル通信の送信として設定
  //ピンの番号 = (機能)
  //3はU1に送信、5にするとU2に送信
  _RP8R  = 3;


  int data;

  // GGR
  Delay_ms( 10 );
  while( U1STAbits.URXDA )get_serial_data();

  while(1){             // 無限ループ
    data = get_serial_data();   // データを受信する。
    LATB = data << 12;        // 受信したデータをPORTBに出力する。
    put_serial_data( data );    // 受信したデータをそのまま送信する。
  }                 // ここまで無限ループ
}