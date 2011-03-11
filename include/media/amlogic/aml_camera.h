/*******************************************************************
 *
 *  Copyright C 2010 by Amlogic, Inc. All Rights Reserved.
 *
 *  Description:
 *
 *  Author: Amlogic Software
 *  Created: 2010/4/1   19:46
 *
 *******************************************************************/
#ifndef __AML_VIDEO_CAMERA_INCLUDE_9908049_
#define __AML_VIDEO_CAMERA_INCLUDE_9908049_
typedef void(*video_cam_init_fun_t)(void);
typedef void(*video_cam_uninit_fun_t)(void);  
typedef struct {
	char* name;
	int video_nr;    /* videoX start number, -1 is autodetect */
	video_cam_init_fun_t device_init;
	video_cam_uninit_fun_t device_uninit;
	unsigned pri_dat;
	
}aml_plat_cam_data_t;

 
#endif /* __AML_VIDEO_CAMERA_INCLUDE_9908049_ */