#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <algorithm>

#include "dsp/analyzer.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

// KaiserWindow w(1024);
// float y(float x) {
//   return w._window[(int)x];
// }

// Chebyshev gain coefficient; x is ripple.
// float y(float x) {
// 	float e = x / 10.0;
// 	e = std::pow(10.0f, e);
// 	e -= 1.0f;
// 	e = std::sqrt(e);
//
// 	return 1.0f / (2.0f * e);
// }

// // Q https://en.wikipedia.org/wiki/Q_factor
// float y(float x) {
// 	x *= 2.0f;
// 	return std::pow(2.0f, x / 2.0f) / (std::pow(2.0f, x) - 1.0f);
// }

const float m = 20000.0f;
float y(float x) {
	x /= m;
	x = powf(x, 1.25f);
	x *= 0.5f * m;
	return x;
}

int main() {
  const float xMin = 0.1f;
  // const float xMax = 1023.0f;
  const float xMax = 100.0f;
  const float samples = 1024.0f;

  const float delta = (xMax - xMin) / samples;
  float x = xMin;
  while (x <= xMax) {
    printf("%f, %f\n", x, y(x));
    x += delta;
  }
  // printf("%f\n", y(1.0f));
  return 0;
}
