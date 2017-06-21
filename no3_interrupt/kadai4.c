#include <p33FJ32MC302.h>

_FOSCSEL(FNOSC_FRCPLL);
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_NONE );
_FWDT(FWDTEN_OFF);
_FGS(GCP_OFF);

#define FCY 30401250UL

void Delay_ms(unsigned int ms) {
  while(ms--)
    __delay32(FCY / 1000);
}

void _ISR _CNInterrupt(void) {
  static int count = 0;
  count ++;
  if (count % 2) {
    LATBbits.LATB7 = 1;
  }
  else { 
    LATBbits.LATB7 = 0;
  }
  Delay_ms(300);//0.5秒待つ
  _CNIF = 0;
}

void _ISR _T3Interrupt(void) {
  static int count = 0;
  count ++;
  if (count % 2) {
    LATB |= 0x8000;
  }
  else {
    LATB &= 0x0fff;
  }
  _T3IF = 0;
}

void _ISR _T4Interrupt(void) {
  static int count = 0;
  count ++;
  if (count % 2) {
    LATB |= 0x1000;
  }
  else {
    LATB &= 0x0fff;
  }
  _T4IF = 0;
}

int main(void) {
  //初期設定
  CLKDIVbits.PLLPRE = 0;
  CLKDIVbits.PLLPOST = 0;
  PLLFBD = 31;
  RCONbits.SWDTEN = 0;
  TRISA = 0x001f;
  TRISB = 0x0103;
  AD1PCFGL = 0xfffc;

  //タイマ２設定
  T2CONbits.TON = 1;
  T2CONbits.TCKPS = 0x3;
  T2CONbits.TCS = 0;

  //タイマ２と３は32ビットで使用
  T2CONbits.T32 = 1;

  //タイマ３設定
  T3CONbits.TON = 1;
  T3CONbits.TCKPS = 0x3;
  T3CONbits.TCS = 0;

  //タイマ４設定
  T4CONbits.TON = 1;
  T4CONbits.TCS = 0;
  T4CONbits.TCKPS = 0x3;
  PR4 = 59376;

  //１秒でオーバーフローするように設定
  PR3 = 0x0001;
  PR2 = 0xcfe2;

  //RB0が押された時に入力割り込みをするように設定  
  _CN4IE = 1;

  _T3IF = 0;
  _T3IE = 1;

  _T4IF = 0;
  _T4IE = 1;

  _CNIE = 1;
  _CNIF = 0;

  while (1);
}