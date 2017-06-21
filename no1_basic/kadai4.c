#include <p33FJ32MC302.h>

_FOSCSEL(FNOSC_FRCPLL); //内部オシレータ使用
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_NONE ); //クロック切り替えモードOFF、OSCOは通常のI/Oピン、主発振器無効(内部オシレータ使用のため)
_FWDT(FWDTEN_OFF); //ウォッチドッグタイマーOFF
_FGS(GCP_OFF); //コードプロテクトOFF

#define FCY 30401250UL //システムクロックサイクル(30.40125MIPS)
void Delay_ms(unsigned int ms)  { while(ms--) __delay32(FCY / 1000); }

int main( void ) {
  int rb_data0 = 0;
  int rb_data1 = 0;

  //クロックの設定 30.40125MIPS
  CLKDIVbits.PLLPRE = 0; //クロック設定N2=2
  CLKDIVbits.PLLPOST = 0; //クロック設定N1=2
  PLLFBD = 31; //クロック設定M=33
  RCONbits.SWDTEN = 0;//ウォッチドッグタイマーオフ

  //入出力設定
  TRISA = 0x001f;//Aポートの設定
  TRISB = 0x0103;//Bポートの設定
  AD1PCFGL = 0xfffc;//アナログ・ディジタル入力の設定

  while( 1 ) {
    rb_data0 = PORTBbits.RB0;
    rb_data1 = PORTBbits.RB1;
    
    //BT0が押されている場合
    if( rb_data0 ) {
      LATB |= 0x3000; // LED点灯
      LATBbits.LATB7 = 1;
      Delay_ms(1000);
      LATBbits.LATB7 = 0;
      LATB &= 0x0fff; // LED消灯
    }
    else if ( rb_data1 ) {
      LATB |= 0xc000; // LED点灯
      LATBbits.LATB7 = 1;
      Delay_ms(1000);
      LATBbits.LATB7 = 0;
      LATB &= 0x0fff; // LED消灯
    }
  }
}