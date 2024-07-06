/********************************************************************************
 *
 * Copyright (c) 2018 STMicroelectronics - All Rights Reserved
 *
 * License terms: STMicroelectronics Proprietary in accordance with licensing
 * terms at www.st.com/sla0081
 *
 * STMicroelectronics confidential
 * Reproduction and Communication of this document s strictly prohibited unless
 * specifically authorized in writing by STMicroelectronics.
 *
 *******************************************************************************/

#include "gestures_example.h"
#include "RGB_control.h"

/* VL53LMZ variables -----------------------------------------------------------------*/
int HTGDVerbose = HTGD_VERBOSE_NOLOG; //HTGD_VERBOSE_NOLOG - HTGD_VERBOSE_PROFILING - HTGD_VERBOSE_ERROR - HTGD_VERBOSE_INFO - HTGD_VERBOSE_DEBUG - In debug compilation mode, choose your debug level according htgd_platform.h

VL53LMZ_Configuration LMZDev;
VL53LMZ_ResultsData RangingData;

/* Gesture library variables -----------------------------------------------------------------*/
GW_proc_t gest_predictor;
HT_proc_t hand_tracker;
SEN_data_t sensor_data;

// Function to get the first target data
int* select_target_index(int *target_indices, int zone_index, VL53LMZ_ResultsData *pRangingData)
{
	(void) pRangingData;
	target_indices[0] = zone_index * VL53LMZ_NB_TARGET_PER_ZONE;
	return target_indices;
}

// Format data from LMZ driver to Gesture algorithm
int SEN_CopyRangingData(SEN_data_t *pDest, VL53LMZ_ResultsData *pRangingData)
{
	float ranging, peak;
	bool valid;
	int idx, result, nb_targets, target_idx[3], target_status;

	if (pDest == NULL || pRangingData == NULL)
		return -1;

	pDest->timestamp_ms = (int32_t) HAL_GetTick();

	for (idx = 0; idx < pDest->info.nb_zones; idx++)
	{
		select_target_index(target_idx, idx, pRangingData);

		ranging = pRangingData->distance_mm[target_idx[0]] / 4.0; // Signed 14.2

		peak = pRangingData->signal_per_spad[target_idx[0]] / 2048.0; // Unsigned 21.11
		nb_targets = pRangingData->nb_target_detected[idx];
		target_status = pRangingData->target_status[target_idx[0]];

		valid = (nb_targets > 0)
				&& (target_status == 5 || target_status == 6 || target_status == 9 || target_status == 10); //target_status == 4 -> wrapper on issue on L8
		result = SEN_set_data(pDest, idx, ranging, peak, valid);

		if (result != 0)
			return result;
	}

	return 0;
}

// Function used to print the algo results on the UART port and the data are formatted to be read in a terminal (Tera Term for example)
// The serial baudrate is 921600 by default in this example (see main.c)
void Gesture_print_uart(void)
{
	if (gest_predictor.gesture.ready)
	{
		if (gest_predictor.gesture.label == GW_LEFT)
		{
			printf("Gesture LEFT  <-  \r\n");
		}
		else if (gest_predictor.gesture.label == GW_RIGHT)
		{
			printf("Gesture RIGHT  ->  \r\n");
		}
		//-------------------------------
		else if (gest_predictor.gesture.label == GW_DOWN)
		{
			printf("Gesture DOWN  V  \r\n");
		}
		else if (gest_predictor.gesture.label == GW_UP)
		{
			printf("Gesture UP ^  \r\n");
		}
		else if (gest_predictor.gesture.label == GW_TOWARD)
		{
			printf("Gesture TAP [-]  \r\n");
		}
		//-------------------------------
		else if (gest_predictor.gesture.label == GW_DOUBLETAP)
		{
			printf("Gesture DOUBLE TAP  [--]  \r\n");
		}
		else if (gest_predictor.gesture.label == GW_AWAY)
		{
			printf("Gesture GW AWAY  \r\n");
		}
	}

	if (gest_predictor.lc_state == LC_UNDER_CONTROL)
	{
		printf("Level Control(LC_UNDER_CONTROL) : %3lu, Z = %6.1f, Polar: r = %6.1f, Theta = %4.1f\n",
				gest_predictor.gesture.lc_currentLevel,
				hand_tracker.hand.hand_z,
				gest_predictor.gesture.lc_polar_r,
				gest_predictor.gesture.lc_polar_theta);

		/*
		 if(gest_predictor.gesture.lc_currentLevel > 10)
		 {
		 //htim3.Instance->CCR1 = 50; // red
		 htim3.Instance->CCR3 = gest_predictor.gesture.lc_currentLevel - 1; // green
		 //htim3.Instance->CCR4 = 0;  // blue
		 }
		 */
	}
	else if (gest_predictor.lc_state == LC_NONE)
	{
		//printf("Level Control(LC_NONE)\r\n");
	}
	else if (gest_predictor.lc_state == LC_HAND_STABLE)
	{
		//printf("Level Control(LC_HAND_STABLE)\r\n");
	}
	else
	{

	}
}

