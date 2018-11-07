#ifndef GOAL_FINDER
#define GOAL_FINDER

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <vector>

using namespace std;
using namespace cv;

#define v2f vector<Vec2f>
#define v4i vector<Vec4i>

class goalFinder
{
private:
  Mat gambar, hasilMorph, hasilCanny, hasilHLT;
  v2f garis, garisHorizontal, garisVertikal;
  //Parameter Image Processing
  //MorphOps
  int minY, minU, minV, maxY, maxU, maxV;
  int thresVal, maxThres, structSize, erodeCount, dilateCount;
  //Canny
  int apertureSize;
  //Hough Line Transform
  int thresHLT;
  //Deteksi garis ujung
  bool adaGaris;
  //Deteksi gawang
  float epsilon;

public:
  //Constructor
  goalFinder();
  //Destructor
  ~goalFinder();
  //Setter
  void setGambar(Mat x);
  void init();

  //Init GUI
  static void on_trackbar(int, void *);
  void initWindow();
  void initTrackbar();

  //Fungsi lainnya

  //Image processing
  void morphOps();
  void canny();
  void HLP();

  void cekGaris();
  //Cek apakah ada garis horizontal/vertikal, lalu di push ke vector yang sesuai

  //void intersect();
  // Mencari titik potong dua garis

  v4i adaGawang();
  //Mereturn 4-tuple koordinat kotak gawang

  bool adaGarisUjung();
  //return true jika ketemu garis ujung
};

#endif
