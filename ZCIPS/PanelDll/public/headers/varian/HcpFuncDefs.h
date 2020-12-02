
//////////////////////////////////////////////////////////////////////////////
// Filename:	hcpfuncdefs.h
// Description:	This is the main interface definition file 	
// Copyright:	Varian Medical Systems
//				All Rights Reserved
//				Varian Proprietary   
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// This file is the basis for all interface function defines and typedefs for
// function pointers. If used without any prior #defines it defaults to 
// customer requirements allowing dll import. When using the file to define
// its own exports MC module defines HCPMC_EXPORTS. When used in these ways,
// the function names in the macro parameters are decorated with a 'vip_'
// prefix. More specialized uses '#define HCP_DLL_BUILD'. 
//////////////////////////////////////////////////////////////////////////////

#ifndef  VIP_HCP_FUNCDEFS_H
#define  VIP_HCP_FUNCDEFS_H

#ifndef HCP_DLL_BUILD

#ifdef  HCPMC_EXPORTS 
#define HCP_LINKAGE __declspec(dllexport)
#else
#define HCP_LINKAGE __declspec(dllimport)
#endif // HCPMC_EXPORTS 

#define HCP_PRM
#define HCP_COMPRM
#ifdef __cplusplus
#define HCP_DEFPRM(X)   =X
#else
#define HCP_DEFPRM(X)
#endif

#define HCP_PRE		HCP_LINKAGE int

#define HCP_DEC_FUNC(FuncName)  HCP_PRE vip_##FuncName

#endif // HCP_DLL_BUILD

