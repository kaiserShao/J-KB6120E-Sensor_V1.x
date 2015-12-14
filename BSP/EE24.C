/**************** (C) COPYRIGHT 2012 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: EE24.C
* �� �� ��: Dean
* ��  ��  : ��д24ϵ�е�EEPROM����
*         : 
* ����޸�: 2012��4��14��
*********************************** �޶���¼ ***********************************
* ��  ��: 
* �޶���: 
*******************************************************************************/

#include "BSP.H"
//	#include "Pin.H"


/*******************************************************************************
* Function Name  : I2C_EE_WaitEepromStandbyState
* Description    : Wait for EEPROM Standby state
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static	BOOL	polling( uint8_t SlaveAddress  )
{
	uint16_t	iRetry;
	
	/*	����������400Kʱ, 10msʱ����෢��400����ַ�ֽ� */
	for ( iRetry = 400U; iRetry != 0x00U; --iRetry )
	{
		if ( bus_i2c_start( SlaveAddress, I2C_Write ))
		{
			bus_i2c_stop();
			return	TRUE;
		}
	}
	return	FALSE;
}

/*******************************************************************************
* ��������: FM24C04A_save/load
* ����˵��: FM24C04A ��ȡ
* �������: ��ַ/ָ��/���ݳ���
* �������: None
* �� �� ֵ: None
*******************************************************************************/
BOOL	_EE_Save_inside_page( uint16_t address, uint8_t const * buffer, uint8_t count )
{
	// send sub address
	if ( ! bus_i2c_start( _SLAVE_AT24C02, I2C_Write ))	{ bus_i2c_stop(); return FALSE; }
//    if ( ! bus_i2c_shout((uint8_t)( address / 0x100U ))){ bus_i2c_stop(); return FALSE; }
    if ( ! bus_i2c_shout((uint8_t)( address % 0x100U ))){ bus_i2c_stop(); return FALSE; }

	// continue send write data.
    do
	{
		if ( ! bus_i2c_shout((uint8_t)~(*buffer++))){	bus_i2c_stop(); return FALSE; 	}
	}
	while ( --count );

    bus_i2c_stop();

    // acknowledge polling.
	return	polling( _SLAVE_AT24C02 );	/*	����洢������Ҫ polling, ������Ҳ����ν */
}

BOOL	_EE_Load_inside_page( uint16_t address, uint8_t * buffer, uint8_t count )
{
	// send sub address
	if ( ! bus_i2c_start( _SLAVE_AT24C02, I2C_Write )) { bus_i2c_stop(); return FALSE; }
//	if ( ! bus_i2c_shout((uint8_t)( address / 0x100U ))){ bus_i2c_stop(); return FALSE; }
	if ( ! bus_i2c_shout((uint8_t)( address % 0x100U ))){ bus_i2c_stop(); return FALSE; }

	// Send read command and receive data.
	if ( ! bus_i2c_start( _SLAVE_AT24C02, I2C_Read ))	{ bus_i2c_stop(); return FALSE; }
	while ( --count )
	{
		*buffer++ =  (uint8_t)~bus_i2c_shin( I2C_ACK );	// Receive and send ACK
	}
	*buffer =  (uint8_t)~bus_i2c_shin( I2C_NoACK );		// Receive and send NoACK
	bus_i2c_stop();

	return TRUE;
}

BOOL	_EE_Load( uint16_t address, uint8_t * buffer, uint8_t count )
{
	uint8_t	len = _EE_Page_Len - ( address % _EE_Page_Len );
	
	while ( len < count )
	{
		if ( !_EE_Load_inside_page( address, buffer, len ))
		{
			return	FALSE;
		}
		address += len;
		buffer  += len;
		count   -= len;
		
		len = _EE_Page_Len;
	}
	return	_EE_Load_inside_page( address, buffer, count );
}

BOOL	_EE_Save( uint16_t address, uint8_t const * buffer, uint8_t count )
{
	uint8_t	len = _EE_Page_Len - ( address % _EE_Page_Len );	//	len �㲻���� 0
	
	while ( len < count )
	{
		if ( !_EE_Save_inside_page( address, buffer, len ))
		{
			return	FALSE;
		}
		address += len;
		buffer  += len;
		count   -= len;		// �� (len < count ), �� ( count - len ) > 0
		
		len = _EE_Page_Len;
	}
	return	_EE_Save_inside_page( address, buffer, count );
}

////////////////////////////////////////////////////////////////////////////////
BOOL	E24C02_Load( uint8_t address, void * buffer, uint8_t count )
{
	BOOL	state;

	bus_i2c_mutex_apply();
	state = _EE_Load( address, buffer, count );
	bus_i2c_mutex_release();
	
	return	state;
}

BOOL	E24C02_Save( uint8_t address, void const * buffer, uint8_t count )
{
	BOOL	state;
	
	bus_i2c_mutex_apply();
	state = _EE_Save( address, buffer, count );
	bus_i2c_mutex_release();

	return	state;
}

/********  (C) COPYRIGHT 2012 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/