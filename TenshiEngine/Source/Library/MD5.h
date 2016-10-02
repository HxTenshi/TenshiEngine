#pragma once

namespace MD5{

	union MD5HashCoord{
		char key_c[16];
		INT32 key_i[4];
		bool operator<(const MD5HashCoord &right) const {
			return this->key_i[0] != right.key_i[0] ?
				this->key_i[0] < right.key_i[0] :
				this->key_i[1] != right.key_i[1] ?
				this->key_i[1] < right.key_i[1] :
				this->key_i[2] != right.key_i[2] ?
				this->key_i[2] < right.key_i[2] :
				this->key_i[3] < right.key_i[3]
				;
		}

		bool operator==(const MD5HashCoord &right) const {
			return this->key_i[0] == right.key_i[0] &&
				this->key_i[1] == right.key_i[1] &&
				this->key_i[2] == right.key_i[2] &&
				this->key_i[3] == right.key_i[3];
		}

		std::string GetString() const{
			std::string temp;
			for (int i = 0; i < 16;i++)
				temp.push_back(key_c[i]);
			return temp;
		}

		MD5HashCoord(const char* hash){
			memset(key_c, 0, sizeof(char)* 16);
			memcpy_s(key_c, sizeof(key_c), hash, sizeof(char) * 16);
		}
		MD5HashCoord(){}
	};

	//	キーからMD5ハッシュ計算（128ビット)
	bool GenerateMD5(std::string key, MD5HashCoord& hash);
	//	ランダムにMD5ハッシュ計算（128ビット)
	bool GenerateMD5(MD5HashCoord& hash);
}