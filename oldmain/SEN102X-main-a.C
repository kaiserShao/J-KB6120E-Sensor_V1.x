#include "BSP.H"
#include <string.h>
extern	void	MODBUS_Init( uint8_t MBAddress );
extern	uint16_t	usRegInputBuf[];
extern	uint16_t	usRegHoldingBuf[];
extern	uint8_t		ucRegDiscBuf[];
extern	uint8_t		ucRegCoilsBuf[];

#define	_EE_SLAVE_BASE	2500u
#define	_EE_IMAGE_BASE	2048u
#define	_EE_IMAGE_LEN	100u

uint16_t	get_AD7705( uint8_t	SelectBPGA );
uint16_t	get_AD7705( uint8_t	SelectBPGA )
{
	uint32_t	Sum = 0u;

	Sum += Readout_AD7705( SelectBPGA );
	Sum += Readout_AD7705( SelectBPGA );
	Sum += Readout_AD7705( SelectBPGA );
	Sum += Readout_AD7705( SelectBPGA );

	return	(uint16_t)( Sum / 4u );
	
// 	delay( 80u );
// 	
// 	return	get_CPU_Temp();
}

////////////////////////////////////////////////////////////////////////////////
#define	ArrayLen	128u
volatile uint16_t	Array[ArrayLen];
uint16_t	ArrayIndex = 0u;

void	FilterArrayIn( uint16_t InValue )
{
	if ( ++ArrayIndex >= ArrayLen )
	{
		ArrayIndex = 0u;
	}
	Array[ArrayIndex] = InValue;
}

void	FilterArrayOut( uint16_t OutResult[] )
{
	const	static	uint16_t	NSEL[] =
	{
		2, 		//	ȡ��� 0.16 ��ľ�ֵ
		4,  	//	ȡ��� 0.32 ��ľ�ֵ
		8,  	//	ȡ��� 0.64 ��ľ�ֵ
		12, 	//	ȡ��� 0.96 ��ľ�ֵ
		25, 	//	ȡ��� 2.00 ��ľ�ֵ
		60, 	//	ȡ��� 4.80 ��ľ�ֵ
		125 	//	ȡ��� 10.0 ��ľ�ֵ
	};

	uint16_t	Index = ArrayIndex;
	uint16_t	Select = 0u;
	uint16_t	Count = 0u;
	uint32_t	Sum = 0u;
	
	for ( Select = 0u; Select < sizeof( NSEL) / sizeof(*NSEL); ++Select )
	{
		uint8_t	FilterLen = NSEL[Select];
		
		do {
			Sum += Array[Index];
			if ( 0u == Index )
			{
				Index = ArrayLen;
			}
			--Index;
		} while ( ++Count < FilterLen );
	
		OutResult[Select] = Sum / Count;
	}
}

////////////////////////////////////////////////////////////////////////////////

