#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <cmath>
#include <algorithm>

struct Scatter {
	virtual ~Scatter() {}
	virtual bool next(float& x, float &y) = 0;
};

struct TestScatter : Scatter {
	int i = 0;

	bool next(float& x, float &y) override {
		x = i;
		y = i;
		++i;
		return i < 5;
	}
};

struct ButterworthPoles : Scatter {
	int k = 1;
	int n;
	bool conjugate = false;

	ButterworthPoles(int n) : n(n) {}

	bool next(float& x, float &y) override {
		float a = ((float)(2 * k + n - 1)) * M_PI / (float)(2 * n);

		x = std::cos(a);
		y = std::sin(a);
		if (conjugate) {
			y = -y;
		}
		conjugate = !conjugate;
		k += !conjugate;
		return k <= n / 2;
	}
};

struct ChebyshevPoles : Scatter {
	int k = 1;
	int n;
	double ripple;
	double e;
	double ef;
	bool conjugate = false;

	ChebyshevPoles(int n, double ripple) : n(n), ripple(ripple) {
		e = ripple / 10.0;
		e = std::pow(10.0, e);
		e -= 1.0f;
		e = std::sqrt(e);

		ef = std::asinh(1.0 / e) / (float)n;
	}

	bool next(float& x, float &y) override {
		double a = ((double)(2 * k + n - 1)) * M_PI / (double)(2 * n);

		x = -std::sinh(ef) * std::sin(a);
		y = std::cosh(ef) * std::cos(a);
		if (conjugate) {
			y = -y;
		}
		conjugate = !conjugate;
		k += !conjugate;
		return k <= n / 2;
	}
};

int main() {
	// std::unique_ptr<Scatter> s(new TestScatter());
	// std::unique_ptr<Scatter> s(new ButterworthPoles(16));
	std::unique_ptr<Scatter> s(new ChebyshevPoles(16, 0.01));

	float x = 0.0f;
	float y = 0.0f;
	while (true) {
		bool c = s->next(x, y);
		printf("%f, %f\n", x, y);
		if (!c) {
			break;
		}
	}

	return 0;
}
