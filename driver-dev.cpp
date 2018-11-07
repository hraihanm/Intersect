#include "include/goalFinder-dev.h"
#include "include/goalFinder-dev.cpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <vector>
using namespace std;
using namespace cv;

int main()
{

    VideoCapture cap;
    if (!cap.open(0))
    {
        return 0;
    }
    else
    {
        //Webcam ada
        Mat gambar;
        goalFinder gF;
        gF.init();
        for (;;)
        {
            cap >> gambar;
            gF.setGambar(gambar);
            imshow("Asli", gambar);
            //Pre-Processing
            gF.morphOps();
            gF.canny();
            //Cari garis
            gF.HLP();
            if (waitKey(10) == 27)
                break;
        }
    }
}
