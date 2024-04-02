/*
 * Copyright (C) C. De Wagter
 *
 * This file is part of paparazzi
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
/**
 * @file "modules/computer_vision/opencv_example.cpp"
 * @author C. De Wagter
 * A simple module showing what you can do with opencv on the bebop.
 */


#include "opencv_example.h"



using namespace std;
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
using namespace cv;
#include "opencv_image_functions.h"

/*
#if USE_NPS
#define CV_YUV2GRAY_Y422 COLOR_BGR2GRAY
#endif
*/

Mat computeDepth(const cv::Mat& opticalFlow);


//function for depth
Mat computeDepth(const cv::Mat& opticalFlow) {
    // Compute magnitude of optical flow vectors
    Mat flowMagnitude;
    Mat flowComponents[2];
    split(opticalFlow, flowComponents);
    magnitude(flowComponents[0], flowComponents[1], flowMagnitude);

    // Scale magnitude to range [0, 255]
    Mat magnitudeScaled;
    normalize(flowMagnitude, magnitudeScaled, 0, 255, cv::NORM_MINMAX);

    // Invert magnitude to represent depth (farther objects have smaller magnitudes)
    Mat depth = 255 - magnitudeScaled;
    
    CV_Assert(depth.channels() == 1 && depth.type() == CV_8UC1);
    
    return depth;
}


int opencv_example(char *img, int width, int height)
{
  // Create a new image, using the original bebop image.
  Mat M(height, width, CV_8UC2, img);
  Mat image;
  static Mat lastimage;

  //  Grayscale image example
  cvtColor(M, image, CV_YUV2GRAY_Y422);
  // Canny edges, only works with grayscale image
  //int edgeThresh = 35;

  Mat opticalFlow;
  calcOpticalFlowFarneback(image, lastimage, opticalFlow, 0.6, 4, 10, 5, 7, 1.2, 0);
  
  //estimate depth based on optical flow
  Mat depthRatio = computeDepth(opticalFlow);
  
  //Canny(image, image, edgeThresh, edgeThresh * 3);
  // Convert back to YUV422, and put it in place of the original image
  grayscale_opencv_to_yuv422(depthRatio, img, width, height);
  
  lastimage = image.clone();

  return 0;
}
