#ifndef _VL53L1_API_DEBUG_H_
#define _VL53L1_API_DEBUG_H_

#include "vl53l1_platform.h"

#ifdef __cplusplus
extern "C" {
#endif


VL53L1_Error VL53L1_get_additional_data(
	VL53L1_DEV                Dev,
	VL53L1_additional_data_t *pdata);

/* End Patch_AdditionalDebugData_11823 */


#ifdef VL53L1_LOG_ENABLE

/**
 * @brief Implements an sprintf function for  signed fixed point numbers
 *
 * @param[in]   fp_value   : input signed fixed point number
 * @param[in]   frac_bits  : number of fixed point fractional bits
 * @param[in]   buf_size   : size of supplied text buffer
 * @param[out]  pbuffer    : pointer to text buffer
 *
 */

void  VL53L1_signed_fixed_point_sprintf(
	int32_t    fp_value,
	uint8_t    frac_bits,
	uint16_t   buf_size,
	char      *pbuffer);


/**
 * @brief   Convenience function to print out VL53L1_static_nvm_managed_t for debug
 *
 * @param[in]  pdata      : pointer to VL53L1_static_nvm_managed_t
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_static_nvm_managed(
	VL53L1_static_nvm_managed_t   *pdata,
	char                          *pprefix,
	uint32_t                       trace_flags);


/**
 * @brief   Convenience function to print out VL53L1_customer_nvm_managed_t for debug
 *
 * @param[in]  pdata       : pointer to VL53L1_customer_nvm_managed_t
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_customer_nvm_managed(
	VL53L1_customer_nvm_managed_t *pdata,
	char                          *pprefix,
	uint32_t                       trace_flags);


/**
 * @brief   Convenience function to print out VL53L1_nvm_copy_data_t for debug
 *
 * @param[in]  pdata    : pointer to VL53L1_nvm_copy_data_t
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_nvm_copy_data(
	VL53L1_nvm_copy_data_t        *pdata,
	char                          *pprefix,
	uint32_t                       trace_flags);


/**
 * @brief Convenience function to print out the contents of
 *        the Range Results structure for debug
 *
 * @param[in]  pdata       : pointer to a VL53L1_range_results_t structure
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_range_results(
	VL53L1_range_results_t *pdata,
	char                   *pprefix,
	uint32_t                trace_flags);

/**
 * @brief Convenience function to print out the contents of
 *        the  Range Data structure for debug
 *
 * @param[in]  pdata       : pointer to a VL53L1_range_data_t structure
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_range_data(
	VL53L1_range_data_t *pdata,
	char                *pprefix,
	uint32_t             trace_flags);

/**
 * @brief Convenience function to print out the contents of
 *        the offset range results structure for debug
 *
 * @param[in]  pdata       : pointer to a VL53L1_offset_range_results_t structure
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_offset_range_results(
	VL53L1_offset_range_results_t *pdata,
	char                          *pprefix,
	uint32_t                       trace_flags);

/**
 * @brief Convenience function to print out the contents of
 *        the offset range data structure for debug
 *
 * @param[in]  pdata       : pointer to a VL53L1_offset_range_data_t structure
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_offset_range_data(
	VL53L1_offset_range_data_t *pdata,
	char                       *pprefix,
	uint32_t                    trace_flags);


/**
 * @brief Convenience function to print out the contents of
 *        the peak rate map calibration data structure
 *
 * @param[in]  pdata       : pointer to a VL53L1_cal_peak_rate_map_t structure
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_cal_peak_rate_map(
	VL53L1_cal_peak_rate_map_t *pdata,
	char                       *pprefix,
	uint32_t                    trace_flags);


/**
 * @brief Convenience function to print out the contents of
 *        the additional offset calibration data structure
 *
 * @param[in]  pdata       : pointer to a VL53L1_additional_offset_cal_data_t structure
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_additional_offset_cal_data(
	VL53L1_additional_offset_cal_data_t *pdata,
	char                                *pprefix,
	uint32_t                             trace_flags);

/**
 * @brief Convenience function to print out the contents of
 *        the additional  data structure
 *
 * @param[in]  pdata       : pointer to a VL53L1_additional_data_t structure
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_additional_data(
	VL53L1_additional_data_t *pdata,
	char                     *pprefix,
	uint32_t                 trace_flags);


/**
 * @brief Convenience function to print out the contents of
 *        the LL driver gain calibration data structure
 *
 * @param[in]  pdata       : pointer to a VL53L1_gain_calibration_data_t structure
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_gain_calibration_data(
	VL53L1_gain_calibration_data_t *pdata,
	char                           *pprefix,
	uint32_t                        trace_flags);


/**
 * @brief Convenience function to print out the contents of
 *        the xtalk configuration data for debug
 *
 * @param[in]  pdata       : pointer to a VL53L1_xtalk_config_t Structure
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_xtalk_config(
	VL53L1_xtalk_config_t *pdata,
	char                  *pprefix,
	uint32_t               trace_flags);

/**
 * @brief Convenience function to print out the contents of
 *        the Optical Centre structure for debug
 *
 * @param[in]  pdata       : pointer to a VL53L1_optical_centre_t structure
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_optical_centre(
	VL53L1_optical_centre_t   *pdata,
	char                      *pprefix,
	uint32_t                   trace_flags);


/**
 * @brief Convenience function to print out the contents of
 *        the User Zone (ROI) structure for debug
 *
 * @param[in]  pdata       : pointer to a VL53L1_user_zone_t structure
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_user_zone(
	VL53L1_user_zone_t   *pdata,
	char                 *pprefix,
	uint32_t              trace_flags);

/**
 * @brief Convenience function for printing out VL53L1_spad_rate_data_t
 *
 * @param[in]  pspad_rates : pointer to VL53L1_spad_rate_data_t
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_spad_rate_data(
	VL53L1_spad_rate_data_t  *pspad_rates,
	char                     *pprefix,
	uint32_t                  trace_flags);


/**
 * @brief Convenience function for printing out VL53L1_spad_rate_map_t
 *
 * @param[in]  pspad_rates  : pointer to VL53L1_spad_rate_map_t
 * @param[in]  pprefix     : pointer to name prefix string
 * @param[in]  trace_flags : logging module enable bit flags
 */

void VL53L1_print_spad_rate_map(
	VL53L1_spad_rate_data_t  *pspad_rates,
	char                     *pprefix,
	uint32_t                  trace_flags);


#endif /* VL53L1_LOG_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* _VL53L1_API_DEBUG_H_ */
