#pragma comment(lib,"mfreadwrite.lib")
#pragma comment(lib,"evr.lib")
#pragma comment(lib,"mfplat.lib")
#pragma comment(lib,"mf.lib")
#pragma comment(lib,"mfuuid.lib")
#include <string>
#include <mfplay.h>
#include <evr.h>
#include <mfapi.h>
#include <assert.h>
#include <mfobjects.h>
#include <mferror.h>
#include <mfreadwrite.h>
#include <exception>
using namespace std;

typedef byte* bytes;

/// <summary>
/// Resolution data for camera
/// </summary>
struct Resolution
{
	int height, width;
};
//Simple camera handler
//Use try .. catch to see errors
class Camera
{
private:
	IMFSourceReader* source_reader;
	IMFMediaSource* currmediaSource;
protected:
	//returns device name
	wchar_t* next_camera_device(_In_ int _dev_index);
	virtual int prepare();
public:
	string name;
	Resolution* res_ptr;
	/*init default object of camera*/
	Camera();
	Camera(int);
	VOID capture_photo(bytes const*, int*);
};
int Camera::prepare()
{
	res_ptr = (Resolution*)malloc(sizeof(Resolution));
	IMFMediaType* media_type = nullptr;
	MFCreateSourceReaderFromMediaSource(currmediaSource, NULL, &source_reader);
	source_reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &media_type);
	MFGetAttributeSize(media_type, MF_MT_FRAME_SIZE, (uint32_t*)&res_ptr->width, (uint32_t*)&res_ptr->height);
	media_type->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);
	if (source_reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM,nullptr,media_type) != S_OK)
	{
		throw runtime_error("Format NV12 is not supported\nPlease override method <YourCamera>::prepare()");
		return 0;
	}

	return !media_type;
}
void Camera::capture_photo(_Out_ bytes const* buff, _Out_ int* len)
{
	IMFMediaBuffer* mediaBuffer;
	IMFSample* sample = nullptr;
	DWORD streamIndex, streamFlags = 0;
	long long timeStamp;
	while (!sample)
	{
		if (streamFlags == MF_SOURCE_READERF_ERROR)
		{
			throw runtime_error("Error ocurred when readsample is executing");
		}
		source_reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &streamIndex, &streamFlags, &timeStamp, &sample);
	}
	DWORD max_len = 0, cur_len;
	currmediaSource->Stop();
	sample->ConvertToContiguousBuffer(&mediaBuffer);
	mediaBuffer->Lock((BYTE**)buff, &max_len, &cur_len);
	*len = max_len;
	mediaBuffer->Release();
	mediaBuffer = nullptr;
	sample->Release();
	sample = nullptr;
}
wchar_t* Camera::next_camera_device(_In_ int _dev_index)
{
	wchar_t* devName = nullptr;
	UINT32 _count;
	IMFActivate** devs = nullptr;
	IMFAttributes* pAttributes = nullptr;
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);

	MFCreateAttributes(&pAttributes, 1);
	pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);

	HRESULT hrCode = MFEnumDeviceSources(pAttributes, &devs, &_count);
	if (hrCode != S_OK)
	{
		throw "Error in MFEnumDeviceSources";
	}


	UINT32 length;
	devs[_dev_index]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &devName, &length);
	devs[_dev_index]->ActivateObject(IID_IMFMediaSource, (void**)&currmediaSource);


	for (size_t i = 0; i < _count; i++)
	{
		devs[i]->Release();
		devs[i] = nullptr;
	}
	pAttributes->Release();

	return devName;
}
Camera::Camera() {
	wstring ws(next_camera_device(0));
	string str(ws.begin(), ws.end());
	name = str;
	prepare();
}
Camera::Camera(int _devIndex) {
	wstring ws(next_camera_device(_devIndex));
	string str(ws.begin(), ws.end());
	name = str;
	prepare();
}