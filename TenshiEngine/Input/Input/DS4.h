#pragma once

#include <windows.h>
#include <setupapi.h>
#include <hidsdi.h>
#pragma comment(lib, "hid.lib")
#pragma comment(lib, "setupapi.lib")

class DS4{
public:
	DS4()
		: m_HidHandle(NULL)
		, m_EventHandle(NULL)
		, m_ReceiveBuffer(NULL)
	{
		memset(&m_Overlapped, 0, sizeof(OVERLAPPED));
		for (int i = 0; i < (int)PAD_DS4_KeyCoord::Count; i++){
			m_DS4State[i] = 0;
		}
		for (int i = 0; i < (int)PAD_DS4_LevelCoord::Count; i++){
			m_DS4Analog[i] = 0;
		}
	}
	~DS4()
	{
	}
	void Initialize_And_FindHID(USHORT VendorID = 0x054C, USHORT ProductID = 0x05C4){
		//
		//　　　HIDclass識別子取得
		//
		GUID hidGuid;
		HidD_GetHidGuid(&hidGuid);
		//
		//      HIDデバイス情報セット取得
		//
		HDEVINFO devinf;
		devinf = SetupDiGetClassDevs(&hidGuid, NULL, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
		//
		//　　　個別デバイス情報の構造体の取得
		//
		SP_DEVICE_INTERFACE_DATA spid;
		spid.cbSize = sizeof(spid);
		//
		for (int index = 0;; index++)
		{

			if (!SetupDiEnumDeviceInterfaces(devinf, NULL, &hidGuid, index, &spid))
			{
				break;
			}

			//
			//      デバイスインターフェイスの詳細情報の取得
			//
			unsigned long size;
			SetupDiGetDeviceInterfaceDetail(devinf, &spid, NULL, 0, &size, 0);
			PSP_INTERFACE_DEVICE_DETAIL_DATA dev_det = PSP_INTERFACE_DEVICE_DETAIL_DATA(new char[size]);
			dev_det->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
			//
			SetupDiGetDeviceInterfaceDetail(devinf, &spid, dev_det, size, &size, 0);

			//
			//      ファイルハンドルの取得
			//
			HANDLE handle = CreateFile(dev_det->DevicePath, GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0/*FILE_FLAG_OVERLAPPED*/, NULL);

			delete dev_det;

			if (handle == INVALID_HANDLE_VALUE)
			{
				continue;
			}
			//
			//      VenderIDとProductIDの取得
			//
			HIDD_ATTRIBUTES attr;
			HidD_GetAttributes(handle, &attr);

			if ((int)attr.VendorID == VendorID &&
				(int)attr.ProductID == ProductID){

				Initialize(handle);
				return;

			}
			else{
				CloseHandle(handle);
			}
		}
	}
	void Initialize(HANDLE handle){
		m_HidHandle = handle;


		if (!m_HidHandle)return;
		if (m_EventHandle)return;

		m_EventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_Overlapped.hEvent = m_EventHandle;


		PHIDP_PREPARSED_DATA lpData;
		// Windowsが管理するバッファーへ、デバイスの情報を読み込み取得する。
		if (HidD_GetPreparsedData(m_HidHandle, &lpData))
		{
			HIDP_CAPS objCaps;


			// デバイスの情報を取得
			HidP_GetCaps(lpData, &objCaps);
			// デバイスがサポートするReportの読出し、書き出しのバッファーサイズを取得します。
			// USBデバイスでは、このサイズが固定です。
			m_InputReportLength = objCaps.InputReportByteLength;
			m_OutputReportLength = objCaps.OutputReportByteLength;
			m_ReceiveBuffer = new byte[m_InputReportLength];
			memset(m_ReceiveBuffer, 0, sizeof(byte)*m_InputReportLength);
			// HidD_GetPreparsedData() で取得したデータ領域を開放します。
			HidD_FreePreparsedData(lpData);
		}
	}

	void Release(){
		if (m_HidHandle)CloseHandle(m_HidHandle);
		if (m_EventHandle)CloseHandle(m_EventHandle);
		if (m_ReceiveBuffer)delete[] m_ReceiveBuffer;
	}

	//入力を更新します
	void Read(){
		if (!m_HidHandle)return;

		DWORD BytesRead = 0;
		ReadFile(m_HidHandle, m_ReceiveBuffer, m_InputReportLength, &BytesRead, NULL);

		_ReadInput(m_DS4State);
		_ReadAnalog(m_DS4Analog);
	}

	bool Down(PAD_DS4_KeyCoord keyCoord){
		return m_DS4State[(int)keyCoord] != 0;
	}
	float Level(PAD_DS4_LevelCoord levelCoord){
		return m_DS4Analog[(int)levelCoord];
	}

	bool IsConnecting(){
		return m_HidHandle != NULL;
	}

	//-1.0 ~ 1.0
	//MaxLength 1.0
	void ReadLStick(float *x, float *y){
		*x =
			(m_ReceiveBuffer[(int)DS4_ArrayFlag::LAnalogX] / 255.0f) * 2.0f - 1.0f;
		*y =
			(m_ReceiveBuffer[(int)DS4_ArrayFlag::LAnalogY] / 255.0f) * -2.0f + 1.0f;
	}
	//-1.0 ~ 1.0
	//MaxLength 1.0
	void ReadRStick(float *x, float *y){
		*x =
			(m_ReceiveBuffer[(int)DS4_ArrayFlag::RAnalogX] / 255.0f) * 2.0f - 1.0f;
		*y =
			(m_ReceiveBuffer[(int)DS4_ArrayFlag::RAnalogY] / 255.0f) * -2.0f + 1.0f;
	}
	//押し込み 0.0 ~ 1.0
	//255段階
	void ReadL2Level(float *l){
		*l =
			(m_ReceiveBuffer[(int)DS4_ArrayFlag::L2_Level] / 255.0f);
	}
	//押し込み 0.0 ~ 1.0
	//255段階
	void ReadR2Level(float *r){
		*r =
			(m_ReceiveBuffer[(int)DS4_ArrayFlag::R2_Level] / 255.0f);
	}
	//左上(0.0,0.0) ~ 右下(1.0,1.0)
	//解像度:1920x943
	void Read1stTouch(float *x, float *y){
		*x =
			((m_ReceiveBuffer[(int)DS4_ArrayFlag::Touch_1stX_LOW]) |
			(((m_ReceiveBuffer[(int)DS4_ArrayFlag::Touch_1stX_HIGH] & 0x0f)) << 8)
			) / m_TouchPadPixelX;
		*y = 1.0f -
			(
			((m_ReceiveBuffer[(int)DS4_ArrayFlag::Touch_1stY_LOW] & 0xf0) >> 4) |
			((m_ReceiveBuffer[(int)DS4_ArrayFlag::Touch_1stY_HIGH]) << 4)
			) / m_TouchPadPixelY;
	}

	//左上(0.0,0.0) ~ 右下(1.0,1.0)
	//解像度:1920x943
	void Read2ndTouch(float *x, float *y){
		*x =
			((m_ReceiveBuffer[(int)DS4_ArrayFlag::Touch_2ndX_LOW]) |
			(((m_ReceiveBuffer[(int)DS4_ArrayFlag::Touch_2ndX_HIGH] & 0x0f)) << 8)
			) / m_TouchPadPixelX;
		*y = 1.0f -
			(
			((m_ReceiveBuffer[(int)DS4_ArrayFlag::Touch_2ndY_LOW] & 0xf0) >> 4) |
			((m_ReceiveBuffer[(int)DS4_ArrayFlag::Touch_2ndY_HIGH]) << 4)
			) / m_TouchPadPixelY;
	}

	//加速度 -1.0 ~ 1.0
	void ReadAxisAngular(float *x, float *y, float *z){
		*x =
			((short)((m_ReceiveBuffer[(int)DS4_ArrayFlag::XAxisAngularLOW]) |
			((m_ReceiveBuffer[(int)DS4_ArrayFlag::XAxisAngularHIGH]) << 8))
			) / ((float)0x7fff);
		*y =
			((short)((m_ReceiveBuffer[(int)DS4_ArrayFlag::YAxisAngularLOW]) |
			((m_ReceiveBuffer[(int)DS4_ArrayFlag::YAxisAngularHIGH]) << 8))
			) / ((float)0x7fff);
		*z =
			((short)((m_ReceiveBuffer[(int)DS4_ArrayFlag::ZAxisAngularLOW]) |
			((m_ReceiveBuffer[(int)DS4_ArrayFlag::ZAxisAngularHIGH]) << 8))
			) / ((float)0x7fff);
	}
	//ジャイロ -1.0 ~ 1.0
	void ReadAxisAcceleration(float *x, float *y, float *z){
		*x =
			((short)((m_ReceiveBuffer[(int)DS4_ArrayFlag::XAxisAccelerationLOW]) |
			((m_ReceiveBuffer[(int)DS4_ArrayFlag::XAxisAccelerationHIGH]) << 8))
			) / ((float)0x7fff);

		*y =
			((short)((m_ReceiveBuffer[(int)DS4_ArrayFlag::YAxisAccelerationLOW]) |
			((m_ReceiveBuffer[(int)DS4_ArrayFlag::YAxisAccelerationHIGH]) << 8))
			) / ((float)0x7fff);
		*z =
			((short)((m_ReceiveBuffer[(int)DS4_ArrayFlag::ZAxisAccelerationLOW]) |
			((m_ReceiveBuffer[(int)DS4_ArrayFlag::ZAxisAccelerationHIGH]) << 8))
			) / ((float)0x7fff);
	}

	const float m_TouchPadPixelX = 1920.0f;
	const float m_TouchPadPixelY = 943.0f;

private:

	void _ReadInput(BYTE* buttons){

		byte pov = m_ReceiveBuffer[(int)DS4_ArrayFlag::POV] & 0x0f;
		buttons[(int)PAD_DS4_KeyCoord::Button_UP] =
			(pov == DS4_BitFlag::Button_UP ||
			pov == DS4_BitFlag::Button_UP_RIGHT ||
			pov == DS4_BitFlag::Button_LEFT_UP
			);
		buttons[(int)PAD_DS4_KeyCoord::Button_RIGHT] =
			(pov == DS4_BitFlag::Button_RIGHT ||
			pov == DS4_BitFlag::Button_UP_RIGHT ||
			pov == DS4_BitFlag::Button_RIGHT_DOWN
			);
		buttons[(int)PAD_DS4_KeyCoord::Button_DOWN] =
			(pov == DS4_BitFlag::Button_DOWN ||
			pov == DS4_BitFlag::Button_RIGHT_DOWN ||
			pov == DS4_BitFlag::Button_DOWN_LEFT
			);
		buttons[(int)PAD_DS4_KeyCoord::Button_LEFT] =
			(pov == DS4_BitFlag::Button_LEFT ||
			pov == DS4_BitFlag::Button_DOWN_LEFT ||
			pov == DS4_BitFlag::Button_LEFT_UP
			);

		buttons[(int)PAD_DS4_KeyCoord::Button_SQUARE] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button] & DS4_BitFlag::Button_SQUARE;
		buttons[(int)PAD_DS4_KeyCoord::Button_CROSS] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button] & DS4_BitFlag::Button_CROSS;
		buttons[(int)PAD_DS4_KeyCoord::Button_CIRCLE] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button] & DS4_BitFlag::Button_CIRCLE;
		buttons[(int)PAD_DS4_KeyCoord::Button_TRIANGLE] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button] & DS4_BitFlag::Button_TRIANGLE;

		buttons[(int)PAD_DS4_KeyCoord::Button_L1] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button2] & DS4_BitFlag::Button_L1;
		buttons[(int)PAD_DS4_KeyCoord::Button_L2] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button2] & DS4_BitFlag::Button_L2;
		buttons[(int)PAD_DS4_KeyCoord::Button_L3] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button2] & DS4_BitFlag::Button_L3;

		buttons[(int)PAD_DS4_KeyCoord::Button_R1] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button2] & DS4_BitFlag::Button_R1;
		buttons[(int)PAD_DS4_KeyCoord::Button_R2] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button2] & DS4_BitFlag::Button_R2;
		buttons[(int)PAD_DS4_KeyCoord::Button_R3] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button2] & DS4_BitFlag::Button_R3;

		buttons[(int)PAD_DS4_KeyCoord::Button_OPTIONS] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button2] & DS4_BitFlag::Button_OPTIONS;
		buttons[(int)PAD_DS4_KeyCoord::Button_SHARE] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button2] & DS4_BitFlag::Button_SHARE;


		buttons[(int)PAD_DS4_KeyCoord::Button_1stTOUCH] =
			(m_ReceiveBuffer[(int)DS4_ArrayFlag::Touch_1st] & DS4_BitFlag::Button_1stTOUCH) == 0 ? 1 : 0;
		buttons[(int)PAD_DS4_KeyCoord::Button_2ndTOUCH] =
			(m_ReceiveBuffer[(int)DS4_ArrayFlag::Touch_2nd] & DS4_BitFlag::Button_2ndTOUCH) == 0 ? 1 : 0;

		buttons[(int)PAD_DS4_KeyCoord::Button_PS] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button3] & DS4_BitFlag::Button_PS;

		buttons[(int)PAD_DS4_KeyCoord::Button_TOUCHPAD] =
			m_ReceiveBuffer[(int)DS4_ArrayFlag::Button3] & DS4_BitFlag::Button_TOUCHPAD;
	}

	void _ReadAnalog(float* analog){

		ReadL2Level(&analog[0]);
		ReadR2Level(&analog[1]);
		ReadLStick(&analog[2], &analog[3]);
		ReadRStick(&analog[4], &analog[5]);
		Read1stTouch(&analog[6], &analog[7]);
		Read2ndTouch(&analog[8], &analog[9]);
		ReadAxisAngular(&analog[10], &analog[11], &analog[12]);
		ReadAxisAcceleration(&analog[13], &analog[14], &analog[15]);
	}

	HANDLE m_HidHandle;
	HANDLE m_EventHandle;

	OVERLAPPED m_Overlapped;
	USHORT m_InputReportLength;
	USHORT m_OutputReportLength;
	byte* m_ReceiveBuffer;

	BYTE m_DS4State[(int)PAD_DS4_KeyCoord::Count];
	float m_DS4Analog[(int)PAD_DS4_LevelCoord::Count];

	enum DS4_ArrayFlag{
		LAnalogX = 1,
		LAnalogY = 2,
		RAnalogX = 3,
		RAnalogY = 4,

		POV = 5,
		Button = 5,
		Button2 = 6,
		Button3 = 7,

		L2_Level = 8,
		R2_Level = 9,

		XAxisAngularLOW = 13,
		XAxisAngularHIGH = 14,
		YAxisAngularLOW = 15,
		YAxisAngularHIGH = 16,
		ZAxisAngularLOW = 17,
		ZAxisAngularHIGH = 18,

		XAxisAccelerationLOW = 19,
		XAxisAccelerationHIGH = 20,
		YAxisAccelerationLOW = 21,
		YAxisAccelerationHIGH = 22,
		ZAxisAccelerationLOW = 23,
		ZAxisAccelerationHIGH = 24,

		Touch_1st = 35,
		Touch_1stX_LOW = 36,
		Touch_1stX_HIGH = 37,
		Touch_1stY_LOW = 37,
		Touch_1stY_HIGH = 38,

		Touch_2nd = 39,
		Touch_2ndX_LOW = 40,
		Touch_2ndX_HIGH = 41,
		Touch_2ndY_LOW = 41,
		Touch_2ndY_HIGH = 42,
	};
	enum DS4_BitFlag{
		Button_UP = 0,
		Button_UP_RIGHT = 1,
		Button_RIGHT = 2,
		Button_RIGHT_DOWN = 3,
		Button_DOWN = 4,
		Button_DOWN_LEFT = 5,
		Button_LEFT = 6,
		Button_LEFT_UP = 7,
		Button_SQUARE = 1 << 4,
		Button_CROSS = 1 << 5,
		Button_CIRCLE = 1 << 6,
		Button_TRIANGLE = 1 << 7,
		Button_L1 = 1 << 0,
		Button_R1 = 1 << 1,
		Button_L2 = 1 << 2,
		Button_R2 = 1 << 3,
		Button_SHARE = 1 << 4,
		Button_OPTIONS = 1 << 5,
		Button_L3 = 1 << 6,
		Button_R3 = 1 << 7,

		Button_PS = 1 << 0,
		Button_TOUCHPAD = 1 << 1,
		Button_1stTOUCH = 1 << 7,
		Button_2ndTOUCH = 1 << 7,
	};

};