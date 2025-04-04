#include "stdio.h"
#include "ASICamera2.h"
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "pthread.h"


int  main()
{
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

	printf("\nselect one to privew\n");
	scanf("%d", &CamNum);

	if(ASIOpenCamera(CamNum) != ASI_SUCCESS)
	{
		printf("OpenCamera error,are you root?,press any key to exit\n");
		getchar();
		return -1;
	}
	ASIInitCamera(CamNum);

	ASI_ERROR_CODE err = ASIEnableDarkSubtract(CamNum, (char*)"dark.bmp");
	if(err == ASI_SUCCESS)
		printf("load dark ok\n");
	else
		printf("load dark failed %d\n", err);

	printf("%s information\n",ASICameraInfo.Name);
	int iMaxWidth, iMaxHeight;
	iMaxWidth = ASICameraInfo.MaxWidth;
	iMaxHeight =  ASICameraInfo.MaxHeight;
	printf("resolution:%dX%d\n", iMaxWidth, iMaxHeight);
	if(ASICameraInfo.IsColorCam)
		printf("Color Camera: bayer pattern:%s\n",bayer[ASICameraInfo.BayerPattern]);
	else
		printf("Mono camera\n");

	ASI_CONTROL_CAPS ControlCaps;
	int iNumOfCtrl = 0;
	ASIGetNumOfControls(CamNum, &iNumOfCtrl);
	for( i = 0; i < iNumOfCtrl; i++)
	{
		ASIGetControlCaps(CamNum, i, &ControlCaps);
		printf("%s\n", ControlCaps.Name);
	}



	long ltemp = 0;
	ASI_BOOL bAuto = ASI_FALSE;
	ASIGetControlValue(CamNum, ASI_TEMPERATURE, &ltemp, &bAuto);
	printf("sensor temperature:%02f\n", (float)ltemp/10.0);

	int bin = 1, Image_type;

	do{
		printf("\nPlease input the <width height bin image_type> with one space, ie. 640 480 2 0. use max resolution if input is 0. Press ESC when video window is focused to quit capture\n");

		scanf("%d %d %d %d", &width, &height, &bin, &Image_type);
		if(width == 0 || height == 0)
		{
			width = iMaxWidth;
			height = iMaxHeight;
		}

	}
	while(ASI_SUCCESS != ASISetROIFormat(CamNum, width, height, bin, (ASI_IMG_TYPE)Image_type));//IMG_RAW8

	printf("\nset image format %d %d %d %d success, start privew, press ESC to stop \n", width, height, bin, Image_type);

	ASIGetROIFormat(CamNum, &width, &height, &bin, (ASI_IMG_TYPE*)&Image_type);
	int displayWid = 1280, displayHei = 960;

	long imgSize = width*height*(1 + (Image_type==ASI_IMG_RAW16));
	long displaySize = displayWid*displayHei*(1 + (Image_type==ASI_IMG_RAW16));
	unsigned char* imgBuf = new unsigned char[imgSize];

	ASISetControlValue(CamNum, ASI_GAIN,70, ASI_FALSE);

	int exp_ms = 1000;
	// printf("Please input exposure time(ms)\n");
	// scanf("%d", &exp_ms);
	ASISetControlValue(CamNum, ASI_EXPOSURE, exp_ms*1000, ASI_FALSE);
	// ASISetControlValue(CamNum, ASI_BANDWIDTHOVERLOAD, 40, ASI_FALSE);

	ASIStartExposure(CamNum, ASI_FALSE);
	ASI_EXPOSURE_STATUS status;
	status = ASI_EXP_WORKING;
	while(status == ASI_EXP_WORKING)
	{
			ASIGetExpStatus(CamNum, &status);
	}
	if(status == ASI_EXP_SUCCESS)
	{
		printf("exposure ended \r\n");
		ASIGetDataAfterExp(CamNum, imgBuf, imgSize);
		printf("data acq \r\n");
		FILE* f = fopen("image.raw", "w");
		printf("f opend \r\n");
		fwrite(imgBuf, imgSize, imgSize, f);
		printf("f wr \r\n");
		fclose(f);
	}
	else{
		printf("exposure failed \r\n");
	}
	delete imgBuf;
	ASIStopExposure(CamNum);
	ASICloseCamera(CamNum);

	// time1 = GetTickCount();
	// int iStrLen = 0, iTextX = 40, iTextY = 60;
	// void* retval;


	// int iDropped = 0;
	//bool bSave = true;
// 	while(bMain)
// 	{

		
// 		ASIStartExposure(CamNum, ASI_FALSE);
// 		usleep(10000);//10ms
// 		status = ASI_EXP_WORKING;
// 		while(status == ASI_EXP_WORKING)
// 		{
// 			ASIGetExpStatus(CamNum, &status);		
// 		}
// 		if(status == ASI_EXP_SUCCESS)
// 		{
// 			ASIGetDataAfterExp(CamNum, imgBuf, imgSize);
// 			// sprintf(szTemp, "saveImage%d.jpg", bMain);
			
// 			if(Image_type==ASI_IMG_RAW16)
// 			{
// 				unsigned short *pCv16bit = (unsigned short *)(pRgb->imageData);
// 				unsigned short *pImg16bit = (unsigned short *)imgBuf;
// 				for(int y = 0; y < displayHei; y++)
// 				{
// 					memcpy(pCv16bit, pImg16bit, displayWid*2);
// 					pCv16bit+=displayWid;
// 					pImg16bit+=width;
// 				}
// 			}
// 			else{
// 				unsigned char *pCv8bit = (unsigned char *)pRgb->imageData;
// 				unsigned char *pImg8bit = (unsigned char *)imgBuf;
// 				for(int y = 0; y < displayHei; y++)
// 				{
// 					memcpy(pCv8bit, pImg8bit, displayWid);
// 					pCv8bit+=displayWid;
// 					pImg8bit+=width;
// 				}
// 			/*	if(bSave)
//    					cvSaveImage("saveImage.jpg", pRgb);
// 				bSave = false;*/
// 			}
					
// 		}
// //			ASIGetDataAfterExp(CamNum, (unsigned char*)pRgb->imageData, pRgb->imageSize);
	
// 		time2 = GetTickCount();
// 		count++;
		
// 		if(time2-time1 > 1000 )
// 		{
// 			ASIGetDroppedFrames(CamNum, &iDropped);			

// 			count = 0;
// 			time1=GetTickCount();	
// 			printf("fps:%d dropped frames:%d ImageType:%d\n",count, iDropped, Image_type);
	

// 		}
// 		if(Image_type != ASI_IMG_RGB24 && Image_type != ASI_IMG_RAW16)
// 		{
// 			iStrLen = strlen(buf);
// 			CvRect rect = cvRect(iTextX, iTextY - 15, iStrLen* 11, 20);
// 			cvSetImageROI(pRgb , rect);
// 			cvSet(pRgb, CV_RGB(180, 180, 180)); 
// 			cvResetImageROI(pRgb);
// 		}
// 		cvText(pRgb, buf, iTextX,iTextY );

// 		if(bChangeFormat)
// 		{
// 			bChangeFormat = 0;
// 			bDisplay = false;
// 			pthread_join(thread_display, &retval);
// 			cvReleaseImage(&pRgb);
			
// 			switch(change)
// 			{
// 				 case change_imagetype:
// 					Image_type++;
// 					if(Image_type > 3)
// 						Image_type = 0;
					
// 					break;
// 				case change_bin:
// 					if(bin == 1)
// 					{
// 						bin = 2;
// 						width/=2;
// 						height/=2;
// 					}
// 					else 
// 					{
// 						bin = 1;
// 						width*=2;
// 						height*=2;
// 					}
// 					break;
// 				case change_size_smaller:
// 					if(width > 320 && height > 240)
// 					{
// 						width/= 2;
// 						height/= 2;
// 					}
// 					break;
				
// 				case change_size_bigger:
				
// 					if(width*2*bin <= iMaxWidth && height*2*bin <= iMaxHeight)
// 					{
// 						width*= 2;
// 						height*= 2;
// 					}
// 					break;
// 			}
// 			ASISetROIFormat(CamNum, width, height, bin, (ASI_IMG_TYPE)Image_type);
// 			ASIGetROIFormat(CamNum, &width, &height, &bin, (ASI_IMG_TYPE*)&Image_type);
// 			if(Image_type == ASI_IMG_RAW16)
// 				pRgb=cvCreateImage(cvSize(width,height), IPL_DEPTH_16U, 1);
// 			else if(Image_type == ASI_IMG_RGB24)
// 				pRgb=cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 3);
// 			else
// 				pRgb=cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 1);
// 			bDisplay = 1;
// 			pthread_create(&thread_display, NULL, Display, (void*)pRgb);
// 		}
// 	}
// END:
	
// 	if(bDisplay)
// 	{
// 		bDisplay = 0;
// #ifdef _LIN
//    		pthread_join(thread_display, &retval);
// #elif defined _WINDOWS
// 		Sleep(50);
// #endif
// 	}
	
// 	ASIStopExposure(CamNum);
// 	ASICloseCamera(CamNum);
// 	cvReleaseImage(&pRgb);
// 	if(imgBuf)
// 		delete[] imgBuf;
// 	printf("main function over\n");
// 	return 1;
}






