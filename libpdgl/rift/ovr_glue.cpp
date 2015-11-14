#include <pdgl.h>
#include <pdglui.h>

// #define PDGLUI_HASRIFT

#ifdef PDGLUI_HASRIFT

#include "OculusSDK/LibOVR/Include/OVR.h"

using namespace OVR;

extern "C" {

char *pdgl_rift_monitorName;
float pdgl_rift_eyeDistance;
float pdgl_rift_distortionK[4];
float pdgl_rift_chromaAb[4];

SensorFusion *pdgl_rift_SFusion=NULL;

Ptr<DeviceManager> pManager;
Ptr<HMDDevice> pHMD;
HMDInfo hmd;
Ptr<SensorDevice> pSensor;

PDGLUI_API void PDGLUI_InitRift()
{
	static int init=0;

	if(init)return;
	init=1;

	System::Init(Log::ConfigureDefaultLog(LogMask_All));
	
	pManager = *DeviceManager::Create();
	pHMD = *pManager->EnumerateDevices<HMDDevice>().CreateDevice();

	if(!pHMD)
	{
		printf("PDGLUI_InitRift: No Device\n");
		pdgl_rift_SFusion=NULL;
		return;
	}

	if(pHMD->GetDeviceInfo(&hmd))
	{
		pdgl_rift_monitorName = dystrdup(hmd.DisplayDeviceName);
		pdgl_rift_eyeDistance = hmd.InterpupillaryDistance;
		pdgl_rift_distortionK[0] = hmd.DistortionK[0];
		pdgl_rift_distortionK[1] = hmd.DistortionK[1];
		pdgl_rift_distortionK[2] = hmd.DistortionK[2];
		pdgl_rift_distortionK[3] = hmd.DistortionK[3];

		pdgl_rift_chromaAb[0] = hmd.ChromaAbCorrection[0];
		pdgl_rift_chromaAb[1] = hmd.ChromaAbCorrection[1];
		pdgl_rift_chromaAb[2] = hmd.ChromaAbCorrection[2];
		pdgl_rift_chromaAb[3] = hmd.ChromaAbCorrection[3];
		
		printf("PDGLUI_InitRift: %s %f\n",
			pdgl_rift_monitorName, pdgl_rift_eyeDistance);
		printf("\tDistort=%f %f %f %f\n",
			pdgl_rift_distortionK[0],
			pdgl_rift_distortionK[1],
			pdgl_rift_distortionK[2],
			pdgl_rift_distortionK[3]);
		printf("\tChromaAb=%f %f %f %f\n",
			pdgl_rift_chromaAb[0],
			pdgl_rift_chromaAb[1],
			pdgl_rift_chromaAb[2],
			pdgl_rift_chromaAb[3]);
	}

	pSensor = *pHMD->GetSensor();

//	SensorFusion SFusion;
//	if (pSensor)
//		SFusion.AttachToSensor(pSensor);

	if (pSensor)
	{
		pdgl_rift_SFusion=new SensorFusion();
		pdgl_rift_SFusion->AttachToSensor(pSensor);
	}
}

PDGLUI_API void PDGLUI_GetRiftOrientation(float *ang)
{
	float yaw, pitch, roll;
	Quatf hmdOrient;
	
	PDGLUI_InitRift();
	
	if(!pdgl_rift_SFusion)
	{
		V3F_ZERO(ang);
		return;
	}
	
	hmdOrient=pdgl_rift_SFusion->GetOrientation();
//	hmdOrient.GetEulerABC<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);
	hmdOrient.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);

//	printf("PDGLUI_GetRiftOrientation: Q %f %f %f %f\n",
//		hmdOrient.x, hmdOrient.y, hmdOrient.z, hmdOrient.w);
	
//	printf("PDGLUI_GetRiftOrientation: A %f %f %f\n", yaw, pitch, roll);
	
	yaw=yaw*(180/M_PI);
	pitch=pitch*(180/M_PI);
	roll=-roll*(180/M_PI);
	
	ang[0]=pitch;
	ang[1]=roll;
	ang[2]=yaw;
}

PDGLUI_API void PDGLUI_GetRiftDeltaOrientation(float *ang)
{
	static float la[3];
	float ia[3];
	
	PDGLUI_GetRiftOrientation(ia);
	V3F_SUB(ia, la, ang);
	V3F_COPY(ia, la);
}

}	//extern "C"

#else //PDGLUI_HASRIFT

PDGLUI_API void PDGLUI_GetRiftOrientation(float *ang)
{
	V3F_ZERO(ang);
}

#endif	//PDGLUI_HASRIFT
