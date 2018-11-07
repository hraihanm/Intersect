#include <iostream>
#include <vector>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "goalFinder-dev.h"

using namespace std;
using namespace cv;

//Constructor
goalFinder::goalFinder() {}

goalFinder::~goalFinder() {}

//Setter
void goalFinder::setGambar(Mat x)
{
    x.copyTo(this->gambar);
    cvtColor(this->gambar, this->gambar, CV_BGR2YUV);
}

void goalFinder::init()
{
    this->minY = 105;
    this->maxY = 217;
    this->thresVal = 180;
    this->maxThres = 255;
    this->erodeCount = 2;
    this->dilateCount = 3;
    this->thresHLT = 150;
    this->structSize = 2;
    this->epsilon = 0.03;
    this->apertureSize = 0;
    this->initWindow();
    this->initTrackbar();
}
void goalFinder::initWindow()
{
    namedWindow("HLT");
    namedWindow("Properties");
    namedWindow("Canny");
    namedWindow("Morph");
}
void goalFinder::initTrackbar()
{
    //Morph
    createTrackbar("Min Y", "Properties", &this->minY, 255, on_trackbar);
    //createTrackbar("Min U", "Properties", &this->minU, 255, on_trackbar);
    //createTrackbar("Min V", "Properties", &this->minV, 255, on_trackbar);
    createTrackbar("Max Y", "Properties", &this->maxY, 255, on_trackbar);
    //createTrackbar("Max U", "Properties", &this->maxU, 255, on_trackbar);
    //createTrackbar("Max V", "Properties", &this->maxV, 255, on_trackbar);
    createTrackbar("Thresold Val", "Properties", &this->thresVal, 255, on_trackbar);
    createTrackbar("Thresold Max", "Properties", &this->maxThres, 255, on_trackbar);
    createTrackbar("Structure Size", "Properties", &this->structSize, 7, on_trackbar);
    createTrackbar("Banyak Erosi", "Properties", &this->erodeCount, 7, on_trackbar);
    createTrackbar("Banyak Dilasi", "Properties", &this->dilateCount, 7, on_trackbar);
    //Canny
    createTrackbar("Aperture Canny", "Properties", &this->apertureSize, 2, on_trackbar);
    //Hough Line Transform
    createTrackbar("Thresold HLT", "Properties", &this->thresHLT, 200, on_trackbar);
}
void goalFinder::on_trackbar(int, void *) {}
//Image Processing
void goalFinder::morphOps()
{
    this->gambar.copyTo(this->hasilMorph);
    //Ambil warna putih saja
    inRange(this->hasilMorph, Scalar(minY, 0, 0), Scalar(maxY, 255, 255), this->hasilMorph);

    //Thresold
    //threshold(this->hasilMorph, this->hasilMorph, this->thresVal, this->maxThres, 0);

    //Erode & Dilatasi gambar
    Mat strElem = getStructuringElement(CV_SHAPE_RECT, Size(this->structSize, this->structSize), Point(0, 0));

    for (int i = 0; i < this->erodeCount; i++) //2 bagus
    {
        erode(this->hasilMorph, this->hasilMorph, strElem);
    }
    for (int i = 0; i < this->dilateCount; i++) //4 bagus
    {
        dilate(this->hasilMorph, this->hasilMorph, strElem);
    }
    imshow("Morph", this->hasilMorph);
}

