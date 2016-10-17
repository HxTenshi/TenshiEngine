#pragma once

namespace MD5{

	union MD5HashCode{
		char key_c[16];
		int key_i[4];
		bool operator<(const MD5HashCode &right) const {
			return this->key_i[0] != right.key_i[0] ?
				this->key_i[0] < right.key_i[0] :
				this->key_i[1] != right.key_i[1] ?
				this->key_i[1] < right.key_i[1] :
				this->key_i[2] != right.key_i[2] ?
				this->key_i[2] < right.key_i[2] :
				this->key_i[3] < right.key_i[3]
				;
		}

		bool operator==(const MD5HashCode &right) const {
			return this->key_i[0] == right.key_i[0] &&
				this->key_i[1] == right.key_i[1] &&
				this->key_i[2] == right.key_i[2] &&
				this->key_i[3] == right.key_i[3];
		}
		MD5HashCode& operator=(const MD5HashCode &right){
			this->key_i[0] = right.key_i[0];
			this->key_i[1] = right.key_i[1];
			this->key_i[2] = right.key_i[2];
			this->key_i[3] = right.key_i[3];
			return *this;
		}

		std::string GetString() const{
			std::string temp;
			for (int i = 0; i < 16;i++)
				temp.push_back(key_c[i]);
			return temp;
		}


		bool IsNull() const {
			return this->key_i[0] == 0 &&
				this->key_i[1] == 0 &&
				this->key_i[2] == 0 &&
				this->key_i[3] == 0;
		}
		void clear(){
			key_i[0] = 0;
			key_i[1] = 0;
			key_i[2] = 0;
			key_i[3] = 0;
		}


		MD5HashCode(const char* hash){
			memset(key_c, 0, sizeof(char)* 16);
			memcpy_s(key_c, sizeof(key_c), hash, sizeof(char) * 16);
		}
		MD5HashCode(){
			key_i[0] = 0;
			key_i[1] = 0;
			key_i[2] = 0;
			key_i[3] = 0;
		}
	};

	//	キーからMD5ハッシュ計算（128ビット)
	bool GenerateMD5(std::string key, MD5HashCode& hash);
	//	ランダムにMD5ハッシュ計算（128ビット)
	bool GenerateMD5(MD5HashCode& hash);
}