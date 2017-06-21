#include <p33FJ32MC302.h>

_FOSCSEL(FNOSC_FRCPLL); //内部オシレータ使用
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_NONE ); //クロック切り替えモードOFF、OSCOは通常のI/Oピン、主発振器無効(内部オシレータ使用のため)
_FWDT(FWDTEN_OFF); //ウォッチドッグタイマーOFF
_FGS(GCP_OFF); //コードプロテクトOFF

#define FCY 30401250UL //システムクロックサイクル(30.40125MIPS)
void Delay_ms(unsigned int ms)  { while(ms--) __delay32(FCY / 1000); }

int main(void) {
  unsigned long i;
  AD1PCFGL = 0xfffc;//アナログ・ディジタル入力の設定
  TRISA = 0x001f; //Aポートの入出力設定
  TRISB = 0x0103; //Bポートの入出力設定
  T4CONbits.TON = 1; //タイマを使う宣言
  //時間を制御内部の周波数OR外部の時計と連携するか
  //外部とはボタンやタッチセンサのこと
  T4CONbits.TCS = 1;
  //プリスケーラ
  //何回に１回通すか
  //16進数の書き方で書く
  //0x3 = 1:256, 0x2 = 1:64, 0x1 = 1:8, 0x0 = 1:1
  T4CONbits.TCKPS = 0x0;
  //どのピンに入力させるのか
  //今回ならRB0とタッチセンサの組み合わせが該当する
  _T4CKR = 0; //この0はRB0の「0」

  while (1) {
    LATB = TMR4 << 12;
  }
} 