void goalFinder::canny()
{
    //this->hasilMorph.copyTo(this->hasilCanny);
    int apSize = 2 * (this->apertureSize + 1) + 1;
    Canny(this->hasilMorph, this->hasilCanny, 0, 0, apSize);
    imshow("Canny", hasilCanny);
}
void goalFinder::HLP()
{
    this->gambar.copyTo(this->hasilHLT);

    //HoughLinesP(hasilCanny, garis, 1, CV_PI / 180, thresHLP, minLen, maxGap); //Probabilistic
    HoughLines(this->hasilCanny, this->garis, 1, CV_PI / 180, this->thresHLT);
    Point testps(10,10);

    int intersect_num = 0;

    cout << "Detected: " << this->garis.size() << endl;

    double grad[this->garis.size()], cts[this->garis.size()];

    for (size_t i = 0; i < this->garis.size(); i++)
    {
        //line(hasilHLP, Point(titik[0], titik[1]), Point(titik[2], titik[3]), Scalar(0, 0, 255), 2);
        double rho = this->garis[i][0], theta = this->garis[i][1];
        double deltaT = theta;

        Point p1, p2;

        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        p1.x = cvRound(x0 + 1000 * (-b));
        p1.y = cvRound(y0 + 1000 * (a));
        p2.x = cvRound(x0 - 1000 * (-b));
        p2.y = cvRound(y0 - 1000 * (a));


        grad[i] = ((double)p1.y-(double)p2.y)/((double)p1.x-(double)p2.x);  //Gradien (m) garis ke-i    (y = mx+c)
        cts[i]  = (double)p1.y - (double)p1.x * grad[i];                    //konstanta (c) garis ke-i

        cout << " grad[" << i << "] = " << setprecision(3) << fixed << grad[i] << endl;

        line(this->hasilHLT, p1, p2, Scalar(0, 0, 255), 1);

        for(int j = i-1; j >= 0; j--){

          Point intersect;  //Koordinat titik potong

          cout << " hello the " << j << endl;

          if (intersect_num>= 1000) break;

          if (abs(atan(grad[i])-atan(grad[j])) > 0.01){  //Agar garis dengan yang gradiennya mirip tidak dicek titik potingnya
              intersect.x = -1 * (cts[i]-cts[j])/(grad[i]-grad[j]);
              intersect.y = (cts[i]*grad[j] - cts[j]*grad[i])/(grad[j]-grad[i]);
              intersect_num++;
              cout << "Intersect["<<intersect_num <<"]: " << intersect.x << ", " << intersect.y << endl;
              circle(this->hasilHLT,intersect,5,Scalar(255,0,0),FILLED);
          }
        }
    }
    cout << "Intersect number = " << intersect_num << endl;

    /*
    // Probabilistic Line Transform
    vector<Vec4i> linesP; // will hold the results of the detection
    HoughLinesP(this->hasilCanny, linesP, 1, CV_PI/180, this->thresHLT); // runs the actual detection
    // Draw the lines
    for( size_t i = 0; i < linesP.size(); i++ )
    {
        Vec4i l = linesP[i];
        line(hasilHLT, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 2, LINE_AA);
    }
    // Show results9
    */

    imshow("HLT", this->hasilHLT);
}

/*
void drawIntersect(Point p1, Point p2, Point q1, Point q2) {
    double m1,m2,c1,c2 = 0;

    m1 = (p1.y-p2.y)/(p1.x-p2.x);
    c1 = p1.y - p1.x * ((p1.y-p2.y)/(p1.x-p2.x));

    m2 = (q1.y-q2.y)/(q1.x-q2.x);
    c2 = q1.y - q1.x * ((q1.y-q2.y)/(q1.x-q2.x));

    Point intersect = ((c1-c2)/(m2-m1), (c1*m2 - c2*m1)/(m1-m2));

    circle(hasilHLT,p1,2,Scalar(255,0,0),FILLED);
}
*/

void goalFinder::cekGaris()
{
    this->garisHorizontal.clear();
    this->garisVertikal.clear();
    for (auto g : this->garis)
    {
        float rho = g[0], theta = g[1];
        if (theta > -epsilon && theta < epsilon)
        {
            //Garis vertikal
            garisVertikal.push_back(g);
        }
        else
        {
            theta = theta - CV_PI/2;
            float theta2 = theta - CV_PI/2;
            if ((theta > -epsilon && theta < epsilon) || (theta2 > -epsilon && theta2 < epsilon))
            {
                //Garis horizontal
                garisHorizontal.push_back(g);
            }
        }
    }
    //cout<<"Horizontal : "<<garisHorizontal.size()<<endl;
    //cout<<"Vertikal : "<<garisVertikal.size()<<endl;
}

v4i goalFinder::adaGawang()
{
}
