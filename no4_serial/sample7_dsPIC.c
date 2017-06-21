#include <p33FJ128MC802.h>

_FOSCSEL(FNOSC_FRCPLL)//�����I�V���[�^�g�p
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_NONE ) //�N���b�N�؂�ւ����[�hOFF�AOSCO�͒ʏ��I/O�s���A�唭�U�햳��(�����I�V���[�^�g�p�̂���)
_FWDT(FWDTEN_OFF) //�E�H�b�`�h�b�O�^�C�}�[OFF
_FGS(GCP_OFF) //�R�[�h�v���e�N�gOFF

#define FCY 30401250UL
void Delay_ms(unsigned int ms)  { while(ms--) __delay32(FCY / 1000); }

// �񓯊��V���A�����M�p�̊֐�(data�͑��M����f�[�^)
void put_serial_data( int data )
{
  //�f�[�^�𑗐M����܂ŉi�v�ɑ҂B
  do{                 // �������[�v
    if( !U1STAbits.UTXBF ){     // ���M�p�o�b�t�@���󂢂Ă��邩�𒲂ׂ�
      U1TXREG = data;       // ���M�p�o�b�t�@�Ƀf�[�^�������đ��M�J�n
      return;           // ���M�����I��
    }
  }while( 1 );            // ���M�\�łȂ���Α��M�\�ɂȂ�܂ő҂B
}

// �񓯊��V���A����M�p�̊֐�(�߂�l���f�[�^)
int get_serial_data( void )
{
  int data; // ��M�f�[�^��錾

  // �f�[�^������܂ŉi�v�ɑ҂B
  do{                 // �������[�v
    while( U1STAbits.URXDA ){     // �V������M�f�[�^�����Ă��邩���f����
      if( U1STAbits.FERR ){     // �t���[�~���O�G���[�̃`�F�b�N
        data = U1RXREG;     // �f�[�^�����[�h
        U1STAbits.FERR = 0;   // �t���[�~���O�G���[�̃N���A
        if( U1STAbits.OERR ){   // �I�[�o�[�����G���[�̃`�F�b�N
          U1STAbits.OERR = 0; // �I�[�o�[�����G���[���N���A            }
          break;        // �������[�v�ɖ߂�
        }
      }
      if( U1STAbits.OERR ){     // �I�[�o�[�����G���[�̃`�F�b�N
        U1STAbits.OERR = 0;   // �I�[�o�[�����G���[���N���A
        break;          // �������[�v�ɖ߂�
      }
      data = U1RXREG;       // �G���[��������Ύ�M�f�[�^��ǂށB
      return data;        // ��M�����f�[�^��Ԃ��Ď�M�����I���B
    }
  }while( 1 );            // �����܂Ŗ������[�v
}

//main��
void main( void ){
  // �N���b�N�̐ݒ� 30.40125MIPS
  CLKDIVbits.PLLPRE = 0;        // �N���b�N�ݒ�N2=2
  CLKDIVbits.PLLPOST = 0;       // �N���b�N�ݒ�N1=2
  PLLFBD = 31;            // �N���b�N�ݒ�M=33
  RCONbits.SWDTEN = 0;        // �E�H�b�`�h�b�O�^�C�}�[�I�t

  // ���o�͐ݒ�
  TRISA = 0x001f;           // A�|�[�g�̐ݒ�
  TRISB = 0x0203;           // B�|�[�g�̐ݒ�
  AD1PCFGL = 0xfffc;          // �A�i���O�E�f�B�W�^�����͂̐ݒ�

  // UART1�̐ݒ�
  U1MODE = 0x8808;          // UART�ʐM���[�h�̐ݒ�
  U1STA  = 0x0400;          // UART�̏�ԂƐ���̐ݒ�
  U1BRG  = 6333;            // �{�[���C�g��2400��1200bps�ɕύX

  // ���o�̓|�[�g�̐ݒ�

  //�s�����V���A���ʐM�̎�M�Ƃ��Đݒ�
  //��f�̏ꍇ�AU1RXR = (�s���̔ԍ�) �Ƃ���
  _U1RXR = 9;   

  //RPINR19bits.U2RXR = 9;
  //�s�����V���A���ʐM�̑��M�Ƃ��Đݒ�
  //�s���̔ԍ� = (�@�\)
  //3��U1�ɑ��M�A5�ɂ����U2�ɑ��M
  _RP8R  = 3;


  int data;

  // GGR
  Delay_ms( 10 );
  while( U1STAbits.URXDA )get_serial_data();

  while(1){             // �������[�v
    data = get_serial_data();   // �f�[�^����M����B
    LATB = data << 12;        // ��M�����f�[�^��PORTB�ɏo�͂���B
    put_serial_data( data );    // ��M�����f�[�^�����̂܂ܑ��M����B
  }                 // �����܂Ŗ������[�v
}