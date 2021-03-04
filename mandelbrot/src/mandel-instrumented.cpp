#include <iostream>
#include <complex>
#include <vector>
#include <chrono>
#include <functional>


#include "window.h"
#include "save_image.h"
#include "utils.h"
#include "../../tracing.hpp"

//#include "tbb/tbb.h"
///#include "tbb/pipeline.h"

// clang++ -std=c++11 -stdlib=libc++ -O3 save_image.cpp utils.cpp mandel.cpp -lfreeimage

enum {
  LOOP60,
  LOOP62,
  LOOP33,
  LOOP_NUMBER
} __mark;

bool __visited[LOOP_NUMBER] = {false};

const char * __name[LOOP_NUMBER] =
  {
    "loop60-body",
    "loop62-body",
    "loop33-body"
  };

// Use an alias to simplify the use of complex type
using Complex = std::complex<double>;


// Convert a pixel coordinate to the complex domain
Complex scale(window<int> &scr, window<double> &fr, Complex c) {
	Complex aux(c.real() / (double)scr.width() * fr.width() + fr.x_min(),
		c.imag() / (double)scr.height() * fr.height() + fr.y_min());
	return aux;
}

// Check if a point is in the set or escapes to infinity, return the number if iterations
int escape(Complex c, int iter_max, const std::function<Complex( Complex, Complex)> &func) {
	Complex z(0,0);
	int iter = 0;

	__begin_loop(LOOP33);
	while (abs(z) < 2.0 && iter < iter_max) {
		__begin_iteration(LOOP33);
		z = func(z, c);
		iter++;
		__end_iteration(LOOP33);
	}
	__end_loop(LOOP33);

	return iter;
}

// Loop over each pixel from our image and check if the points associated with this pixel escape to infinity
/*
 void get_number_iterations(window<int> &scr, window<double> &fract, int iter_max, std::vector<int> &colors,
	const std::function<Complex(Complex, Complex)> &func) {
	int k = 0;
	int N = scr.width();
	for(int i = scr.y_min(); i < scr.y_max(); ++i) {
		for(int j = scr.x_min(); j < scr.x_max(); ++j) {
			Complex c((double) (j), (double) (i));
			c = scale(scr, fract, c);
			colors[N * i + j] = escape(c, iter_max, func);
		}
	}
}*/

void get_number_iterations(window<int> &scr, window<double> &fract, int iter_max, std::vector<int> &colors,
		const std::function<Complex(Complex, Complex)> &func) {
	int N = scr.width();
	__begin_loop(LOOP60);
	for(int i = scr.y_min(); i < scr.y_max(); ++i) {
		__begin_iteration(LOOP60);
		int Ni = N*i;
		__begin_loop(LOOP62);
		for(int j = scr.x_min(); j < scr.x_max(); ++j) {
			__begin_iteration(LOOP62);
			Complex c((double) (j), (double) (i));
			c = scale(scr, fract, c);
			colors[Ni + j] = escape(c, iter_max, func);
			__end_iteration(LOOP62);
		}
		__end_loop(LOOP62);
		__end_iteration(LOOP60);
	}
	__end_loop(LOOP60);
}

void fractal(window<int> &scr, window<double> &fract, int iter_max, std::vector<int> &colors,
	const std::function<Complex( Complex, Complex)> &func, const char *fname, bool smooth_color) {
	auto start = std::chrono::steady_clock::now();
	get_number_iterations(scr, fract, iter_max, colors, func);
	auto end = std::chrono::steady_clock::now();
	std::cout << "Time to generate " << fname << " = " << std::chrono::duration <double, std::milli> (end - start).count() << " [ms]" << std::endl;

	// Save (show) the result as an image
	plot(scr, colors, iter_max, fname, smooth_color);
}

void small_mandelbrot() {
	// Define the size of the image
	window<int> scr(0, 4, 0, 4);
	// The domain in which we test for points
	window<double> fract(-0.1, 0.1, -0.1, 0.1);
	// The function used to calculate the fractal
	auto func = [] (Complex z, Complex c) -> Complex {return z * z + c; };
	int iter_max = 4;
	const char *fname = "mandelbrot.png";
	bool smooth_color = true;
	std::vector<int> colors(scr.size());
	fractal(scr, fract, iter_max, colors, func, fname, smooth_color);
}

void mandelbrot() {
	// Define the size of the image
	window<int> scr(0, 1200, 0, 1200);
	// The domain in which we test for points
	window<double> fract(-2.2, 1.2, -1.7, 1.7);

	// The function used to calculate the fractal
	auto func = [] (Complex z, Complex c) -> Complex {return z * z + c; };

	int iter_max = 500;
	const char *fname = "mandelbrot.png";
	bool smooth_color = true;
	std::vector<int> colors(scr.size());

	// Experimental zoom (bugs ?). This will modify the fract window (the domain in which we calculate the fractal function) 
	zoom(1.0, -1.225, -1.22, 0.15, 0.16, fract); //Z2
	
	fractal(scr, fract, iter_max, colors, func, fname, smooth_color);
}

void triple_mandelbrot() {
	// Define the size of the image
	window<int> scr(0, 1200, 0, 1200);
	// The domain in which we test for points
	window<double> fract(-1.5, 1.5, -1.5, 1.5);

	// The function used to calculate the fractal
	auto func = [] (Complex z, Complex c) -> Complex {return z * z * z + c; };

	int iter_max = 500;
	const char *fname = "triple_mandelbrot.png";
	bool smooth_color = true;
	std::vector<int> colors(scr.size());

	fractal(scr, fract, iter_max, colors, func, fname, smooth_color);
}

int main() {
	__begin_tracing();
	small_mandelbrot();
//	mandelbrot();
//	triple_mandelbrot();

	return 0;
}