#include "FluoroStructs.h"
#include "HcpSundries.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(analog_offset_cal) (int mdNum HCP_COMPRM);
// equivalent to 
// HCP_LINKAGE int vip_analog_offset_cal(int mdNum);
// with HCP_DLL_BUILD not defined  (for MC import/export)

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(cal_control)  (SCalCtrl* cc HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(cal_end)  (HCP_PRM); 

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(check_link) (SCheckLink* cl HCP_DEFPRM(NULL) HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(close_link)  (int recNum HCP_DEFPRM(0) HCP_COMPRM);
//  5

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(correct_image)  (SCorrectImage* corrImg HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(dcds_enable)  (BOOL enable HCP_COMPRM); 

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(enable_auto_cal)  (int mdNum, BOOL enable, int minimum_delay,
									int post_exposure_delay HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(enable_sw_handshaking) (BOOL enable HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(fluoro_dispose)  (HCP_PRM);
//10

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(fluoro_get_buffer_ptr)  (WORD** buf, int bufIdx,
									int bufType HCP_DEFPRM(0) HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(fluoro_get_event_name)  (int eventType,
									char* eventName HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(fluoro_get_prms)  (int structType, 
									void* structPtr HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(fluoro_grabber_start)  (SAcqPrms* acqPrms HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(fluoro_grabber_stop)  (HCP_PRM);
// 15

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(fluoro_init_mode)  (SFluoroModePrms* modePrms HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(fluoro_init_sys)  (SFluoroSysPrms* sysPrms HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(fluoro_record_start)  (int stopAfterN HCP_DEFPRM(0),
									int startFromBufIdx HCP_DEFPRM(-1) HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(fluoro_record_stop)  (HCP_PRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(fluoro_set_prms)  (int structType, 
									void* structPtr HCP_COMPRM);
// 20

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(gain_cal_prepare)  (int mode, int calType HCP_DEFPRM(0) HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_analog_offset_info)  (int mode, 
									SAnalogOffsetInfo* aoi HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_analog_offset_params)  (int mode, 
									SAnalogOffsetParams* aop HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_auto_cal_settings)  (int mode, BOOL *enable,
									int *minimum_delay,
									int *post_exposure_delay HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_cal_info)  (int mode, SCalInfo* calInfo HCP_COMPRM);
// 25

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_cal_limits)  (int mode, SCalLimits* calLim  HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_config_data)  (char *full_file_path,
									char *target_file_name HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_correction_settings)  (SCorrections* corr HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_current_mode)  (int* mode HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_dll_version)  (char* version, char* dllName,
									int size HCP_COMPRM);
// 30

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_gain_scaling_info)  (int mode, SGainScaling* gs HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_hw_config)  (SHwConfig* hwCnfg HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_image)  (int mdNum, int image_type,
                                    int x_size, int y_size, 
									WORD *image_ptr HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_image_counts)  (int* imgRead, int* numImg HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_lih)  (int mdNum, BOOL *lih_active HCP_COMPRM);
// 35

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_mode_acq_type)  (int mdNum, int *mode_acq_type,
									int *num_frames HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_mode_info)  (int mdNum, SModeInfo* mdInfo HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_num_acq_frames)  (int mdNum, 
									int *num_acq_frames HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_num_cal_frames)  (int mdNum, 
									int *num_cal_frames HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_offset_cal_shift)  (int mdNum, 
									int *offset_cal_shift HCP_COMPRM);
// 40

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_rad_scaling)  (int mdNum, int *scaling_type,
									int *target_value HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_recursive_filter)  ( int mdNum, 
									double *buffer_weight HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_self_test_log)  (HCP_PRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_sys_info)  (SSysInfo* sysInfo HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_sys_mode)  (SSysMode* sysMode HCP_COMPRM);
// 45

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_system_version_numbers)  ( int sys_ver_type,
									char *ver_str HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_video_timing)  (int mdNum, int *h_active, 
									int *v_active, int *h_sync,
									int *v_sync, int *h_f_porch,
									int *v_f_porch, int *h_b_porch,
									int *v_b_porch HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_vista_parameters)  (int mdNum, int *alpha_high, 
									int *alpha_low, int *alpha_threshold,
									int *saturation_threshold,
									int *pixel_count,
									int *vista_capable HCP_DEFPRM(NULL) HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(get_wl)  (int *bot, int *top, int *mapping HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(hw_reset)  (HCP_PRM);
// 50

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(initialize_media)  (char* drvLet, char* errMsg HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(io_enable)  (int activeMode HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(io_permit_exposure)  (HCP_PRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(io_query_status)  (int *statusValue, 
									int *exposureValue HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(offset_cal)  (int mdNum HCP_COMPRM);
// 55

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(open_receptor_link)  (SOpenReceptorLink* orl HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(put_config_data)  (char *full_file_path,
									char *target_file_name HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(put_image)  (int mdNum, int image_type, int x_size,
									int y_size, WORD *image_ptr HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(query_error_info)  (SQueryErrorInfo* qei HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(query_prog_info)  (int uType, UQueryProgInfo* uq HCP_COMPRM);
// 60

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(reset_state)  (HCP_PRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(select_mode)  (int mdNum HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(select_receptor)  (int recSel HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(self_test)  (HCP_PRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_analog_offset_params)  (int mdNum, 
									SAnalogOffsetParams* aop HCP_COMPRM);
// 65

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_cal_acq_data)  (int mdNum, SCalAcqData* cad HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_cal_limits)  (int mdNum, SCalLimits* calLim  HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_correction_settings)  (SCorrections* corr HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_debug)  (int enable HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_frame_rate)  (int mdNum,
									double frame_rate HCP_COMPRM);
//70

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_gain_scaling_info)  (int mdNum, SGainScaling* gs HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_hw_config)  (SHwConfig* hwCnfg HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_image_counts)  (int imgRead, int numImg HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_lih)  (int mdNum, BOOL lih_active HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_mode_acq_type)  (int mdNum, int acq_type,
									int num_frames HCP_COMPRM);
// 75

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_num_acq_frames)  (int mdNum, 
									int num_acq_frames HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_num_cal_frames)  (int mdNum,
									int num_cal_frames HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_offset_cal_shift)  (int mdNum,
									int offset_cal_shift HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_rad_scaling)  (int mdNum, int scaling_type,
									int target_value HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_recursive_filter)  (int mdNum, 
									double buffer_weight HCP_COMPRM);
// 80

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_sys_mode)  (SSysMode* sysMode HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_user_sync)  (int mdNum, BOOL user_sync HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_vista_parameters)  (int mdNum, int alpha_high,
									int alpha_low, int alpha_threshold,
									int saturation_threshold,
									int pixel_count HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(set_wl)  (int bot, int top, int mapping HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(signal_frame_start)  (HCP_PRM);
// 85

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(sw_handshaking)  (int signal_type, BOOL active HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(total_reset_media)  (char* drvLet, char* errMsg HCP_COMPRM);

//////////////////////////////////////////////////////////////////////////////
HCP_DEC_FUNC(validate_media)  ( char* drvLet, char* recptr, int*  imgX,
									int*  imgY, char* errMsg HCP_COMPRM);

#ifdef __cplusplus
}
#endif


#endif //  VIP_HCP_FUNCDEFS_H

#ifdef HCP_DLL_BUILD
#undef HCP_DLL_BUILD
#endif //  HCP_DLL_BUILD

#undef HCP_PRE
#undef HCP_DEC_FUNC
#undef HCP_PRM
#undef HCP_COMPRM
#undef HCP_DEFPRM
