/**************** (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾ ****************
* �� �� ��: HCBox.C
* �� �� ��: ����
* ��  ��  : KB-6120E �������¶ȿ���
* ����޸�: 2014��4��21��
*********************************** �޶���¼ ***********************************
* ��  ��: V1.1
* �޶���: ����
* ˵  ��: ������ʾ�ӿڣ���ʾ������״̬
*******************************************************************************/
#include "Pin.H"
#include "BSP.H"
/********************************** ����˵�� ***********************************
*	��������ת��
*******************************************************************************/
#define	fanCountListLen	(4u+(1u+2u))
static	uint16_t	fanCountList[fanCountListLen];
static	uint8_t		fanCountList_index = 0;

uint16_t	HCBoxFan_Circle_Read( void )
{
	return	TIM1->CNT;
}

uint16_t	FanSpeed_fetch( void )
{
	/*	�̶����1s��¼����ת��Ȧ������������
	 *	���μ����������˲��Ľ��������ת�١�
	 */
	uint8_t 	ii, index = fanCountList_index;
	uint16_t	sum = 0u;
	uint16_t	max = 0u;
	uint16_t	min = 0xFFFFu;
	uint16_t	x0, x1, speed;
	x1 = fanCountList[index];
	for ( ii = fanCountListLen - 1u; ii != 0; --ii )
	{
		//	�����������õ��ٶ�
		x0 = x1;
		if ( ++index >= fanCountListLen ){  index = 0u; }
		x1 = fanCountList[index];
		speed = ( x1 - x0 );
		//	�Զ�����ݽ����˲�
		if ( speed > max ) {  max = speed; }
		if ( speed < min ) {  min = speed; }
		sum += speed;
	}

	speed = (uint16_t)( sum - max - min ) / ( fanCountListLen - (1u+2u));
	
	return	speed  * 30u;
}

uint16_t fanspeed;
static	uint16_t	fan_shut_delay = 0u;
void	HCBoxFan_Update( void )
{	
	
	//	�������¼ת��Ȧ��
 	fanCountList[ fanCountList_index] = HCBoxFan_Circle_Read();
	if ( ++fanCountList_index >= fanCountListLen )
	{
		fanCountList_index = 0u;
	}
	fanspeed = FanSpeed_fetch();
	//	���ȿ��ص���̬����
	if ( --fan_shut_delay == 0u )
	{
		HCBoxFan_OutCmd( FALSE );
	}	
}

/********  (C) COPYRIGHT 2014 �ൺ���˴���ӿƼ����޹�˾  **** End Of File ****/


// #if 0

// // ʹ��TIMx��CH1�Ĳ����ܣ���DMA��¼���������.
// #define	CMR_Len	10
// static	uint16_t	CMRA[CMR_Len];

// void	CMR1( void )
// {
// 	DMA_Channel_TypeDef	* DMA1_Channelx = DMA1_Channel2;
// 	TIM_TypeDef * TIMx = TIM1;
// 	
// 	//	DMA1 channel1 configuration
// 	SET_BIT ( RCC->AHBENR,  RCC_AHBENR_DMA1EN );
// 	//	DMAģ�����, ��������
// 	DMA1_Channelx->CCR  = 0u;
// 	DMA1_Channelx->CCR  = DMA_CCR2_PL_0						//	ͨ�����ȼ���01 �е�
// 						| DMA_CCR2_PSIZE_0					//	�ڴ�����λ��01 16λ
// 						| DMA_CCR2_MSIZE_0					//	��������λ��01 16λ
// 						| DMA_CCR2_MINC						//	����ģʽ���ڴ�����
// 						| DMA_CCR2_CIRC						//	ѭ�����䣺ʹ��ѭ��
// 					//	| DMA_CCR6_DIR						//	���ͷ��򣺴������
// 						;
// 	DMA1_Channelx->CPAR  = (uint32_t) &TIM1->CCR1;			//	����DMA�����ַ
// 	DMA1_Channelx->CMAR  = (uint32_t) CMRA;					//	�ڴ��ַ
// 	DMA1_Channelx->CNDTR = CMR_Len;							//	��������
// 	SET_BIT ( DMA1_Channelx->CCR, DMA_CCR1_EN );			//	ʹ��DMAͨ��

