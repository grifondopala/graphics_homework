#include "lib.h" 

using namespace std;
using namespace cv;

void showImage(Mat* image) {
    imshow("Display window", *image);
}

void saveImage(Mat* image, string name) {
    imwrite("D:/MEPHI/KG/Homework/Homework/Homework/result/" + name, *image);
}

Mat loadImage(string name) {
    Mat image = imread("D:/MEPHI/KG/Homework/Homework/Homework/img/" + name);
    if (image.empty()) {
        cout << "Can not load image " << name << endl;
    }
    return image;
}

void BSplineLine() {
    Mat image = Mat::zeros({ 1000, 1000 }, CV_8UC3);

    fillBackground(image, WHITE);

    MyPoint p0 = { 10, 10 };
    MyPoint p1 = { 400, 10 };
    MyPoint p2 = { 800, 800 };
    MyPoint p3 = { 400, 800 };

    vector<MyPoint> points = { p0, p1, p2, p3 };

    DrawBSplineConnective(points, image, BLACK);

    MyPoint p4 = { 600, 400 };
    MyPoint p5 = { 650, 200 };
    MyPoint p6 = { 750, 200 };
    MyPoint p7 = { 800, 400 };

    points = { p4, p5, p6, p7 };

    DrawBSplineConnective(points, image, BLACK);

    saveImage(&image, "bspline.png");
}

void Halftoning(string name, int cellWidth, int cellHeight) {
    Mat image = loadImage(name + ".jpg");

    Mat grayImage = createGrayscale(image);

    Mat halfImage = halftoning(cellWidth, cellHeight, grayImage);

    saveImage(&halfImage, name + ".png");
}

void NonExterior() {
    Mat image = Mat::zeros({ 1000, 1000 }, CV_8UC3);

    fillBackground(image, WHITE);

    vector<MyPoint> points = {
        {200, 200},
        {100, 100},
        {300, 100},
        {150, 170},
        {300, 120},
        {350, 150},
        {120, 140}
    };

    drawPolygonWithNonExterior(points, image, RED, BLUE);

    points = {
        {500, 200},
        {400, 100},
        {600, 100},
        {450, 170},
        {600, 120},
        {650, 150},
        {420, 140}
    };

    drawPolygon(points, image, BLUE, points.size());

    points = {
        {200, 600},
        {250, 500},
        {300, 600},
        {150, 550},
        {350, 550},
        {200, 600},
    };

    drawPolygonWithNonExterior(points, image, RED, BLUE);

    points = {
         {500, 600},
         {550, 500},
         {600, 600},
         {450, 550},
         {650, 550},
         {500, 600},
    };

    drawPolygon(points, image, BLUE, points.size());

    saveImage(&image, "nonExterior.png");
}

int main()
{
    BSplineLine();

    Halftoning("dog", 60, 40);

    Halftoning("football", 40, 40);

    NonExterior();

    waitKey(0);
}
