//
// HSVColorTest.cpp
//
#ifdef _WIN32
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#pragma warning(disable: 4819)
#endif

#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "opencv_world300d.lib")
#else
#endif
#pragma comment(lib, "opencv_world300.lib")
#endif

const char *win_name_canvas = "HSVColorTest.cpp";

int val_h = 0;
int val_s = 255;
int val_v = 255;

void rgb2hsv(const uchar &src_r, const uchar &src_g, const uchar &src_b, uchar &dst_h, uchar &dst_s, uchar &dst_v);
void hsv2rgb(const uchar &src_h, const uchar &src_s, const uchar &src_v, uchar &dst_r, uchar &dst_g, uchar &dst_b);

void draw_text(cv::Mat &canvas, const char *str, int x, int y, float scale);
void draw_rgbhsv(cv::Mat &canvas, const char *label, int x, int y, int r, int g, int b, int h, int s, int v);

int main(int argc, char* argv[])
{
	cv::Mat hsv_img(cv::Size(480, 240), CV_8UC3);
	cv::Mat canvas_img;

	cv::namedWindow(win_name_canvas);
	cv::createTrackbar("H", win_name_canvas, &val_h, 179, nullptr, nullptr);
	cv::createTrackbar("S", win_name_canvas, &val_s, 255, nullptr, nullptr);
	cv::createTrackbar("V", win_name_canvas, &val_v, 255, nullptr, nullptr);

	while(true) {
		// HSV -> BGR
		hsv_img.setTo(cv::Scalar(val_h, val_s, val_v));
		cv::cvtColor(hsv_img, canvas_img, cv::COLOR_HSV2BGR);

		// test : HSV -> BGR -> HSV
		uchar r, g, b, h, s, v;
		h = (uchar)val_h;
		s = (uchar)val_s;
		v = (uchar)val_v;
		hsv2rgb(h, s, v, r, g, b);
		rgb2hsv(r, g, b, h, s, v);

		cv::Vec3b hsv = hsv_img.at<cv::Vec3b>(cv::Point(0, 0));
		cv::Vec3b rgb = canvas_img.at<cv::Vec3b>(cv::Point(0, 0));

		draw_rgbhsv(canvas_img, "opencv: ", 10, 20, rgb[2], rgb[1], rgb[0], hsv[0], hsv[1], hsv[2]);
		draw_rgbhsv(canvas_img, "test: ", 34, 46, r, g, b, h, s, v);

		cv::imshow(win_name_canvas, canvas_img);

		int c = cv::waitKey(1);
		if (c == 27) break;
	}
	cv::destroyAllWindows();
 
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
int max_val3_(const int &a, const int &b, const int &c)
{
	if (a > b) {
		if (a > c) return a;
		else       return c;
	}
	else {
		if (b > c) return b;
		else       return c;
	}
}

int min_val3_(const int &a, const int &b, const int &c)
{
	if (a < b) {
		if (a < c) return a;
		else       return c;
	}
	else {
		if (b < c) return b;
		else       return c;
	}
}

void rgb2hsv(
	const uchar &src_r,
	const uchar &src_g,
	const uchar &src_b,
	uchar &dst_h,
	uchar &dst_s,
	uchar &dst_v
	)
{
	int r = src_r;
	int g = src_g;
	int b = src_b;

	int h, s, v;

	// see also... https://ja.wikipedia.org/wiki/HSV%E8%89%B2%E7%A9%BA%E9%96%93
	int max = max_val3_(r, g, b);
	int min = min_val3_(r, g, b);
	int diff = max - min;

	if (diff == 0) {
		h = 0;
	}
	else {
		if (min == b) {
			h = 60 * (g - r) / diff + 60;
		}
		else if (min == r) {
			h = 60 * (b - g) / diff + 180;
		}
		else {
			h = 60 * (r - b) / diff + 300;
		}
	}
	// normalize 0`359
	if (h < 0)    h += 360; 
	if (h >= 360) h -= 360;

	if (max == 0) {
		s = 0;
	}
	else {
		s = 255 * diff / max;
	}

	v = max;

	dst_h = (uchar)(h / 2); // 0`179
	dst_s = (uchar)s; // 0`255
	dst_v = (uchar)v; // 0`255
}

void hsv2rgb(
	const uchar &src_h,
	const uchar &src_s,
	const uchar &src_v,
	uchar &dst_r, uchar
	&dst_g, uchar &dst_b
	)
{
	float h = src_h / 180.0f;  // 0`180 -> 0.0`1.0
	float s = src_s / 255.0f;  // 0.0`1.0
	float v = src_v / 255.0f;  // 0.0`1.0

	float r, g, b;

	// see also... https://ja.wikipedia.org/wiki/HSV%E8%89%B2%E7%A9%BA%E9%96%93
	r = v;
	g = v;
	b = v;

	if (s > 0.0) {
		int i = (int)(h * 6);
		float f = (h * 6) - (float)i;
		
		switch (i) {
		case 0:
			g *= 1.0f - s * (1.0f - f);
			b *= 1.0f - s;
			break;
		case 1:
			r *= 1.0f - s * f;
			b *= 1.0f - s;
			break;
		case 2:
			r *= 1.0f - s;
			b *= 1.0f - s * (1.0f - f);
			break;
		case 3:
			r *= 1.0f - s;
			g *= 1.0f - s * f;
			break;
		case 4:
			r *= 1.0f - s * (1.0f - f);
			g *= 1.0f - s;
			break;
		case 5:
			g *= 1.0f - s;
			b *= 1.0f - s * f;
			break;
		}
	}

	dst_r = (uchar)(r * 255);
	dst_g = (uchar)(g * 255);
	dst_b = (uchar)(b * 255);
}

////////////////////////////////////////////////////////////////////////////////////
void draw_text(cv::Mat &canvas, const char *str, int x, int y, float scale)
{
	int w = 2;
	for (int dy = -w; dy <= w; ++dy) {
		for (int dx = -w; dx <= w; ++dx) {
			cv::putText(canvas, str, cv::Point(x + dx, y + dy), cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(0, 0, 0), 1, CV_AA);
		}
	}

	cv::putText(canvas, str, cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(255, 255, 255), 1, CV_AA);
}

void draw_rgbhsv(cv::Mat &canvas_img, const char *label, int x, int y, int r, int g, int b, int h, int s, int v)
{
	std::stringstream ss;
	ss << label
		<< "rgb=(" << (int)r << ", " << (int)g << ", " << (int)b << "), "
		<< "hsv=(" << (int)h << ", " << (int)s << ", " << (int)v << ")";

	draw_text(canvas_img, ss.str().c_str(), x, y, 0.5f);
}

