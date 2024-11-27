#include "lib.h"

using namespace std;
using namespace cv;

void setPixel(int x, int y, Mat& image, Vec3b color) {
    if (x >= 0 && x < image.cols && y >= 0 && y < image.rows) {
        image.at<Vec3b>(y, x) = color;
    }
}

void fillBackground(Mat& image, Vec3b color) {
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            setPixel(x, y, image, color);
        }
    }
}

vector<MyPoint> drawLine(int x1, int y1, int x2, int y2, Mat& image, Vec3b color) {
    int x = x1, y = y1;
    int dx = x2 - x1, dy = y2 - y1;
    int ix = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    int iy = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
    dx = abs(dx);
    dy = abs(dy);

    vector<MyPoint> borderPoints;

    if (dx >= dy) {
        int e = 2 * dy - dx;
        for (int i = 0; i <= dx; i++) {
            borderPoints.push_back({ x, y });
            setPixel(x, y, image, color);
            if (e >= 0) {
                y += iy;
                e -= 2 * dx;
            }
            x += ix;
            e += 2 * dy;
        }
    }

    else {
        int e = 2 * dx - dy;
        for (int i = 0; i <= dy; i++) {
            borderPoints.push_back({ x, y });
            setPixel(x, y, image, color);
            if (e >= 0) {
                x += ix;
                e -= 2 * dy;
            }
            y += iy;
            e += 2 * dx;
        }
    }

    return borderPoints;
}


MyPoint BSpline(MyPoint& p0, MyPoint& p1, MyPoint& p2, MyPoint& p3, double t)
{
    int x = ((1 - t) * (1 - t) * (1 - t) * p0.x + (3 * t * t * t - 6 * t * t + 4) * p1.x + (-3 * t * t * t + 3 * t * t + 3 * t + 1) * p2.x + t * t * t * p3.x) / 6.0;
    int y = ((1 - t) * (1 - t) * (1 - t) * p0.y + (3 * t * t * t - 6 * t * t + 4) * p1.y + (-3 * t * t * t + 3 * t * t + 3 * t + 1) * p2.y + t * t * t * p3.y) / 6.0;
    return { x, y };
}

double Dist(MyPoint p0) {
    return abs(p0.x) + abs(p0.y);
}

MyPoint GetDistV(MyPoint p0, MyPoint p1, MyPoint p2) {
    int x = p0.x - 2 * p1.x + p2.x;
    int y = p0.y - 2 * p1.y + p2.y;
    return { x, y };
}

MyPoint addedPoint(MyPoint p0, MyPoint p1) {
    int x = 2 * p0.x - p1.x;
    int y = 2 * p0.y - p1.y;

    return { x, y };
}

double getN(MyPoint p0, MyPoint p1, MyPoint p2, MyPoint p3) {
    double H = max(Dist(GetDistV(p0, p1, p2)), Dist(GetDistV(p1, p2, p3)));
    return 1 + sqrt(3 * H);
}

void DrawBSpline(MyPoint& p0, MyPoint& p1, MyPoint& p2, MyPoint& p3, Mat& image, Vec3b color) {
    double t = 0;
    double step = 1 / getN(p0, p1, p2, p3);

    MyPoint prev;

    while (t <= 1 + step) {
        MyPoint point = BSpline(p0, p1, p2, p3, t);

        if (t != 0) {
            drawLine(prev.x, prev.y, point.x, point.y, image, color);
        }

        prev = point;
        t += step;
    }
}

void DrawBSplineConnective(vector<MyPoint>& points, Mat& image, Vec3b color) {
    drawPolygon(points, image, RED, points.size());

    MyPoint leftPoint = addedPoint(points[0], points[1]);
    MyPoint rightPoint = addedPoint(points[points.size() - 1], points[points.size() - 2]);

    points.insert(points.begin(), leftPoint);
    points.push_back(rightPoint);

    for (int i = 0; i < points.size() - 3; ++i) {
        DrawBSpline(points[i], points[i + 1], points[i + 2], points[i + 3], image, color);
    }
}

// при floodFill8 границу полигона наискосок перепрыгивало и все внутри закрашивало
void floodFill4(const vector<MyPoint> startPoints, Vec3b background, Vec3b borderColor, int minX, int minY, int maxX, int maxY, Mat& image)
{
    int dx[4] = { 0, 1, 0, -1 };
    int dy[4] = { -1, 0, 1, 0 };

    vector<MyPoint> stack = startPoints;
    while (!stack.empty()) {
        MyPoint currentPoint = stack.back();
        stack.pop_back();
        Vec3b currentColor = image.at<Vec3b>(currentPoint.y, currentPoint.x);
        if (currentColor != borderColor) {
            setPixel(currentPoint.x, currentPoint.y, image, background);
        }
        for (int i = 0; i < 4; i++) {
            int nx = currentPoint.x + dx[i];
            int ny = currentPoint.y + dy[i];

            Vec3b currentColor = image.at<Vec3b>(ny, nx);

            if (nx >= minX && nx <= maxX && ny >= minY && ny <= maxY) {
                if (currentColor != background && currentColor != borderColor) {
                    stack.push_back({ nx, ny });
                }
            }
        }
    }
}

vector<MyPoint> drawPolygon(const vector<MyPoint>& points, Mat& image, Vec3b borderColor, int n) {
    vector<MyPoint> border;

    for (int i = 0; i < points.size(); ++i) {
        vector<MyPoint> line = drawLine(points[i].x, points[i].y, points[(i + 1)%n].x, points[(i + 1)%n].y, image, borderColor);

        border.insert(border.end(), line.begin(), line.end());
    }

    return border;
}

void drawPolygonWithNonExterior(const vector<MyPoint>& points, Mat& image, Vec3b externalColor, Vec3b insideColor) {
    vector<MyPoint> border = drawPolygon(points, image, insideColor, points.size());

    int minX = points[0].x;
    int minY = points[0].y;
    int maxX = points[0].x;
    int maxY = points[0].y;

    for (int i = 1; i < points.size(); ++i) {
        minX = min(points[i].x, minX);
        minY = min(points[i].y, minY);
        maxX = max(points[i].x, maxX);
        maxY = max(points[i].y, maxY);
    }

    minX = minX - 2;
    minY = minY - 2;
    maxX = maxX + 2;
    maxY = maxY + 2;

    MyPoint p1 = { minX, minY };
    MyPoint p2 = { maxX, minY };
    MyPoint p3 = { maxX, maxY };
    MyPoint p4 = { minX, maxY };

    floodFill4({ p1, p2, p3, p4 }, externalColor, insideColor, minX, minY, maxX, maxY, image);

    floodFill4(border, insideColor, externalColor, minX, minY, maxX, maxY, image);
}


// ========================================================================================
// halftoning

Mat createGrayscale(const Mat& image) {
    Mat gray_image;
    cvtColor(image, gray_image, COLOR_BGR2GRAY);

    return gray_image;
}

Mat halftoning(int widthCell, int heightCell, Mat& image) {
    Mat outputImage(image.rows * heightCell, image.cols * widthCell, CV_8UC1, Scalar(255));

    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            uchar pixelValue = (255 - image.at<uchar>(y, x));

            int radius = (int)(pixelValue * min(widthCell, heightCell) / 255.0 * 0.5);

            int centerX = x * widthCell + widthCell / 2;
            int centerY = y * heightCell + heightCell / 2;

            circle(outputImage, Point(centerX, centerY), radius, Scalar(0), FILLED);
        }
    }

    return outputImage;
}