void Gesture_RGB_Control_Process(void)
{
	RGB_COLOR pcolor = 0;
	float theta_to_val = 0;

	switch (RGB_Control_Get_Menu())
	{
		case RGB_MENU_COLOR_SET:
			if (gest_predictor.gesture.ready && gest_predictor.gesture.label == GW_LEFT)
			{
				pcolor = RGB_Control_Get_Color();
				if (pcolor == RGB_COLOR_RED)
					pcolor = RGB_COLOR_WHITE;
				else pcolor--;

				printf("Change RGB LED Color : %d\r\n", pcolor);

				RGB_Control_Set_Color(pcolor);
			}
			else if (gest_predictor.gesture.ready && gest_predictor.gesture.label == GW_RIGHT)
			{
				pcolor = RGB_Control_Get_Color();
				if (pcolor == RGB_COLOR_WHITE)
					pcolor = RGB_COLOR_RED;
				else pcolor++;

				printf("Change RGB LED Color : %d\r\n", pcolor);

				RGB_Control_Set_Color(pcolor);
			}
			else
			{
			}
		break;

		case RGB_MENU_BRIGHTNESS_SET_Z:
			if (gest_predictor.lc_state == LC_UNDER_CONTROL)
			{
				if (gest_predictor.gesture.lc_currentLevel > 10)
				{
					printf("Change RGB LED Brightness Z-axis : %d\r\n", (uint8_t) gest_predictor.gesture.lc_currentLevel);
					RGB_Control_Set_Brightness((uint8_t) gest_predictor.gesture.lc_currentLevel);
				}
			}
		break;

		case RGB_MENU_BRIGHTNESS_SET_R:
			if (gest_predictor.lc_state == LC_UNDER_CONTROL)
			{
				theta_to_val = (gest_predictor.gesture.lc_polar_theta * 100.0) / 360.0;

				printf("Change RGB LED Brightness Rotate : %d\r\n", (uint8_t) theta_to_val);
				RGB_Control_Set_Brightness((uint8_t) theta_to_val);
			}
		break;

		default:
			break;
	}

	if (gest_predictor.gesture.ready)
	{
		if(gest_predictor.gesture.label == GW_UP)
		{
			if (RGB_Control_Get_Menu() != RGB_MENU_NONE)
			{
				printf("Back to Main Menu...\r\n");
				RGB_Control_Set_Menu(RGB_MENU_NONE);
			}
		}
	}
}

// Recommended Initialization to use the Gesture library
int gesture_library_init_configure(void)
{
	int status = 0;

	//----------------//
	// MANDATORY CODE //
	//----------------//
	//Initialize Gesture algo and use the default parameters
	status = GW_init(&gest_predictor, &hand_tracker, &sensor_data);
	if (status != 0)
	{
		return status;
	}
	status = HT_set_square_resolution(&hand_tracker, &sensor_data, App_Config.Params.Resolution);
	if (status != 0)
	{
		return status;
	}

	status = GW_set_frequency(&gest_predictor, &hand_tracker, &sensor_data, 1000.0 / App_Config.Params.RangingPeriod);
	if (status != 0)
	{
		return status;
	}

	/**
	 ///---------------//
	 // OPTIONAL CODE //
	 //---------------//
	 // TO CHANGE THE ALGO PARAMETERS //
	 // !!! Gesture recognition performances are not guaranteed if default parameter values are changed !!!///
	 status = GW_set_params(&gest_predictor,
	 Params.gesture_selection,
	 Params.analysis_ms,
	 Params.dead_ms,
	 Params.screening_ms,
	 Params.closer_mm,
	 Params.min_speed_x_mm_s,
	 Params.min_speed_y_mm_s,
	 Params.min_speed_z_mm_s,
	 Params.max_speed_mm_s,
	 Params.ranging_ignore_dmax_mm,
	 Params.min_vx_vy,
	 Params.min_vx_vz,
	 Params.min_vy_vx,
	 Params.min_vy_vz,
	 Params.min_vz_vx,
	 Params.min_vz_vy,
	 -1, // This parameter can't be changed
	 Params.min_user_filtering_mm,
	 Params.max_user_filtering_mm,
	 Params.filtering_area_mm,
	 Params.lc_stable_threshold,
	 Params.lc_stable_time_threshold,
	 Params.lc_maxDistance_mm,
	 Params.lc_minDistance_mm,
	 Params.double_tap_ts_threshold);
	 if (status != 0)
	 {
	 MZ_TRACE("GW_set_params failed : %d\n",status);
	 return status;
	 }
	 // Set Frequency has to be run again after changing the Gesture params
	 status = GW_set_frequency(&gest_predictor, &hand_tracker, &sensor_data, 1000.0/Params.RangingPeriod);
	 if (status != 0)
	 {
	 MZ_TRACE("GW_set_frequency failed : %d\n",status);
	 return status;
	 }
	 */

	status = SEN_set_orientation(&sensor_data, App_Config.Params.SensorOrientation);
	if (status != 0)
	{
		return status;
	}

	return status;
}
