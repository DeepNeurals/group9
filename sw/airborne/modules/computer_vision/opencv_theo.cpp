/*
Test implementation opencv_theo
 */


#include "opencv_theo.h"
#include <iostream> // Add this line to include the iostream header


#include <chrono>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;


/* No need anymore, same OpenCV version on both devices
#if USE_NPS
#define CV_YUV2GRAY_Y422 COLOR_BGR2GRAY
#endif
*/

//declaration of the functions
void grayscale_opencv_to_yuv422(cv::Mat image, char *img);
Mat computeDepth(const cv::Mat& opticalFlow);
Mat convertToGrayscale(const cv::Mat& input);


//function to write to the image
void grayscale_opencv_to_yuv422(Mat image, char *img)
{
  //CV_Assert(image.depth() == CV_YUV2GRAY_Y422);
  CV_Assert(image.channels() == 1);

  int n_rows = image.rows;
  int n_cols = image.cols;

  // If the image is one block in memory we can iterate over it all at once!
  if (image.isContinuous()) {
    n_cols *= n_rows;
    n_rows = 1;
  }

  // Iterate over the image, setting only the Y value
  // and setting U and V to 127
  int i, j;
  uchar *p;
  int index_img = 0;
  for (i = 0; i < n_rows; ++i) {
    p = image.ptr<uchar>(i);
    for (j = 0; j < n_cols; j++) {
      img[index_img++] = 127;
      img[index_img++] = p[j];


    }
  }
}

Mat convertToGrayscale(const cv::Mat& input) {
    // Convert the floating-point matrix to an 8-bit unsigned integer matrix
    Mat output;
    convertScaleAbs(input, output);

    // Return the grayscale image
    return output;
}

//function for depth
Mat computeDepth(const cv::Mat& opticalFlow) {
    // Compute magnitude of optical flow vectors
    Mat flowMagnitude;
    Mat flowComponents[2]; //horizontal and vertical
    split(opticalFlow, flowComponents);

    //std::cout << "Size horizontal " << flowComponents[0].size() << std::endl;
    //std::cout << "Size vertical " << flowComponents[1].size() << std::endl;

    // peuetre faire varier le ratio d' importance entre vitesse en x ou en y vu que le drone ne monte pas
    magnitude(flowComponents[0], flowComponents[1], flowMagnitude);

    // Scale magnitude to range [0, 255]
    Mat magnitudeScaled;
    normalize(flowMagnitude, magnitudeScaled, 0, 255, cv::NORM_MINMAX);

    // Invert magnitude to represent depth (farther objects have smaller magnitudes)
    Mat depth_of_image = 255 - magnitudeScaled;  //close objects have a darker appearance
    
    // Print type and channels of depth matrix
    //std::cout << "Depth matrix type: " << depth_of_image.type() << std::endl;
    //std::cout << "Depth matrix channels: " << depth_of_image.channels() << std::endl;

    CV_Assert(depth_of_image.channels() == 1 && depth_of_image.type() == CV_32FC1);

    //transfrom from CV32FC1 to grayscale
    Mat grayscaleImage = convertToGrayscale(depth_of_image);    
    return grayscaleImage;
}


int opencv_theo(char *img, int width, int height)
{
  //Create a new image, using the original bebop image.
  Mat M(height, width, CV_8UC2, img);
  Mat image;
  static Mat lastimage;

  //Grayscale image example
  cvtColor(M, image, CV_YUV2GRAY_Y422);
  
  // Canny edges, only works with grayscale image
  //int edgeThresh = 35;

  Mat opticalFlow;
  if (!lastimage.empty()) {
    auto start_time = std::chrono::high_resolution_clock::now();
    calcOpticalFlowFarneback(lastimage, image, opticalFlow, 0.6, 4, 10, 5, 7, 1.2, 0);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "Time taken by your_function: " << duration << " milliseconds" << std::endl;
    std::cout << opticalFlow.type() << std::endl;
    //estimate depth based on optical flow
    Mat estimated_depth = computeDepth(opticalFlow);
    //Convert back to YUV422, and put it in place of the original image
    grayscale_opencv_to_yuv422(estimated_depth, img);
    //grayscale_opencv_to_yuv422(image, img, width, height);
  }else {// If lastimage is not available, compute optical flow between the same image
    std::cout << "no image yet" << std::endl;  
    // Convert back to YUV422, and put it in place of the original image
    //grayscale_opencv_to_yuv422(image, img, width, height);
  }

  //Canny(image, image, edgeThresh, edgeThresh * 3);
  
  lastimage = image.clone();

  return 0;
}
