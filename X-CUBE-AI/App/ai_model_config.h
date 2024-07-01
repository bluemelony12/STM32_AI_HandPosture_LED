/**
******************************************************************************
"  * @file    ai_model_config.h\n"
"  * @author  ST Imaging Division and Artificial Intelligence Solutions group (AIS)
"  * @brief   User header file for Preprocessing configuration\n"
"  **************************************************************************
"  * @attention
* Copyright (c) 2022 STMicroelectronics.
* All rights reserved.
* This software is licensed under terms that can be found in the LICENSE file in
the root directory of this software component.\n"
If no LICENSE file comes with this software, it is provided AS-IS.\n"
******************************************************************************
**/

/* ---------------    Generated code    ----------------- */

#ifndef __AI_MODEL_CONFIG_H__
#define __AI_MODEL_CONFIG_H__

/* I/O configuration */
#define NB_CLASSES        8

#define INPUT_HEIGHT      8
#define INPUT_WIDTH       8
#define INPUT_CHANNELS    2

/* Classes */
#define CLASSES_TABLE 	const char* classes_table[NB_CLASSES] = {"None", "FlatHand", "Like", "Love", "Dislike", "BreakTime", "CrossHands", "Fist"};
#define EVK_LABEL_TABLE const int evk_label_table[NB_CLASSES] = {0, 20, 21, 24, 25, 27, 28, 32};

/* Pre_processing */
#define BACKGROUND_REMOVAL 120
#define MAX_DISTANCE 350
#define MIN_DISTANCE 150

#endif      /* __AI_MODEL_CONFIG_H__  */
