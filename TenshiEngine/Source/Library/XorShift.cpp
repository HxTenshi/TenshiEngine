#include "XorShift.h"

#include <climits>
#include <random>
#include <algorithm>

namespace Random{

	xor128::xor128(){
		// シードが与えられない時は、
		// システムのハードウェアエントロピーソースからシードを生成
		std::random_device rd;
		w = rd();

		for (int i = 0; i < 10; i++){  // 数回空読み(不要?)
			random();
		}
	}
	unsigned xor128::random(){
		unsigned t;
		t = x ^ (x << 11);
		x = y; y = z; z = w;
		return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
	}


	float xor128::random(float min, float max){
		float t = max - min;
		unsigned i = random();
		float f = *(float*)&i;
		return std::fmod(abs(f), t) + min;

	}
	int xor128::random(int min, int max){
		int t = max - min;
		int i = random();
		return i%max + min;
	}

}