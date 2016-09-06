
#pragma push_macro("new")
#undef new

#include "MD5.h"
#include "atlstr.h"
#include "wincrypt.h"
#include <time.h>

#include "XorShift.h"

namespace MD5{
	//
	//	MD5ハッシュ計算（128ビット）
	//
	//対応：Windows 95 OSR2以降
	//
	bool GetMD5Hash(const char* pData, DWORD dwLen, char* pcbHashData)
	{
		bool		ret;
		HCRYPTPROV	hCryptProv;
		HCRYPTHASH	hHash;
		BYTE		pbHash[0x7f];
		DWORD		dwHashLen;

		::ZeroMemory(pcbHashData, 16);
		if (pData == NULL || dwLen == 0)
			return	false;

		dwHashLen = 16;
		hHash = NULL;
		hCryptProv = NULL;
		ret = false;
		if (::CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET))
		{
			if (::CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash))
			{
				if (::CryptHashData(hHash, (BYTE*)pData, dwLen, 0))
				{
					if (::CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashLen, 0))
					{
						::memcpy_s(pcbHashData, dwHashLen, pbHash, dwHashLen);
						ret = true;
					}
				}
			}
		}

		if (hHash)
			::CryptDestroyHash(hHash);
		if (hCryptProv)
			::CryptReleaseContext(hCryptProv, 0);

		if (ret == false)
			::ZeroMemory(pcbHashData, dwHashLen);
		else{
			for (int i = 0; i < 16; i++){
				if (pcbHashData[i] == '\0'){
					pcbHashData[i] = 1;
				}
			}
		}

		return	ret;
	}


	bool GenerateMD5(std::string key, MD5HashCoord& hash)
	{
		return GetMD5Hash(key.c_str(), key.length(), hash.key_c);
	}

	bool GenerateMD5(MD5HashCoord& hash){
		static Random::xor128 r;

		/* 現在時刻の取得 */
		time_t timer;
		time(&timer);

		auto key = std::to_string(r.random()) + ctime(&timer);


		return GetMD5Hash(key.c_str(), key.length(), hash.key_c);
	}

}

#pragma pop_macro("new")