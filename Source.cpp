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
		cam.capture_photo(&_bytes,&len);
		FILE* fp = fopen("filename.yuv", "wb");
		fwrite(_bytes,1,len,fp);
		fclose(fp);
	}
	catch (const std::exception& e)
	{
		cout << e.what()<<"\n";
	}
	return 0;
}