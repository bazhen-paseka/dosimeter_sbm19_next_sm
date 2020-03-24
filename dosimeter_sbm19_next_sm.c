/**
* \file
* \version 1.0
* \author bazhen.levkovets
** \date 2018
*
*************************************************************************************
* \copyright	Bazhen Levkovets
* \copyright	Brovary, Kyiv region
* \copyright	Ukraine
*
*************************************************************************************
*
* \brief
*
*/

/*
**************************************************************************
*							INCLUDE FILES
**************************************************************************
*/
	#include "dosimeter_sbm19_sm.h"
/*
**************************************************************************
*							LOCAL DEFINES
**************************************************************************
*/

/*
**************************************************************************
*							LOCAL CONSTANTS
**************************************************************************
*/


/*
**************************************************************************
*						    LOCAL DATA TYPES
**************************************************************************
*/


/*
**************************************************************************
*							  LOCAL TABLES
**************************************************************************
*/

/*
**************************************************************************
*								 MACRO'S
**************************************************************************
*/

/*
**************************************************************************
*						    GLOBAL VARIABLES
**************************************************************************
*/

const unsigned long port_mask_UL[] = {
	1UL<<0,			/*  0 LED PC  0 */
	1UL<<1,			/*  1 LED PC  1 */
	1UL<<2,			/*  2 LED PC  2 */
	1UL<<3,			/*  3 LED PC  3 */
	1UL<<4,			/*  4 LED PC  4 */
	1UL<<5,			/*  5 LED PC  5 */
	1UL<<6,			/*  6 LED PC  6 */
	1UL<<7,			/*  7 LED PC  7 */
	1UL<<8,			/*  8 LED PC  8 */
	1UL<<9, 		/*  9 LED PC  9 */
	1UL<<0x0A,		/* 10 LED PC 10 */
	1UL<<0x0B,		/* 11 LED PC 11 */
	1UL<<0x0C,		/* 12 LED PC 12 */
	1UL<<0x0D,		/* 13 LED PC 13 */
	1UL<<0x0E,		/* 14 LED PC 14 */
	1UL<<0x0F		/* 15 LED PC 15 */
 };

uint8_t 	tim3_flag_u8 			= 0;
uint8_t		update_flag_u8 			= 0;
uint8_t 	led_count_u8			= 0;
uint8_t		electron_array_count_u8	= 0;
uint32_t	electron_hard_count_u32	= 0;
uint32_t 	radiation_u32_arr[VALUE_ARRAY_CNT];

/*
**************************************************************************
*                        LOCAL FUNCTION PROTOTYPES
**************************************************************************
*/
	void LED_Blink(uint8_t _position_u8);
	void Print_radiation(uint32_t _radiation_u32);

	void Port_A_Off (uint8_t _bit_u8);
	void Port_A_On  (uint8_t _bit_u8);
	void Port_B_Off (uint8_t _bit_u8);
	void Port_B_On  (uint8_t _bit_u8);
	void Port_C_Off (uint8_t _bit_u8);
	void Port_C_On  (uint8_t _bit_u8);

	void Print_d1_0 (void);
	void Print_d1_1 (void);
	void Print_d1_2 (void);
	void Print_d1_3 (void);
	void Print_d1_4 (void);
	void Print_d1_5 (void);
	void Print_d1_6 (void);
	void Print_d1_7 (void);
	void Print_d1_8 (void);
	void Print_d1_9 (void);
	void Print_d1_A (void);
	void Print_d1_B (void);
	void Print_d1_C (void);
	void Print_d1_D (void);
	void Print_d1_E (void);
	void Print_d1_F (void);
	void Print_d1_M (void);

	void Print_d2_0 (void);
	void Print_d2_1 (void);
	void Print_d2_2 (void);
	void Print_d2_3 (void);
	void Print_d2_4 (void);
	void Print_d2_5 (void);
	void Print_d2_6 (void);
	void Print_d2_7 (void);
	void Print_d2_8 (void);
	void Print_d2_9 (void);
	void Print_d2_F (void);

