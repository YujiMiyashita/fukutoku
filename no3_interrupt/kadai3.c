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

void _ISR _INT2Interrupt(void) {
  static int count = 0;
  count ++;

  if (count % 2) {
    LATB |= 0x7000;
  }
  else {
    LATB &= 0x0fff;
  }
  Delay_ms(300);
  _INT2IF = 0;
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

  //入力変化割込のピン設定（RB0ボタン）
  _INT2R = 0; 

  //入力割り込み関数呼び出し
  _INT2IE = 1;
  _INT2IF = 0;

  while (1);
}