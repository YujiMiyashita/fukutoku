#include <p33FJ32MC302.h>

_FOSCSEL(FNOSC_FRCPLL); //内部オシレータ使用
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_NONE ); //クロック切り替えモードOFF、OSCOは通常のI/Oピン、主発振器無効(内部オシレータ使用のため)
_FWDT(FWDTEN_OFF); //ウォッチドッグタイマーOFF
_FGS(GCP_OFF); //コードプロテクトOFF

#define FCY 30401250UL //システムクロックサイクル(30.40125MIPS)
void Delay_ms(unsigned int ms)	{ while(ms--) __delay32(FCY / 1000); }

int main( void )
{
	int count = 0;
	int rb0_data = 0, rb1_data = 0;
	//クロックの設定 30.40125MIPS
	CLKDIVbits.PLLPRE = 0; //クロック設定N2=2
	CLKDIVbits.PLLPOST = 0; //クロック設定N1=2
	PLLFBD = 31; //クロック設定M=33
	RCONbits.SWDTEN = 0;//ウォッチドッグタイマーオフ

	//入出力設定
	TRISA = 0x001f;//Aポートの設定
	TRISB = 0x0103;//Bポートの設定
	AD1PCFGL = 0xfffc;//アナログ・ディジタル入力の設定

	while( 1 )
	{
		rb0_data = PORTBbits.RB0; //RB0の状態を取得
		rb1_data = PORTBbits.RB1;

		if( rb0_data && rb1_data ) {
		  LATBbits.LATB15 = 1;
		  LATBbits.LATB14 = 1;
		  Delay_ms(1000);
		  LATBbits.LATB15 = 0; //消灯
		  Delay_ms(1000);
		  LATBbits.LATB15 = 1; //消灯
		  LATBbits.LATB14 = 0; //消灯
		  Delay_ms(1000);
		  LATBbits.LATB15 = 0;
		  Delay_ms(1000);
		}

		else if( rb0_data && !rb1_data ) {
		  LATB |= 0x8000;
		  Delay_ms(1000);
		  LATB &= 0x0fff; //消灯
		  Delay_ms(1000);
		}

		else if( !rb0_data && rb1_data ) {
		  LATB |= 0x4000;
		  Delay_ms(2000);
		  LATB &= 0x0fff; //消灯
		  Delay_ms(2000);
		}
	}
}