/*
**************************************************************************
*                           GLOBAL FUNCTIONS
**************************************************************************
*/

void Dozimeter_set_TIM3_flag(uint8_t _flag) {
	tim3_flag_u8 = _flag;
}
//************************************************************************

void Dozimeter_set_time_between_electrons(void) {
	electron_array_count_u8++;
	if (electron_array_count_u8 >= VALUE_ARRAY_CNT) electron_array_count_u8 = 0;
	radiation_u32_arr[electron_array_count_u8] = TIM4->CNT;
	TIM4->CNT = 0;
	electron_hard_count_u32++;
	update_flag_u8 = 1;
}
//************************************************************************

void Dozimeter_Init(void) {
	int soft_version_arr_int[3];
	soft_version_arr_int[0] = ((SOFT_VERSION) / 100)     ;
	soft_version_arr_int[1] = ((SOFT_VERSION) /  10) %10 ;
	soft_version_arr_int[2] = ((SOFT_VERSION)      ) %10 ;

	char DataChar[100];
	sprintf(DataChar,"\r\n Dosimeter SBM19 2020-march-18 v%d.%d.%d \r\n\tUART3 for debug on speed 115200\r\n\r\n",
			soft_version_arr_int[0], soft_version_arr_int[1], soft_version_arr_int[2]);
	HAL_UART_Transmit(&huart3, (uint8_t *)DataChar, strlen(DataChar), 100);

	for (int i=0; i<VALUE_ARRAY_CNT; i++) {
	  radiation_u32_arr[i] = 60000 / START_RADIATION_VALUE;
	}

	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start(&htim4);
}
//************************************************************************

void Dozimeter_Main(void) {
	char DataChar[100];

	if (tim3_flag_u8 == 1) {
		sprintf(DataChar,"\t\t\t\tTIM3 60Sec. Hard_CNT= %d imp;\r\n", (int)electron_hard_count_u32);
		HAL_UART_Transmit(&huart3, (uint8_t *)DataChar, strlen(DataChar), 100);
		electron_hard_count_u32 = 0;
		tim3_flag_u8 = 0;
	}
	//************************************************************************

	if (update_flag_u8 > 0) {

		uint32_t summa_of_all_array_u32 = 0;
		for (int i=0; i<VALUE_ARRAY_CNT; i++) {
			summa_of_all_array_u32 = summa_of_all_array_u32 + radiation_u32_arr[i];
		}
		uint32_t qnt_electrons_per_60sec_u32 = ( 60000 * VALUE_ARRAY_CNT ) / summa_of_all_array_u32;

		sprintf (DataChar, "%d) \t%04d \t%d \t CNT: %03d \r\n",
				(int)electron_array_count_u8,
				(int)radiation_u32_arr[electron_array_count_u8],
				(int)summa_of_all_array_u32,
				(int)qnt_electrons_per_60sec_u32 );
		HAL_UART_Transmit(&huart3, (uint8_t *)DataChar, strlen(DataChar), 100);

		Print_radiation(qnt_electrons_per_60sec_u32);

		LED_Blink(electron_array_count_u8%4);
		update_flag_u8 = 0;
	}
}

/*
**************************************************************************
*                           LOCAL FUNCTIONS
**************************************************************************
*/

void LED_Blink(uint8_t _position_u8) {
	HAL_GPIO_WritePin(LED__GREEN_GPIO_Port,	LED__GREEN_Pin,	SET);
	HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port,	LED_YELLOW_Pin, SET);
	HAL_GPIO_WritePin(LED____RED_GPIO_Port,	LED____RED_Pin, SET);

	switch (_position_u8) {
		case 0: HAL_GPIO_WritePin(LED__GREEN_GPIO_Port,	LED__GREEN_Pin,	RESET); break;
		case 1: HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port,	LED_YELLOW_Pin, RESET); break;
		case 2: HAL_GPIO_WritePin(LED____RED_GPIO_Port,	LED____RED_Pin,	RESET);	break;
		case 3: HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port,	LED_YELLOW_Pin, RESET); break;
		default: break;
	}
}
//************************************************************************

