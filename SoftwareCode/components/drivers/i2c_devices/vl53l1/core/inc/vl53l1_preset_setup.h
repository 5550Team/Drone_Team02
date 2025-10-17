#ifndef _VL53L1_PRESET_SETUP_H_
#define _VL53L1_PRESET_SETUP_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* indexes for the bare driver tuning setting API function */
enum VL53L1_Tuning_t {
	VL53L1_TUNING_VERSION = 0,
	VL53L1_TUNING_PROXY_MIN,
	VL53L1_TUNING_SINGLE_TARGET_XTALK_TARGET_DISTANCE_MM,
	VL53L1_TUNING_SINGLE_TARGET_XTALK_SAMPLE_NUMBER,
	VL53L1_TUNING_MIN_AMBIENT_DMAX_VALID,
	VL53L1_TUNING_MAX_SIMPLE_OFFSET_CALIBRATION_SAMPLE_NUMBER,
	VL53L1_TUNING_XTALK_FULL_ROI_TARGET_DISTANCE_MM,
	VL53L1_TUNING_SIMPLE_OFFSET_CALIBRATION_REPEAT,

	VL53L1_TUNING_MAX_TUNABLE_KEY
};

/* default values for the tuning settings parameters */
#define TUNING_VERSION	0x0004

#define TUNING_PROXY_MIN -30 /* min distance in mm */
#define TUNING_SINGLE_TARGET_XTALK_TARGET_DISTANCE_MM 600
/* Target distance in mm for single target Xtalk */
#define TUNING_SINGLE_TARGET_XTALK_SAMPLE_NUMBER 50
/* Number of sample used for single target Xtalk */
#define TUNING_MIN_AMBIENT_DMAX_VALID 8
/* Minimum ambient level to state the Dmax returned by the device is valid */
#define TUNING_MAX_SIMPLE_OFFSET_CALIBRATION_SAMPLE_NUMBER 50
/* Maximum loops to perform simple offset calibration */
#define TUNING_XTALK_FULL_ROI_TARGET_DISTANCE_MM 600
/* Target distance in mm for target Xtalk from Bins method*/
#define TUNING_SIMPLE_OFFSET_CALIBRATION_REPEAT 1
/* Number of loops done during the simple offset calibration*/

/* the following table should actually be defined as static and shall be part
 * of the VL53L1_StaticInit() function code
 */

#ifdef __cplusplus
}
#endif

#endif /* _VL53L1_PRESET_SETUP_H_ */
