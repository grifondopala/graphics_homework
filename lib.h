#ifndef LIB_H 
#define LIB_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>

using namespace cv;
using namespace std;

enum CLPointType { LEFT, RIGHT, BEYOND, BEHIND, BETWEEN, ORIGIN, DESTINATION };

struct MyPoint {
	int x;
	int y;
};

struct Segment {
	MyPoint p0;
	MyPoint p1;
};

const Vec3b BLACK = { 0, 0, 0 };
const Vec3b WHITE = { 255, 255, 255 };
const Vec3b RED = { 0, 0, 255 };
const Vec3b GREEN = { 0, 255, 0 };
const Vec3b BLUE = { 255, 0, 0 };
const Vec3b YELLOW = { 0, 255, 255 };


void setPixel(int x, int y, Mat& image, Vec3b color);

void fillBackground(Mat& image, Vec3b color);

void DrawBSpline(MyPoint& p0, MyPoint& p1, MyPoint& p2, MyPoint& p3, Mat& image, Vec3b color);

vector<MyPoint> drawPolygon(const vector<MyPoint>& points, Mat& image, Vec3b borderColor, int n);

void DrawBSplineConnective(vector<MyPoint>& points, Mat& image, Vec3b color);

vector<MyPoint> drawLine(int x1, int y1, int x2, int y2, Mat& image, Vec3b color);

void drawPolygonWithNonExterior(const vector<MyPoint>& points, Mat& image, Vec3b externalColor, Vec3b insideColor);

// ==============================================
// halftoning

Mat halftoning(int widthCell, Mat& image);

Mat generateTestImage();

#endif#pragma once