void Print_radiation(uint32_t _radiation_u32)	{
	uint8_t digit_1_u8 = _radiation_u32 / 10 ;
	uint8_t digit_2_u8 = _radiation_u32 % 10 ;

	if (digit_1_u8== 0) Print_d1_0();
	if (digit_1_u8== 1) Print_d1_1();
	if (digit_1_u8== 2) Print_d1_2();
	if (digit_1_u8== 3) Print_d1_3();
	if (digit_1_u8== 4) Print_d1_4();
	if (digit_1_u8== 5) Print_d1_5();
	if (digit_1_u8== 6) Print_d1_6();
	if (digit_1_u8== 7) Print_d1_7();
	if (digit_1_u8== 8) Print_d1_8();
	if (digit_1_u8== 9) Print_d1_9();
	if (digit_1_u8==10) Print_d1_A();
	if (digit_1_u8==11) Print_d1_B();
	if (digit_1_u8==12) Print_d1_C();
	if (digit_1_u8==13) Print_d1_D();
	if (digit_1_u8==14) Print_d1_E();
	if (digit_1_u8==15) Print_d1_F();
	if (digit_1_u8 >15) Print_d1_M();

	if (digit_2_u8==0) Print_d2_0();
	if (digit_2_u8==1) Print_d2_1();
	if (digit_2_u8==2) Print_d2_2();
	if (digit_2_u8==3) Print_d2_3();
	if (digit_2_u8==4) Print_d2_4();
	if (digit_2_u8==5) Print_d2_5();
	if (digit_2_u8==6) Print_d2_6();
	if (digit_2_u8==7) Print_d2_7();
	if (digit_2_u8==8) Print_d2_8();
	if (digit_2_u8==9) Print_d2_9();
	if (digit_2_u8 >9) Print_d2_F();
}
//************************************************************************

/*----------------------------------------------------------------------------
  Switch on
 *----------------------------------------------------------------------------*/
void Port_A_Off (uint8_t _bit_u8)	{	GPIOA->BRR  = port_mask_UL[_bit_u8];	}
void Port_A_On  (uint8_t _bit_u8)	{	GPIOA->BSRR = port_mask_UL[_bit_u8];	}

void Port_B_Off (uint8_t _bit_u8)	{	GPIOB->BRR  = port_mask_UL[_bit_u8];	}
void Port_B_On  (uint8_t _bit_u8)	{	GPIOB->BSRR = port_mask_UL[_bit_u8];	}

void Port_C_Off (uint8_t _bit_u8)	{	GPIOC->BRR  = port_mask_UL[_bit_u8];	}
void Port_C_On  (uint8_t _bit_u8)	{	GPIOC->BSRR = port_mask_UL[_bit_u8];	}

/*----------------------------------------------------------------------------
  Letter
 *----------------------------------------------------------------------------*/
