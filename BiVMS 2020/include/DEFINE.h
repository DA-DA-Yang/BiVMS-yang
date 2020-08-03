#pragma once
#define PI                   3.1415926
//通信宏
#define SIZE_MAXBYTEWRITE    50000
#define	MES_OPEN_WIDGET	     "OPEN"
#define	MES_EXIT	         "CLOSE"
#define MES_NEW_PROJECT	     "NEW_PROJECT"
#define MES_LOAD_PROJECT	 "LOAD_PROJECT"
#define MES_OPEN_CAMERA	     "OPEN_CAMERA"
#define MES_CLOSE_CAMERA	 "CLOSE_CAMERA"
#define MES_SNAP_IMAGE	     "SNAP_IMAGE"
#define MES_CAPTURE_IMAGES	 "CAPTURE_IMAGES"
#define MES_COMPLETED        "COMPLETED"

//POI结构体
struct POI
{
	int ID{};
	double x{};
	double y{};
};