int32_t	main( void )
{
	uint16_t	Result7705[8];
	uint16_t	Config7705;
	uint8_t		MBAddress;
	uint16_t	i;
	
	//	����������ʼ��
	BIOS_Init();
	
	for ( i = 0; i < 10u; ++i )
	{
		usRegHoldingBuf[i] = 0u;
	}

	for ( i = 0; i < 40u; ++i )
	{
		usRegInputBuf[i] = 0u;
	}
	
//	for ( i = 0; i < ArrayLen - 1; ++i )
//	{
//		FilterArrayIn( 0u );
//	}
	
//	get_Unique96(( void * ) &usRegInputBuf[13] );

	Initialize_AD7705();

	//	��ȡ�ڲ�EEPROM
//	Vload( _EE_IMAGE_BASE, &usRegHoldingBuf[10], _EE_IMAGE_LEN );
//	Vload( _EE_SLAVE_BASE, &MBAddress, sizeof( MBAddress ));
	Vload( _EE_IMAGE_BASE, &usRegHoldingBuf[10], _EE_IMAGE_LEN );
	Vload( _EE_SLAVE_BASE, &MBAddress, sizeof( MBAddress ));
	if (( MBAddress < 1u ) || ( MBAddress > 247u ))
	{
		MBAddress = 1u;
	}

	//	��ʼ��MODBUSЭ��ջ
	MODBUS_Init( 1 );	//	MODBUS_Init( MBAddress );
	
	//	���Ź�����
	for(;;)
	{
		//	���Ź�����


		//	�����������ʾ�ӻ���������
		++usRegInputBuf[0];
		delay(500);
		
		++usRegInputBuf[15];
		++usRegInputBuf[16];
		++usRegInputBuf[17];
		
	}
//	
//		++usRegInputBuf[10];
//		++usRegInputBuf[11];
//		++usRegInputBuf[12];
//		++usRegInputBuf[13];
//		//	ȡ�� CPU �¶ȶ���
//		usRegInputBuf[1] = get_CPU_Temp();

//		//	���� �����Դ ��ѹ
//		usRegInputBuf[2] = get_Bat_Volt();

//		//	���� ��ѹ���� ��ѹ
//		usRegInputBuf[3] = get_HV_Volt();

//		//	ȡ��AD7705�Ķ���
//		Config7705 = usRegHoldingBuf[2];
//		Result7705[0] = get_AD7705( Config7705 & 0x00FFu ); //	����ѭ��ʱ�����Ҫ���أ�����ֵΪ80ms
//		FilterArrayIn( Result7705[0] );
//		FilterArrayOut( Result7705 + 1u );

//		usRegInputBuf[4] = Result7705[( Config7705 >> 8  ) % 8u];
//		usRegInputBuf[5] = Result7705[( Config7705 >> 12 ) % 8u];
//		usRegInputBuf[ 6] = Result7705[1];
//		usRegInputBuf[ 7] = Result7705[2];
//		usRegInputBuf[ 8] = Result7705[3];
//		usRegInputBuf[ 9] = Result7705[4];
//		usRegInputBuf[10] = Result7705[5];
//		usRegInputBuf[11] = Result7705[6];
//		usRegInputBuf[12] = Result7705[7];

//		//	�����Լ�LED
//		if ( 0u != usRegHoldingBuf[0] )
//		{
//			PWM1_OutputSet( usRegHoldingBuf[0] );	//	���PWM
//		}
//		else
//		{
//			PWM1_OutputSet( 0u );					//	�ر�PWM
//		}

//		//	���Ƹ�ѹ
//		if ( 0u != usRegHoldingBuf[1] )
//		{	
//			DAC1_OutputSet( usRegHoldingBuf[1] );
//			HVPower_OutCmd( TRUE );					//	�򿪸�ѹ
//		}
//		else
//		{
//			DAC1_OutputSet( 0u );
//			HVPower_OutCmd( FALSE );				//	�رո�ѹ
//		}

//		//	�������ݿ�������
//		if ( 0u != usRegHoldingBuf[9] )
//		{
//			if ( 0u == memcmp( &usRegHoldingBuf[3], &usRegInputBuf[13], 12 ))
//			{
//				uint16_t	DataControlWord = usRegHoldingBuf[9];
//				uint8_t	HiByte, LoByte;
//				
//				HiByte = DataControlWord >> 8;
//				LoByte = DataControlWord & 0xFFu;
//			
//				if ( (uint8_t)( ~HiByte ) == LoByte )
//				{
//					switch ( LoByte )
//					{
//					case 0x00u:
//					case 0xF8u:
//					case 0xF9u:
//					case 0xFAu:
//					case 0xFBu:
//					case 0xFCu:	break;

//					default:	//	1u - 247u �޸Ĵӻ���ַ����
//						MBAddress = LoByte;
//						Vsave( _EE_SLAVE_BASE, &MBAddress, sizeof( MBAddress ));
//						break;
//					case 0xFDu:	//	Write Disable
//						usRegInputBuf[19] = FALSE;
//						NVRAM_Write_Disable();
//						break;
//					case 0xFEu:	//	Write Enable
//						usRegInputBuf[19] = TRUE;
//						NVRAM_Write_Enable();
//						break;
//					case 0xFFu:	//	�����ڲ�EEPROMӳ��
//						Vsave( _EE_IMAGE_BASE, &usRegHoldingBuf[10], _EE_IMAGE_LEN );
//						break;
//					}
//					//	��������ɣ����������־����λ������
//					memset( &usRegHoldingBuf[3], 0u, 12 + 2 );
//				}
//			}
//		}
//	}
}