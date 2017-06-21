#include <p33FJ32MC302.h>

_FOSCSEL(FNOSC_FRCPLL); //内部オシレータ使用
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_NONE ); //クロック切り替えモードOFF、OSCOは通常のI/Oピン、主発振器無効(内部オシレータ使用のため)
_FWDT(FWDTEN_OFF); //ウォッチドッグタイマーOFF
_FGS(GCP_OFF); //コードプロテクトOFF

#define FCY 30401250UL //システムクロックサイクル(30.40125MIPS)
void Delay_ms(unsigned int ms)  { while(ms--) __delay32(FCY / 1000); }

int main(void) {
  unsigned long i;
  int count = 0;
  TRISA = 0x001f; //Aポートの入出力設定
  TRISB = 0x0103; //Bポートの入出力設定
  T5CONbits.TON = 1; //タイマを使う宣言
  //時間を制御内部の周波数OR外部の時計と連携するか
  //外部とはボタンやタッチセンサのこと
  T5CONbits.TCS = 0;
  //プリスケーラ
  //何回に１回通すか
  //16進数の書き方で書く
  //0x3 = 1:256, 0x2 = 1:64, 0x1 = 1:8, 0x0 = 1:1
  T5CONbits.TCKPS = 0x3;

  PR5 = 59376;

  while (1) {
    if (IFS1bits.T5IF) {
      count ++;
      if (count % 4 == 0 || count % 4 == 1) { //奇数回の処理
        _LATB12 = 1; //LEDを点灯
      }
      else { //偶数回の処理
        _LATB12= 0; //LEDを消灯
      }
      IFS1bits.T5IF = 0; //T1IFを0にする。
    }
  }
} 