// 	//	����TIMx �������벶��
// 	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_TIM1EN );
// 	TIMx->CR1   = 0u;
// 	TIMx->CR2   = 0u;
// 	TIMx->CCER  = 0u;
// 	TIMx->CCMR1 = 0u;
// 	TIMx->CCMR2 = 0u;
// 	//	TIMx ʱ����ʼ��: ����ʱ��Ƶ��24MHz����Ƶ��1MHz�����롣
// 	//	ʱ���������Բ���������ٶ�������ٶȡ�
// 	TIMx->PSC = 240u - 1;	//	10us @ 24MHz
// 	TIMx->ARR = 0xFFFFu;
// 	TIMx->EGR = TIM_EGR_UG;
// 	
// 	TIMx->CCMR1 = TIM_CCMR1_CC1S_0					//	CC1S  : 01b   IC1 ӳ�䵽IT1�ϡ�
// 				| TIM_CCMR1_IC1F_1|TIM_CCMR1_IC1F_0	//	IC1F  : 0011b ���������˲�����8����ʱ��ʱ�������˲�
// 				| TIM_CCMR1_IC2PSC_1				//	IC1PSC: 01b   ���������Ƶ��ÿ��2���¼�����һ�β���
// 				;
// 	TIMx->CCER  = TIM_CCER_CC1E						//	���� CCR1 ִ�в���
// 				| TIM_CCER_CC1P						//	������CCR1�����ź����ڡ�
// 				;
// 	TIMx->DIER  = TIM_DIER_CC1DE;

// 	TIMx->CR1   = TIM_CR1_CEN;						//	ʹ�ܶ�ʱ��

// 	//	���ùܽţ�PA.12 ��������
// 	SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPAEN );
// 	MODIFY_REG( GPIOA->CRH, 0x000F0000u, 0x00040000u );
// }

// uint16_t	fetchSpeed( void )
// {	//	ȡ DMA ���� �� �ڴ��ַָ�룬��������ǰ�������Ρ�
// 	//	���DMA���� �� �ڴ�ָ�붼�����ã�ȡN�εĲ�ֵ�����������ֵ����Сֵ��
// 	
// 	/*	�̶����1s��¼����ת��Ȧ������������
// 	 *	���μ����������˲��Ľ��������ת�١�
// 	 */
// 	DMA_Channel_TypeDef	* DMA1_Channelx = DMA1_Channel2;
// 	uint8_t 	ii, index;
// 	uint16_t	sum = 0u;
// //	uint16_t	max = 0u;
// //	uint16_t	min = 0xFFFFu;
// 	uint16_t	x0, x1, period;

// 	index = ( DMA1_Channelx->CMAR - ( uint32_t ) CMRA ) / sizeof( uint16_t);	//	�ڴ��ַ
// 	if ( ++index >= CMR_Len ){  index = 0u; }
// 	if ( ++index >= CMR_Len ){  index = 0u; }
// 	
// 	x1 = CMRA[index];
// 	for ( ii = CMR_Len - 2u; ii != 0; --ii )
// 	{
// 		//	�����������õ��ٶ�
// 		x0 = x1;
// 		if ( ++index >= CMR_Len ){  index = 0u; }
// 		x1 = CMRA[index];
// 		period = (uint16_t)( x1 - x0 );
// 		//	�Զ�����ݽ����˲�
// //		if ( period > max ) {  max = period; }
// //		if ( period < min ) {  min = period; }
// //		sum += period;
// 	}
// 	period = sum / ( CMR_Len - 2u );
// //	period = (uint16_t)( sum - max - min ) / ( CMR_Len - (1u+2u));

// 	if ( period == 0u )
// 	{
// 		return	0xFFFFu;
// 	}
// 	else
// 	{	//	ÿ���ӵļ������� / ÿ������ļ���ʱ�� => ÿ���ӵ�ת��
// 		return	(( 60u * 100000u ) / period );
// 	}
// }

// #endif
/*

****************/