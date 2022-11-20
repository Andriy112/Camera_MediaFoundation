#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "camera.h"
using namespace std;




int main() {
	int len;
	GUID type;
	bytes _bytes;
	Camera cam;
	try
	{
		
		cam.capture_photo(&_bytes,&type,&len);
		if (&type == &MFVideoFormat_NV12)
		{
			ofstream fs;
			fs.open("_out_.yuv",ofstream::binary);
			fs.write((char*)_bytes, len);
			fs.close();
		}		
	}
	catch (const std::exception& e)
	{
		printf("%s",e.what());
	}
	return 0;
}