/*
 * RGB_control.c
 *
 *  Created on: Jul 5, 2024
 *      Author: LeeJH
 */
#include "RGB_control.h"
#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim3;

RGB_CONFIG rgb_config;

/**
 * \fn void RGB_Control_init(void)
 * \brief
 */
void RGB_Control_init(void)
{
	rgb_config.Brightness = 0;
	rgb_config.state = RGB_OFF;
	rgb_config.Color = RGB_COLOR_WHITE;
	rgb_config.menu = RGB_MENU_NONE;

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // Red pin pwm timer start
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3); // Green pin pwm timer start
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4); // Blue pin pwm timer start
}

/**
 * \fn void RGB_Control_Power_On(void)
 * \brief
 */
void RGB_Control_Power_On(void)
{
	if(rgb_config.state == RGB_ON) return;

	if(rgb_config.Brightness == 0) rgb_config.Brightness = 100;

	rgb_config.state = RGB_ON;

	switch (rgb_config.Color)
	{
		case RGB_COLOR_RED:
			htim3.Instance->CCR1 = rgb_config.Brightness - 1;
		break;
		case RGB_COLOR_GREEN:
			htim3.Instance->CCR3 = rgb_config.Brightness - 1;
		break;
		case RGB_COLOR_BLUE:
			htim3.Instance->CCR4 = rgb_config.Brightness - 1;
		break;
		case RGB_COLOR_WHITE:
			htim3.Instance->CCR1 = rgb_config.Brightness - 1;
			htim3.Instance->CCR3 = rgb_config.Brightness - 1;
			htim3.Instance->CCR4 = rgb_config.Brightness - 1;
		break;
		default:
			break;
	}
}

/**
 * \fn void RGB_Control_Power_Off(void)
 * \brief
 */
void RGB_Control_Power_Off(void)
{
	if(rgb_config.state == RGB_OFF) return;

	rgb_config.state = RGB_OFF;

	htim3.Instance->CCR1 = 0;
	htim3.Instance->CCR3 = 0;
	htim3.Instance->CCR4 = 0;
}

/**
 * \fn void RGB_Control_Set_Brightness(uint8_t)
 * \brief
 * \param val
 */
void RGB_Control_Set_Brightness(uint8_t val)
{
	if (rgb_config.state == RGB_OFF)
		return;

	rgb_config.state = RGB_ON;
	if (val > 100)
		val = 100;
	else if (val < 10)
		val = 10;
	else
	{
	}

	rgb_config.Brightness = val;

	switch (rgb_config.Color)
	{
		case RGB_COLOR_RED:
			htim3.Instance->CCR1 = rgb_config.Brightness - 1;
		break;
		case RGB_COLOR_GREEN:
			htim3.Instance->CCR3 = rgb_config.Brightness - 1;
		break;
		case RGB_COLOR_BLUE:
			htim3.Instance->CCR4 = rgb_config.Brightness - 1;
		break;
		case RGB_COLOR_WHITE:
			htim3.Instance->CCR1 = rgb_config.Brightness - 1;
			htim3.Instance->CCR3 = rgb_config.Brightness - 1;
			htim3.Instance->CCR4 = rgb_config.Brightness - 1;
		break;
		default:
			break;
	}
}

/**
 * \fn uint8_t RGB_Control_Get_Brightness(void)
 * \brief
 * \return
 */
uint8_t RGB_Control_Get_Brightness(void)
{
	return rgb_config.Brightness;
}

/**
 * \fn void RGB_Control_Set_Color(RGB_COLOR)
 * \brief
 * \param color
 */
void RGB_Control_Set_Color(RGB_COLOR color)
{
	if (rgb_config.state == RGB_OFF)
		return;

	rgb_config.Color = color;
	switch (color)
	{
		case RGB_COLOR_RED:
			htim3.Instance->CCR1 = rgb_config.Brightness; // Red
			htim3.Instance->CCR3 = 0; 					// Green
			htim3.Instance->CCR4 = 0;  					// Blue
		break;

		case RGB_COLOR_GREEN:
			htim3.Instance->CCR1 = 0; 						// Red
			htim3.Instance->CCR3 = rgb_config.Brightness; // Green
			htim3.Instance->CCR4 = 0;  						// Blue
		break;

		case RGB_COLOR_BLUE:
			htim3.Instance->CCR1 = 0; 						// Red
			htim3.Instance->CCR3 = 0; 						// Green
			htim3.Instance->CCR4 = rgb_config.Brightness;  // Blue
		break;
		case RGB_COLOR_WHITE:
			htim3.Instance->CCR1 = rgb_config.Brightness; // Red
			htim3.Instance->CCR3 = rgb_config.Brightness; // Green
			htim3.Instance->CCR4 = rgb_config.Brightness;  // Blue
		break;

		default:
			break;
	}
}

/**
 * \fn RGB_COLOR RGB_Control_Get_Color(void)
 * \brief
 * \return
 */
RGB_COLOR RGB_Control_Get_Color(void)
{
	return rgb_config.Color;
}

/**
 * \fn void RGB_Control_Set_Menu(RGB_MENU)
 * \brief
 * \param nmenu
 */
void RGB_Control_Set_Menu(RGB_MENU nmenu)
{
	rgb_config.menu = nmenu;
}

/**
 * \fn RGB_MENU RGB_Control_Get_Menu(void)
 * \brief
 * \return
 */
RGB_MENU RGB_Control_Get_Menu(void)
{
	return rgb_config.menu;
}
