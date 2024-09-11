#include <cstdlib>
#include "ASICamera2.h"
#include <stdio.h>

int main(){
    int width;
	const char* bayer[] = {"RG","BG","GR","GB"};

	int height;
	int i;
	char c;
	bool bresult;

	int time1,time2;
	int count=0;

	char buf[128]={0};

	int CamNum=0;

	int numDevices = ASIGetNumOfConnectedCameras();
	if(numDevices <= 0)
	{
		printf("no camera connected, press any key to exit\n");
		getchar();
		return -1;
	}
	else
		printf("attached cameras:\n");

	ASI_CAMERA_INFO ASICameraInfo;

	for(i = 0; i < numDevices; i++)
	{
		ASIGetCameraProperty(&ASICameraInfo, i);
		printf("%d %s\n",i, ASICameraInfo.Name);
	}
    return EXIT_SUCCESS;
}