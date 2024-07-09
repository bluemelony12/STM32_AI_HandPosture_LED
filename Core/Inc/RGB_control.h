/*
 * RGB_control.h
 *
 *  Created on: Jul 5, 2024
 *      Author: LeeJH
 */

#ifndef INC_RGB_CONTROL_H_
#define INC_RGB_CONTROL_H_

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
	RGB_OFF = 0,
	RGB_ON = 1,
}RGB_STATE;

typedef enum
{
	RGB_COLOR_RED = 0,
	RGB_COLOR_GREEN = 1,
	RGB_COLOR_BLUE = 2,
	RGB_COLOR_WHITE = 3,
}RGB_COLOR;

typedef enum
{
	/* Default Menu : Hand Posture Control */
	RGB_MENU_NONE = 0,
	/* Setting Menu : Hand Gesture Control */
	RGB_MENU_COLOR_SET = 1,
	RGB_MENU_BRIGHTNESS_SET_Z = 2,
	RGB_MENU_BRIGHTNESS_SET_R = 3,
}RGB_MENU;

typedef struct
{
	RGB_STATE state;
	uint8_t Brightness;
	RGB_COLOR Color;
	RGB_MENU menu;
}RGB_CONFIG;

/* Exported functions ------------------------------------------------------- */
void RGB_Control_init(void);
void RGB_Control_Power_On(void);
void RGB_Control_Power_Off(void);
void RGB_Control_Set_Brightness(uint8_t val);
uint8_t RGB_Control_Get_Brightness(void);
void RGB_Control_Set_Color(RGB_COLOR color);
RGB_COLOR RGB_Control_Get_Color(void);
void RGB_Control_Set_Menu(RGB_MENU nmenu);
RGB_MENU RGB_Control_Get_Menu(void);


#endif /* INC_RGB_CONTROL_H_ */
