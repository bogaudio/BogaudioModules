
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <algorithm>

// // #include "util/math.hpp" // Rack
// #include "simd/vector.hpp" // Rack
// using rack::simd::float_4;
//
// // #include "dsp/fixed.hpp"
// // using namespace bogaudio::dsp;
//
// int main() {
// 	// {
// 	// 	fixed_16_16 x = 5;
// 	// 	fixed_16_16 y = 1;
// 	// 	x = 3;
// 	// 	printf("X=%d\n", (int)(x + y));
// 	// 	y = 2;
// 	// 	printf("X=%d\n", (int)(x - y));
// 	// 	x = y + 5;
// 	// 	printf("X=%d\n", (int)x);
// 	// 	x = y - 3;
// 	// 	printf("X=%d\n", (int)x);
// 	//
// 	// 	x += 2.5;
// 	// 	printf("X=%d\n", (int)x);
// 	// 	printf("X=%f\n", (float)x);
// 	//
// 	// 	x = y - 0.3;
// 	// 	printf("X=%d\n", (int)x);
// 	// 	printf("X=%f\n", (float)x);
// 	// }
// 	//
// 	// {
// 	// 	fixed_32_32 x = 5;
// 	// 	fixed_32_32 y = 1;
// 	// 	x = 3;
// 	// 	printf("X=%d\n", (int)(x + y));
// 	// 	y = 2;
// 	// 	printf("X=%d\n", (int)(x - y));
// 	// 	x = y + 5;
// 	// 	printf("X=%d\n", (int)x);
// 	// 	x = y - 3;
// 	// 	printf("X=%d\n", (int)x);
// 	//
// 	// 	x += 2.5;
// 	// 	printf("X=%d\n", (int)x);
// 	// 	printf("X=%f\n", (float)x);
// 	//
// 	// 	x = y - 0.3;
// 	// 	printf("X=%d\n", (int)x);
// 	// 	printf("X=%f\n", (float)x);
// 	// }
// 	//
// 	// return 0;
//
// 	float_4 a(1.0, 2.0, 3.0, 4.0);
// 	float_4 b = _mm_shuffle_ps(a.v, a.v, _MM_SHUFFLE(2, 1, 0, 0));
// 	b[0] = 0.0;
// 	for (int i = 0; i < 4; ++i) {
// 		printf("a=%f b=%f\n", a[i], b[i]);
// 	}
// }

#include "dsp/pitch.hpp"
using namespace bogaudio::dsp;

int main() {
	printf("%f\n", frequencyToSemitone(1.0f));
	printf("%f\n", semitoneToFrequency(0.0f));
	printf("%f\n", semitoneToFrequency(60.0f));
	printf("%f\n", semitoneToFrequency(120.0f));
	printf("%f\n", frequencyToSemitone(20000.0f));
	return 0;
}
