#include <p33FJ32MC302.h>

_FOSCSEL(FNOSC_FRCPLL);
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_NONE );
_FWDT(FWDTEN_OFF);
_FGS(GCP_OFF);

#define FCY 30401250UL

void _ISR _T4Interrupt(void) {
  static int count = 0;
  count ++;
  if (count % 2) {
    LATB |= 0xf000;
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

  //タイマ設定
  T4CONbits.TON = 1;
  T4CONbits.TCS = 0;
  T4CONbits.TCKPS = 0x3;
  PR4 = 29688;

  _T4IF = 0;
  _T4IE = 1;

  while (1);
}