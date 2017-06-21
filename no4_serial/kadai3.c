#include <p33FJ128MC802.h>

_FOSCSEL(FNOSC_FRCPLL)
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_NONE)
_FWDT(FWDTEN_OFF)
_FGS(GCP_OFF)

#define FCY 30401250UL
void Delay_ms(unsigned int ms) {
  while(ms--)
    __delay32(FCY / 1000); 
}

// 非同期シリアル送信用の関数(dataは送信するデータ)
void put_serial_data(int data) {
  do {
    if (!U2STAbits.UTXBF) {
      U2TXREG = data;
      return;
    }
  } while (1);
}

// 非同期シリアル受信用の関数(戻り値がデータ)
int get_serial_data() {
  int data;
  do {
    while (U2STAbits.URXDA) {
      // フレーミングエラーのチェック
      if (U2STAbits.FERR){
        data = U2RXREG; 
        U2STAbits.FERR = 0;
      }
      // オーバーランエラーのチェック
      if (U2STAbits.OERR) {
        U2STAbits.OERR = 0;
        break;
      }
      data = U2RXREG;
      return data;
    }
  } while(1);
}

void main(void) {

  //クロックの設定
  CLKDIVbits.PLLPRE = 0;
  CLKDIVbits.PLLPOST = 0;
  PLLFBD = 31;
  RCONbits.SWDTEN = 0;
  TRISA = 0x001f;
  TRISB = 0x0203;
  AD1PCFGL = 0xfffc;

  //変数宣言
  int data;

  // UART2の設定
  U2MODE = 0x8808; // UART通信モードの設定
  U2STA  = 0x0400; // UARTの状態と制御の設定
  U2BRG  = 6333; // ボーレイトを1200bpsに

  // 入出力ポートの設定
  //ピンをシリアル通信の受信として設定
  //受診の場合、U2RXR = (ピンの番号) とする
  _U2RXR = 9;

  //RPINR19bits.U2RXR = 9;
  //ピンをシリアル通信の送信として設定
  //ピンの番号 = (機能)
  //3はU2に送信、5にするとU2に送信
  _RP8R = 3;
  
  Delay_ms(10);

  //PCから値が入力されるまで無限ループ
  //これをしないとおかしなことになる  
  while (U2STAbits.URXDA) {
    get_serial_data();
  }

  //get_serial_dataがPCから送信されるデータ
  //dataという変数に代入
  //put_serial_dataはマイコンからPCへ送られるデータ
  //1と入力→マイコンに値が送信される→LED点灯→PCのコンソールに表示

  while (1) {
    data = get_serial_data();
    LATB = data << 12;
    put_serial_data(data);
  }
}