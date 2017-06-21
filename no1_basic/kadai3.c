#include <p33FJ32MC302.h>
#include <stdlib.h>

_FOSCSEL(FNOSC_FRCPLL); //?部オシレータ使用
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_NONE ); //クロ?ク?り替えモードOFF、OSCOは通常のI/Oピン、主発振器無効(?部オシレータ使用のため)
_FWDT(FWDTEN_OFF); //ウォ?チド?グタイマ??OFF
_FGS(GCP_OFF); //コード??ロ?ク?OFF

#define FCY 30401250UL //シス??クロ?クサイクル(30.40125MIPS)
void Delay_ms(unsigned int ms)	{ while(ms--) __delay32(FCY / 1000); }

int light_up();

int main( void )
{
	int count = 0;
	int rb0_data = 0, rb1_data = 0;
	int array[] = {0x8000, 0x4000, 0x2000, 0x1000};
	int random_num = 0;
	//クロ?クの設? 30.40125MIPS
	CLKDIVbits.PLLPRE = 0; //クロ?ク設定N2=2
	CLKDIVbits.PLLPOST = 0; //クロ?ク設定N1=2
	PLLFBD = 31; //クロ?ク設定M=33
	RCONbits.SWDTEN = 0;//ウォ?チド?グタイマ??オ?

	//入出力設?
	TRISA = 0x001f;//Aポ??ト??設?
	TRISB = 0x0103;//Bポ??ト??設?
	AD1PCFGL = 0xfffc;//アナログ・?ィジタル入力??設?

	while( 1 )
	{
		rb0_data = PORTBbits.RB0; //RB0の状態を取?
		rb1_data = PORTBbits.RB1;
		random_num = rand() % 4;

		if( rb0_data || rb1_data ) {
		  light_up();
		  LATB |= array[random_num];
		  Delay_ms(1000);
		  LATB &= 0x0fff; //消??
		}
	}
}

int light_up() {
  LATB |= 0x8000;
  Delay_ms(1000);
  LATB &= 0x0fff; //消??
  LATB |= 0x4000;
  Delay_ms(1000);
  LATB &= 0x0fff; //消??
  LATB |= 0x2000;
  Delay_ms(1000);
  LATB &= 0x0fff; //消??
  LATB |= 0x1000;
  Delay_ms(1000);
  LATB &= 0x0fff; //消??
}