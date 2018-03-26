
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <algorithm>

#include "util/math.hpp" // Rack

int main() {
  // rando math or whatever, test it here!

  printf("%f\n", rack::eucmod(M_PI, 2.0f));
  printf("%f\n", rack::eucmod(-M_PI, 2.0f));
  printf("%f\n", rack::eucmod(M_PI, -2.0f));
  printf("%f\n", rack::eucmod(-M_PI, -2.0f));
  printf("%f\n", fmodf(M_PI, 2.0f));
  printf("%f\n", fmodf(-M_PI, 2.0f));
  printf("%f\n", fmodf(M_PI, -2.0f));
  printf("%f\n", fmodf(-M_PI, -2.0f));

  return 0;
}