void Print_d1_0 (void)	{
	Port_A_On  (12);	//	a
	Port_A_On  (11);	//	b
	Port_A_On  ( 8);	//	c
	Port_B_On  (15);	//	d
	Port_B_On  (14);	//	e
	Port_B_On  (13);	//	f
	Port_B_Off (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_1 (void)	{
	Port_A_Off (12);	//	a
	Port_A_On  (11);	//	b
	Port_A_On  ( 8);	//	c
	Port_B_Off (15);	//	d
	Port_B_Off (14);	//	e
	Port_B_Off (13);	//	f
	Port_B_Off (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_2 (void)	{
	Port_A_On  (12);	//	a
	Port_A_On  (11);	//	b
	Port_A_Off ( 8);	//	c
	Port_B_On  (15);	//	d
	Port_B_On  (14);	//	e
	Port_B_Off (13);	//	f
	Port_B_On  (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_3 (void)	{
	Port_A_On  (12);	//	a
	Port_A_On  (11);	//	b
	Port_A_On  ( 8);	//	c
	Port_B_On  (15);	//	d
	Port_B_Off (14);	//	e
	Port_B_Off (13);	//	f
	Port_B_On  (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_4 (void)	{
	Port_A_Off (12);	//	a
	Port_A_On  (11);	//	b
	Port_A_On  ( 8);	//	c
	Port_B_Off (15);	//	d
	Port_B_Off (14);	//	e
	Port_B_On  (13);	//	f
	Port_B_On  (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_5 (void)	{
	Port_A_On  (12);	//	a
	Port_A_Off (11);	//	b
	Port_A_On  ( 8);	//	c
	Port_B_On  (15);	//	d
	Port_B_Off (14);	//	e
	Port_B_On  (13);	//	f
	Port_B_On  (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_6 (void)	{
	Port_A_On  (12);	//	a
	Port_A_Off (11);	//	b
	Port_A_On  ( 8);	//	c
	Port_B_On  (15);	//	d
	Port_B_On  (14);	//	e
	Port_B_On  (13);	//	f
	Port_B_On  (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_7 (void)	{
	Port_A_On  (12);	//	a
	Port_A_On  (11);	//	b
	Port_A_On  ( 8);	//	c
	Port_B_Off (15);	//	d
	Port_B_Off (14);	//	e
	Port_B_Off (13);	//	f
	Port_B_Off (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_8 (void)	{
	Port_A_On  (12);	//	a
	Port_A_On  (11);	//	b
	Port_A_On  ( 8);	//	c
	Port_B_On  (15);	//	d
	Port_B_On  (14);	//	e
	Port_B_On  (13);	//	f
	Port_B_On  (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_9 (void)	{
	Port_A_On  (12);	//	a
	Port_A_On  (11);	//	b
	Port_A_On  ( 8);	//	c
	Port_B_On  (15);	//	d
	Port_B_Off (14);	//	e
	Port_B_On  (13);	//	f
	Port_B_On  (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_A (void)	{
	Port_A_On  (12);	//	a
	Port_A_On  (11);	//	b
	Port_A_On  ( 8);	//	c
	Port_B_Off (15);	//	d
	Port_B_On  (14);	//	e
	Port_B_On  (13);	//	f
	Port_B_On  (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_B (void)	{
	Port_A_Off (12);	//	a
	Port_A_Off (11);	//	b
	Port_A_On  ( 8);	//	c
	Port_B_On  (15);	//	d
	Port_B_On  (14);	//	e
	Port_B_On  (13);	//	f
	Port_B_On  (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_C (void)	{
	Port_A_On  (12);	//	a
	Port_A_Off (11);	//	b
	Port_A_Off ( 8);	//	c
	Port_B_On  (15);	//	d
	Port_B_On  (14);	//	e
	Port_B_On  (13);	//	f
	Port_B_Off (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_D (void)	{
	Port_A_Off (12);	//	a
	Port_A_On  (11);	//	b
	Port_A_On  ( 8);	//	c
	Port_B_On  (15);	//	d
	Port_B_On  (14);	//	e
	Port_B_Off (13);	//	f
	Port_B_On  (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_E (void)	{
	Port_A_On  (12);	//	a
	Port_A_Off (11);	//	b
	Port_A_Off ( 8);	//	c
	Port_B_On  (15);	//	d
	Port_B_On  (14);	//	e
	Port_B_On  (13);	//	f
	Port_B_On  (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_F (void)	{
	Port_A_On  (12);	//	a
	Port_A_Off (11);	//	b
	Port_A_Off ( 8);	//	c
	Port_B_Off (15);	//	d
	Port_B_On  (14);	//	e
	Port_B_On  (13);	//	f
	Port_B_On  (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d1_M (void)	{
	Port_A_Off (12);	//	a
	Port_A_Off (11);	//	b
	Port_A_Off ( 8);	//	c
	Port_B_Off (15);	//	d
	Port_B_Off (14);	//	e
	Port_B_Off (13);	//	f
	Port_B_On  (12);	//	g
} /*----------------------------------------------------------------------------*/

void Print_d2_0 (void)
	{
	Port_C_On  (13);	// a
	Port_B_On  ( 9);	// b
	Port_B_On  ( 8);	// c
	Port_B_On  ( 5);	// d
	Port_A_On  ( 1);	// e
	Port_A_On  ( 2);	// f
	Port_A_Off ( 3);	// g
} /*----------------------------------------------------------------------------*/

void Print_d2_1 (void)
	{
	Port_C_Off (13);	// a
	Port_B_On  ( 9);	// b
	Port_B_On  ( 8);	// c
	Port_B_Off ( 5);	// d
	Port_A_Off ( 1);	// e
	Port_A_Off ( 2);	// f
	Port_A_Off ( 3);	// g
} /*----------------------------------------------------------------------------*/

void Print_d2_2 (void)
	{
	Port_C_On  (13);	// a
	Port_B_On  ( 9);	// b
	Port_B_Off ( 8);	// c
	Port_B_On  ( 5);	// d
	Port_A_On  ( 1);	// e
	Port_A_Off ( 2);	// f
	Port_A_On  ( 3);	// g
} /*----------------------------------------------------------------------------*/

void Print_d2_3 (void)
	{
	Port_C_On  (13);	// a
	Port_B_On  ( 9);	// b
	Port_B_On  ( 8);	// c
	Port_B_On  ( 5);	// d
	Port_A_Off ( 1);	// e
	Port_A_Off ( 2);	// f
	Port_A_On  ( 3);	// g
} /*----------------------------------------------------------------------------*/

void Print_d2_4 (void)
	{
	Port_C_Off  (13);	// a
	Port_B_On   ( 9);	// b
	Port_B_On   ( 8);	// c
	Port_B_Off  ( 5);	// d
	Port_A_Off  ( 1);	// e
	Port_A_On   ( 2);	// f
	Port_A_On   ( 3);	// g
} /*----------------------------------------------------------------------------*/

void Print_d2_5 (void)
	{
	Port_C_On  (13);	// a
	Port_B_Off ( 9);	// b
	Port_B_On  ( 8);	// c
	Port_B_On  ( 5);	// d
	Port_A_Off ( 1);	// e
	Port_A_On  ( 2);	// f
	Port_A_On  ( 3);	// g
} /*----------------------------------------------------------------------------*/

void Print_d2_6 (void)	{
	Port_C_On  (13);	// a
	Port_B_Off ( 9);	// b
	Port_B_On  ( 8);	// c
	Port_B_On  ( 5);	// d
	Port_A_On  ( 1);	// e
	Port_A_On  ( 2);	// f
	Port_A_On  ( 3);	// g
} /*----------------------------------------------------------------------------*/

void Print_d2_7 (void)	{
	Port_C_On  (13);	// a
	Port_B_On  ( 9);	// b
	Port_B_On  ( 8);	// c
	Port_B_Off ( 5);	// d
	Port_A_Off ( 1);	// e
	Port_A_Off ( 2);	// f
	Port_A_Off ( 3);	// g
	}

void Print_d2_8 (void)	{
	Port_C_On  (13);	// a
	Port_B_On  ( 9);	// b
	Port_B_On  ( 8);	// c
	Port_B_On  ( 5);	// d
	Port_A_On  ( 1);	// e
	Port_A_On  ( 2);	// f
	Port_A_On  ( 3);	// g
} /*----------------------------------------------------------------------------*/

void Print_d2_9 (void)	{
	Port_C_On  (13);	// a
	Port_B_On  ( 9);	// b
	Port_B_On  ( 8);	// c
	Port_B_On  ( 5);	// d
	Port_A_Off ( 1);	// e
	Port_A_On  ( 2);	// f
	Port_A_On  ( 3);	// g
} /*----------------------------------------------------------------------------*/

void Print_d2_F (void)	{
	Port_C_On  (13);	// a
	Port_B_Off ( 9);	// b
	Port_B_Off ( 8);	// c
	Port_B_Off ( 5);	// d
	Port_A_On  ( 1);	// e
	Port_A_On  ( 2);	// f
	Port_A_On  ( 3);	// g
} /*----------------------------------------------------------------------------*/
