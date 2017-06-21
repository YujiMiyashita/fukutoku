#include <p33FJ32MC302.h>

_FOSCSEL(FNOSC_FRCPLL); //内部オシレータ使用
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_NONE ); //クロック切り替えモードOFF、OSCOは通常のI/Oピン、主発振器無効(内部オシレータ使用のため)
_FWDT(FWDTEN_OFF); //ウォッチドッグタイマーOFF
_FGS(GCP_OFF); //コードプロテクトOFF

#define FCY 30401250UL //システムクロックサイクル(30.40125MIPS)
int main(void) {
  int count = 0;
  TRISA = 0x001f; //Bポートの入出力設定
  TRISB = 0x0103; //Cポートの入出力設定
  T1CONbits.TON = 1; //TONを1にしてタイマ1を有効に設定
  T1CONbits.TCS = 0; //dsPICの動作サイクルでカウントアップするように設定
  T1CONbits.TCKPS = 0x3; //TCKPS<1:0>を11に設定(プリスケーラ256)
  
  PR1 = 59376; //PR1の設定(約0.5秒でオーバーフローするように設定)

  while (1) {
    if (IFS0bits.T1IF) { //T1IFが1なら(約0.5秒経過)
      count ++;
      if (count % 8 < 4) { //奇数回の処理
        _LATB12 = 1; //LEDを点灯
      }
      else { //偶数回の処理
        _LATB12= 0; //LEDを消灯
      }
      IFS0bits.T1IF = 0; //T1IFを0にする。
    }
  }
}