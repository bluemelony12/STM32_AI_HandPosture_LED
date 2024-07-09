/*
 * gestures_example.h
 *
 *  Created on: Jul 3, 2024
 *      Author: LeeJH
 */

#ifndef INC_GESTURES_EXAMPLE_H_
#define INC_GESTURES_EXAMPLE_H_

/* Includes ------------------------------------------------------------------*/
#include "vl53lmz_api.h"
#include "gwmz.h"
#include "app_utils.h"
#include "main.h"

/* Exported types ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

int gesture_library_init_configure(void);
int SEN_CopyRangingData(SEN_data_t* pDest, VL53LMZ_ResultsData *pRangingData);
void Gesture_print_uart(void);
void Gesture_RGB_Control_Process(void);

#endif /* INC_GESTURES_EXAMPLE_H_ */
