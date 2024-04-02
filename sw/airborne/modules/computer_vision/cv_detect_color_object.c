// /**
//  * @file modules/computer_vision/cv_detect_object.h
//  * Assumes the object consists of a continuous color and checks
//  * if you are over the defined object or not
//  */

// // Own header
// #include "modules/computer_vision/cv_detect_color_object.h"
// #include "modules/computer_vision/cv.h"
// #include "modules/core/abi.h"
// #include "std.h"

// #include <stdio.h>
// #include <stdbool.h>
// #include <math.h>
// #include "pthread.h"



// #define PRINT(string,...) fprintf(stderr, "[object_detector->%s()] " string,__FUNCTION__ , ##__VA_ARGS__)
// #if OBJECT_DETECTOR_VERBOSE
// #define VERBOSE_PRINT PRINT
// #else
// #define VERBOSE_PRINT(...)
// #endif

// static pthread_mutex_t mutex;

// #ifndef COLOR_OBJECT_DETECTOR_FPS1
// #define COLOR_OBJECT_DETECTOR_FPS1 0 ///< Default FPS (zero means run at camera fps)
// #endif
// #ifndef COLOR_OBJECT_DETECTOR_FPS2
// #define COLOR_OBJECT_DETECTOR_FPS2 0 ///< Default FPS (zero means run at camera fps)
// #endif

// // Filter Settings (defined in the bebop_course_orangeavoid.xml )
// uint8_t cod_lum_min1 = 0;
// uint8_t cod_lum_max1 = 0;
// uint8_t cod_cb_min1 = 0;
// uint8_t cod_cb_max1 = 0;
// uint8_t cod_cr_min1 = 0;
// uint8_t cod_cr_max1 = 0;

// uint8_t cod_lum_min2 = 0;
// uint8_t cod_lum_max2 = 0;
// uint8_t cod_cb_min2 = 0;
// uint8_t cod_cb_max2 = 0;
// uint8_t cod_cr_min2 = 0;
// uint8_t cod_cr_max2 = 0;

// bool cod_draw1 = false;
// bool cod_draw2 = false;

// // define global variables
// struct color_object_t {
//   int32_t x_c;
//   int32_t y_c;
//   uint32_t color_count;
//   bool updated;
// };
// struct color_object_t global_filters[2];

// // Function
// uint32_t find_object_centroid(struct image_t *img, int32_t* p_xc, int32_t* p_yc, bool draw,
//                               uint8_t lum_min, uint8_t lum_max,
//                               uint8_t cb_min, uint8_t cb_max,
//                               uint8_t cr_min, uint8_t cr_max);

// /*
//  * object_detector
//  * @param img - input image to process
//  * @param filter - which detection filter to process
//  * @return img
//  */
 
 
// static struct image_t *object_detector(struct image_t *img, uint8_t filter)
// {
//   uint8_t lum_min, lum_max;
//   uint8_t cb_min, cb_max;
//   uint8_t cr_min, cr_max;
//   bool draw;

//   switch (filter){
//     case 1:
//       lum_min = cod_lum_min1;
//       lum_max = cod_lum_max1;
//       cb_min = cod_cb_min1;
//       cb_max = cod_cb_max1;
//       cr_min = cod_cr_min1;
//       cr_max = cod_cr_max1;
//       draw = cod_draw1;
//       break;
//     case 2:
//       lum_min = cod_lum_min2;
//       lum_max = cod_lum_max2;
//       cb_min = cod_cb_min2;
//       cb_max = cod_cb_max2;
//       cr_min = cod_cr_min2;
//       cr_max = cod_cr_max2;
//       draw = cod_draw2;
//       break;
//     default:
//       return img;
//   };

//   int32_t x_c, y_c;

//   // Filter and find centroid
//   //uint32_t count = 0;
//   uint32_t count = find_object_centroid(img, &x_c, &y_c, draw, lum_min, lum_max, cb_min, cb_max, cr_min, cr_max);
//   //VERBOSE_PRINT("Color count %d: %u, threshold %u, x_c %d, y_c %d\n", camera, object_count, count_threshold, x_c, y_c);
//   //VERBOSE_PRINT("centroid %d: (%d, %d) r: %4.2f a: %4.2f\n", camera, x_c, y_c,
//    //     hypotf(x_c, y_c) / hypotf(img->w * 0.5, img->h * 0.5), RadOfDeg(atan2f(y_c, x_c)));

//   pthread_mutex_lock(&mutex);
//   global_filters[filter-1].color_count = count;
//   global_filters[filter-1].x_c = x_c;
//   global_filters[filter-1].y_c = y_c;
//   global_filters[filter-1].updated = true;
//   pthread_mutex_unlock(&mutex);

//   return img;
// }

// struct image_t *object_detector1(struct image_t *img, uint8_t camera_id);
// struct image_t *object_detector1(struct image_t *img, uint8_t camera_id __attribute__((unused)))
// {
//   return object_detector(img, 1);
// }

// struct image_t *object_detector2(struct image_t *img, uint8_t camera_id);
// struct image_t *object_detector2(struct image_t *img, uint8_t camera_id __attribute__((unused)))
// {
//   return object_detector(img, 2);
// }



// //define some new variables
// #ifndef OPENCVTHEO_FPS
// #define OPENCVTHEO_FPS 0       ///< Default FPS (zero means run at camera fps)
// #endif

// // Function that call c++ function in opencv_theo.cpp
// struct image_t *opencv_func(struct image_t *img, uint8_t camera_id);
// struct image_t *opencv_func(struct image_t *img, uint8_t camera_id)
// {

//   if (img->type == IMAGE_YUV422) {
//     // Call OpenCV (C++ from paparazzi C function)
//     opencv_theo((char *) img->buf, img->w, img->h);
//   }
// // opencv_example(NULL, 10,10);

//   return NULL;
// }

// //init function
// void color_object_detector_init(void){
  
//   //initialise callback of opencv function  ///// UNCOMMENT THIS FOR USING OPITICAL FLOW DEFINED IN OPENCV_THEO.CPP /////////
//   //cv_add_to_device(&OPENCVTHEO_CAMERA, opencv_func, OPENCVTHEO_FPS, 0);  
//   //as before
//   memset(global_filters, 0, 2*sizeof(struct color_object_t));
//   pthread_mutex_init(&mutex, NULL);

// #ifdef COLOR_OBJECT_DETECTOR_CAMERA1
// #ifdef COLOR_OBJECT_DETECTOR_LUM_MIN1
//   cod_lum_min1 = COLOR_OBJECT_DETECTOR_LUM_MIN1;
//   cod_lum_max1 = COLOR_OBJECT_DETECTOR_LUM_MAX1;
//   cod_cb_min1 = COLOR_OBJECT_DETECTOR_CB_MIN1;
//   cod_cb_max1 = COLOR_OBJECT_DETECTOR_CB_MAX1;
//   cod_cr_min1 = COLOR_OBJECT_DETECTOR_CR_MIN1;
//   cod_cr_max1 = COLOR_OBJECT_DETECTOR_CR_MAX1;
// #endif
// #ifdef COLOR_OBJECT_DETECTOR_DRAW1
//   cod_draw1 = COLOR_OBJECT_DETECTOR_DRAW1;
// #endif

//   cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA1, object_detector1, COLOR_OBJECT_DETECTOR_FPS1, 0);
// #endif

// #ifdef COLOR_OBJECT_DETECTOR_CAMERA2
// #ifdef COLOR_OBJECT_DETECTOR_LUM_MIN2
//   cod_lum_min2 = COLOR_OBJECT_DETECTOR_LUM_MIN2;
//   cod_lum_max2 = COLOR_OBJECT_DETECTOR_LUM_MAX2;
//   cod_cb_min2 = COLOR_OBJECT_DETECTOR_CB_MIN2;
//   cod_cb_max2 = COLOR_OBJECT_DETECTOR_CB_MAX2;
//   cod_cr_min2 = COLOR_OBJECT_DETECTOR_CR_MIN2;
//   cod_cr_max2 = COLOR_OBJECT_DETECTOR_CR_MAX2;
// #endif
// #ifdef COLOR_OBJECT_DETECTOR_DRAW2
//   cod_draw2 = COLOR_OBJECT_DETECTOR_DRAW2;
// #endif

//   cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA2, object_detector2, COLOR_OBJECT_DETECTOR_FPS2, 1);
// #endif
// }

// /*
//  * find_object_centroid
//  *
//  * Finds the centroid of pixels in an image within filter bounds.
//  * Also returns the amount of pixels that satisfy these filter bounds.
//  *
//  * @param img - input image to process formatted as YUV422.
//  * @param p_xc - x coordinate of the centroid of color object
//  * @param p_yc - y coordinate of the centroid of color object
//  * @param lum_min - minimum y value for the filter in YCbCr colorspace
//  * @param lum_max - maximum y value for the filter in YCbCr colorspace
//  * @param cb_min - minimum cb value for the filter in YCbCr colorspace
//  * @param cb_max - maximum cb value for the filter in YCbCr colorspace
//  * @param cr_min - minimum cr value for the filter in YCbCr colorspace
//  * @param cr_max - maximum cr value for the filter in YCbCr colorspace
//  * @param draw - whether or not to draw on image
//  * @return number of pixels of image within the filter bounds.
//  */
// uint32_t find_object_centroid(struct image_t *img, int32_t* p_xc, int32_t* p_yc, bool draw,
//                               uint8_t lum_min, uint8_t lum_max,
//                               uint8_t cb_min, uint8_t cb_max,
//                               uint8_t cr_min, uint8_t cr_max)
// {
//   //uint32_t cnt = 0;
//   //uint32_t tot_x = 0;
//   //uint32_t tot_y = 0;
//   uint8_t *buffer = img->buf;

//   // Declare cropped_height before using it
//   uint16_t cropped_width = img->w * 0.80;

//   // Define the collision area parameters (center coordinates, width, and height)
//   int collision_center_x = img->w / 2;  // Center of the image in the x-axis
//   int collision_center_y = img->h / 2;  // Center of the image in the y-axis
//   int collision_width = img->w * 0.20;         // Width of the collision area
//   int collision_height = img->h * 0.20;       // Height of the collision area

//   // Go through all the pixels and crop the right part
//   for (uint16_t y = 0; y < img->h; y++) {
//       for (uint16_t x = 0; x < img->w; x++) {
//           if (x >= cropped_width) {
//               uint8_t *yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y component of the pixel
//               *yp = 0;  // Set Y component to 0 (black)
//               // Set U and V components to mid-values (128 for neutral color)
//               buffer[y * 2 * img->w + 2 * x] = 128;  // U component
//               buffer[y * 2 * img->w + 2 * x + 2] = 128;  // V component
//           }
//       }
//   }


//   // // Go through all the pixels
//   // for (uint16_t y = 0; y < img->h; y++) {
//   //   for (uint16_t x = 0; x < img->w; x ++) {
//   //     // Check if the color is inside the specified values
//   //     uint8_t *yp, *up, *vp;
//   //     if (x % 2 == 0) {
//   //       // Even x
//   //       up = &buffer[y * 2 * img->w + 2 * x];      // U
//   //       yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y1
//   //       vp = &buffer[y * 2 * img->w + 2 * x + 2];  // V
//   //       //yp = &buffer[y * 2 * img->w + 2 * x + 3]; // Y2
//   //     } else {
//   //       // Uneven x
//   //       up = &buffer[y * 2 * img->w + 2 * x - 2];  // U
//   //       //yp = &buffer[y * 2 * img->w + 2 * x - 1]; // Y1
//   //       vp = &buffer[y * 2 * img->w + 2 * x];      // V
//   //       yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y2
//   //     }
//   //     if ( (*yp >= lum_min) && (*yp <= lum_max) &&
//   //          (*up >= cb_min ) && (*up <= cb_max ) &&
//   //          (*vp >= cr_min ) && (*vp <= cr_max )) {
//   //       cnt ++;
//   //       tot_x += x;
//   //       tot_y += y;
//   //       //if (draw){
//   //         //*yp = 255;  // make pixel brighter in image
//   //       //}
//   //     }
//   //   }
//   // }
//   // if (cnt > 0) {
//   //   *p_xc = (int32_t)roundf(tot_x / ((float) cnt) - img->w * 0.5f);
//   //   *p_yc = (int32_t)roundf(img->h * 0.5f - tot_y / ((float) cnt));
//   // } else {
//   //   *p_xc = 0;
//   //   *p_yc = 0;
//   // }

//   // Go through all the pixels and filter 
//   for (uint16_t y = 0; y < img->h; y++) {
//       for (uint16_t x = 0; x < img->w; x++) {
//           // Check if the color is inside the specified values
//           uint8_t *yp, *up, *vp;
//           if (x % 2 == 0) {
//               // Even x
//               up = &buffer[y * 2 * img->w + 2 * x];      // U
//               yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y1
//               vp = &buffer[y * 2 * img->w + 2 * x + 2];  // V
//               //yp = &buffer[y * 2 * img->w + 2 * x + 3]; // Y2
//           } else {
//               // Uneven x
//               up = &buffer[y * 2 * img->w + 2 * x - 2];  // U
//               //yp = &buffer[y * 2 * img->w + 2 * x - 1]; // Y1
//               vp = &buffer[y * 2 * img->w + 2 * x];      // V
//               yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y2
//           }
//           /*
//           if ((*yp >= lum_min) && (*yp <= lum_max) &&
//               (*up >= cb_min) && (*up <= cb_max) &&
//               (*vp >= cr_min) && (*vp <= cr_max)) {
//               cnt++;
//               tot_x += x;
//               tot_y += y;
//               if (draw) {
//                   *yp = 187;
//                   *up = 135;
//                   *vp = 176;  // make pixel pink in image
//               } 
//           } */
//           // Check if the color is blue or black  
//           if (((38 < *yp && *yp < 70) && (80 < *up && *up < 150) && (80 < *vp && *vp < 150)) ||   // le premier c noir le deuxieme c bleu
//               ((70 < *yp && *yp < 90) && (152 < *up && *up < 163) && (90 < *vp && *vp < 100))) {
//               // Set the entire row to black
//               for (uint16_t x_row = 0; x_row < img->w; x_row++) {
//                   uint8_t *yp_row, *up_row, *vp_row;
//                   if (x_row % 2 == 0) {
//                       // Even x
//                       up_row = &buffer[y * 2 * img->w + 2 * x_row];      // U
//                       yp_row = &buffer[y * 2 * img->w + 2 * x_row + 1];  // Y1
//                       vp_row = &buffer[y * 2 * img->w + 2 * x_row + 2];  // V
//                       //yp_row = &buffer[y * 2 * img->w + 2 * x_row + 3]; // Y2
//                   } else {
//                       // Uneven x
//                       up_row = &buffer[y * 2 * img->w + 2 * x_row - 2];  // U
//                       //yp_row = &buffer[y * 2 * img->w + 2 * x_row - 1]; // Y1
//                       vp_row = &buffer[y * 2 * img->w + 2 * x_row];      // V
//                       yp_row = &buffer[y * 2 * img->w + 2 * x_row + 1];  // Y2
//                   }
//                   *yp_row = 0;
//                   *up_row = 128;
//                   *vp_row = 128;
//               }
//               break;  // Exit the loop since the entire row has been processed
//           }
//       }

//   }

  
//   // instead of counting the orange pixels, check if the pixels inside the Collision area are all black 
//   // Define a flag to indicate if all pixels in the collision area are black
//   int all_black_column = 700000; // = 1: for clarity used very big number

//   // Calculate the y-coordinate of the row inside the collision area
//   int row_y = collision_center_y;

//   // Go through all the pixels in the row inside the collision area
//   for (int x = collision_center_x - collision_width / 2; x < collision_center_x + collision_width / 2; x++) {
//       // Check if the pixel is within the image boundaries
//       if (x >= 0 && x < img->w && row_y >= 0 && row_y < img->h) {
//           // Access the pixel color values
//           uint8_t *yp = &buffer[row_y * img->w + x];  // Y (grayscale value)
//           if (*yp =0){
//             all_black_column = 500;
//             PRINT("black pixel");
//             break;
//           }
//           // Check if the pixel is black (Y value is 0)
//           else if (*yp != 0) {
//               // If any pixel is not black, set the flag to false and break the loop
//               all_black_column = 300;
//               // Print a message indicating that a colored pixel is found inside the column
//               PRINT("found colored pixel in row inside collision area");
//               break;
//           }
//       }
//       else{
//         PRINT("no pixels inside image boundaries");
//       }
//     }

//   return all_black_column;
// }




// //periodic function that will send the information to the orange_avoider

// void color_object_detector_periodic(void)
// {
  
//   static struct color_object_t local_filters[2];
//   pthread_mutex_lock(&mutex);
//   memcpy(local_filters, global_filters, 2*sizeof(struct color_object_t));
//   pthread_mutex_unlock(&mutex);

//   if(local_filters[0].updated){
//     AbiSendMsgVISUAL_DETECTION(COLOR_OBJECT_DETECTION1_ID, local_filters[0].x_c, local_filters[0].y_c,
//         0, 0, local_filters[0].color_count, 0);  //we only need the information about if all pixels inside the Collision Area are black (no obstacle)
//     local_filters[0].updated = false;
//   }
//   if(local_filters[1].updated){
//     AbiSendMsgVISUAL_DETECTION(COLOR_OBJECT_DETECTION2_ID, local_filters[1].x_c, local_filters[1].y_c,
//         0, 0, local_filters[1].color_count, 1);
//     local_filters[1].updated = false;
//   }
// }

/**
* @file modules/computer_vision/cv_detect_object.h
* Assumes the object consists of a continuous color and checks
* if you are over the defined object or not
*/


// Own header
//#include "modules/computer_vision/cv_detect_color_object.h"
#include "modules/computer_vision/cv.h"
#include "modules/core/abi.h"
#include "std.h"


#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "pthread.h"






#define PRINT(string,...) fprintf(stderr, "[object_detector->%s()] " string,__FUNCTION__ , ##__VA_ARGS__)
#if OBJECT_DETECTOR_VERBOSE
#define VERBOSE_PRINT PRINT
#else
#define VERBOSE_PRINT(...)
#endif


static pthread_mutex_t mutex;


#ifndef COLOR_OBJECT_DETECTOR_FPS1
#define COLOR_OBJECT_DETECTOR_FPS1 0 ///< Default FPS (zero means run at camera fps)
#endif
#ifndef COLOR_OBJECT_DETECTOR_FPS2
#define COLOR_OBJECT_DETECTOR_FPS2 0 ///< Default FPS (zero means run at camera fps)
#endif


// Filter Settings (defined in the bebop_course_orangeavoid.xml )
uint8_t cod_lum_min1 = 0;
uint8_t cod_lum_max1 = 0;
uint8_t cod_cb_min1 = 0;
uint8_t cod_cb_max1 = 0;
uint8_t cod_cr_min1 = 0;
uint8_t cod_cr_max1 = 0;


uint8_t cod_lum_min2 = 0;
uint8_t cod_lum_max2 = 0;
uint8_t cod_cb_min2 = 0;
uint8_t cod_cb_max2 = 0;
uint8_t cod_cr_min2 = 0;
uint8_t cod_cr_max2 = 0;


bool cod_draw1 = false;
bool cod_draw2 = false;


// define global variables
struct color_object_t {
 int32_t x_c;
 int32_t y_c;
 uint32_t color_count;
 bool updated;
};
struct color_object_t global_filters[2];


// Function
// uint32_t find_object_centroid(struct image_t *img, int32_t* p_xc, int32_t* p_yc, bool draw,
//                               uint8_t lum_min, uint8_t lum_max,
//                               uint8_t cb_min, uint8_t cb_max,
//                               uint8_t cr_min, uint8_t cr_max);


uint32_t find_object_centroid(struct image_t *img);                     


/*
* object_detector
* @param img - input image to process
* @param filter - which detection filter to process
* @return img
*/
static struct image_t *object_detector(struct image_t *img, uint8_t filter)
{
 uint8_t lum_min, lum_max;
 uint8_t cb_min, cb_max;
 uint8_t cr_min, cr_max;
 bool draw;


 switch (filter){
   case 1:
     lum_min = cod_lum_min1;
     lum_max = cod_lum_max1;
     cb_min = cod_cb_min1;
     cb_max = cod_cb_max1;
     cr_min = cod_cr_min1;
     cr_max = cod_cr_max1;
     draw = cod_draw1;
     break;
   case 2:
     lum_min = cod_lum_min2;
     lum_max = cod_lum_max2;
     cb_min = cod_cb_min2;
     cb_max = cod_cb_max2;
     cr_min = cod_cr_min2;
     cr_max = cod_cr_max2;
     draw = cod_draw2;
     break;
   default:
     return img;
 };


 int32_t x_c, y_c;




 uint32_t count = find_object_centroid(img);






 pthread_mutex_lock(&mutex);
 global_filters[filter-1].color_count = count;
 global_filters[filter-1].x_c = x_c;
 global_filters[filter-1].y_c = y_c;
 global_filters[filter-1].updated = true;
 pthread_mutex_unlock(&mutex);


 return img;
}


struct image_t *object_detector1(struct image_t *img, uint8_t camera_id);
struct image_t *object_detector1(struct image_t *img, uint8_t camera_id __attribute__((unused)))
{
 return object_detector(img, 1);
}


struct image_t *object_detector2(struct image_t *img, uint8_t camera_id);
struct image_t *object_detector2(struct image_t *img, uint8_t camera_id __attribute__((unused)))
{
 return object_detector(img, 2);
}






//define some new variables
#ifndef OPENCVTHEO_FPS
#define OPENCVTHEO_FPS 0       ///< Default FPS (zero means run at camera fps)
#endif




void color_object_detector_init(void){
  //initialise callback of opencv function  ///// UNCOMMENT THIS FOR USING OPITICAL FLOW DEFINED IN OPENCV_THEO.CPP /////////
 //cv_add_to_device(&OPENCVTHEO_CAMERA, opencv_func, OPENCVTHEO_FPS, 0); 
 //as before
 memset(global_filters, 0, 2*sizeof(struct color_object_t));
 pthread_mutex_init(&mutex, NULL);


#ifdef COLOR_OBJECT_DETECTOR_CAMERA1
#ifdef COLOR_OBJECT_DETECTOR_LUM_MIN1
 cod_lum_min1 = COLOR_OBJECT_DETECTOR_LUM_MIN1;
 cod_lum_max1 = COLOR_OBJECT_DETECTOR_LUM_MAX1;
 cod_cb_min1 = COLOR_OBJECT_DETECTOR_CB_MIN1;
 cod_cb_max1 = COLOR_OBJECT_DETECTOR_CB_MAX1;
 cod_cr_min1 = COLOR_OBJECT_DETECTOR_CR_MIN1;
 cod_cr_max1 = COLOR_OBJECT_DETECTOR_CR_MAX1;
#endif
#ifdef COLOR_OBJECT_DETECTOR_DRAW1
 cod_draw1 = COLOR_OBJECT_DETECTOR_DRAW1;
#endif


 cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA1, object_detector1, COLOR_OBJECT_DETECTOR_FPS1, 0);
#endif


#ifdef COLOR_OBJECT_DETECTOR_CAMERA2
#ifdef COLOR_OBJECT_DETECTOR_LUM_MIN2
 cod_lum_min2 = COLOR_OBJECT_DETECTOR_LUM_MIN2;
 cod_lum_max2 = COLOR_OBJECT_DETECTOR_LUM_MAX2;
 cod_cb_min2 = COLOR_OBJECT_DETECTOR_CB_MIN2;
 cod_cb_max2 = COLOR_OBJECT_DETECTOR_CB_MAX2;
 cod_cr_min2 = COLOR_OBJECT_DETECTOR_CR_MIN2;
 cod_cr_max2 = COLOR_OBJECT_DETECTOR_CR_MAX2;
#endif
#ifdef COLOR_OBJECT_DETECTOR_DRAW2
 cod_draw2 = COLOR_OBJECT_DETECTOR_DRAW2;
#endif


 cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA2, object_detector2, COLOR_OBJECT_DETECTOR_FPS2, 1);
#endif
}






uint32_t find_object_centroid(struct image_t *img) {
 // Define minimum and maximum threshold values for black or dark gray pixels
 uint8_t y_min_threshold_black = 10; // Adjust this threshold value as needed
 uint8_t y_max_threshold_black = 110; // Adjust this threshold value as needed
 uint8_t cr_min_threshold_black = 110; // Adjust this threshold value as needed
 uint8_t cr_max_threshold_black = 135; // Adjust this threshold value as needed
 uint8_t cb_min_threshold_black = 125; // Adjust this threshold value as needed
 uint8_t cb_max_threshold_black = 150; // Adjust this threshold value as needed


 // Define minimum and maximum threshold values for blue pixels
 uint8_t y_min_threshold_blue = 60; // Adjust this threshold value as needed 
 uint8_t y_max_threshold_blue = 100; // Adjust this threshold value as needed
 uint8_t cr_min_threshold_blue = 145; // Adjust this threshold value as needed
 uint8_t cr_max_threshold_blue = 165; // Adjust this threshold value as needed
 uint8_t cb_min_threshold_blue = 85; // Adjust this threshold value as needed
 uint8_t cb_max_threshold_blue = 105; // Adjust this threshold value as needed


 // Define minimum and maximum threshold values for green pixels (belonging to plants)
 uint8_t y_min_threshold_green = 50;  // Adjust this threshold value as needed
 uint8_t y_max_threshold_green = 150; // Adjust this threshold value as needed
 uint8_t cr_min_threshold_green = 80;  // Adjust this threshold value as needed
 uint8_t cr_max_threshold_green = 180;  // Adjust this threshold value as needed
 uint8_t cb_min_threshold_green = 0; // Adjust this threshold value as needed
 uint8_t cb_max_threshold_green = 125;  // Adjust this threshold value as needed




 // Declare cropped_height before using it
 uint8_t *buffer = img->buf;
 uint16_t cropped_width = img->w * 0.80;


 // // Define the collision area parameters (center coordinates, width, and height)
 // int collision_center_x = img->w / 2;  // Center of the image in the x-axis
 // int collision_center_y = img->h / 2;  // Center of the image in the y-axis
 // int collision_width = img->w * 0.20;       // Width of the collision area
 // int collision_height = img->h * 0.20;    // Height of the collision area


 // Go through all the pixels and crop the right part










 // for (uint16_t y = 0; y < img->h; y++) {
 //    for (uint16_t x = 0; x < img->w; x++) {
 //        if (x >= cropped_width) {
 //            uint8_t *yp = buffer + (y * 2 * img->w + 2 * x + 1);  // Y component of the pixel
 //            *yp = 0;  // Set Y component to 0 (black)
 //            // Set U and V components to mid-values (128 for neutral color)
 //            *(buffer + y * 2 * img->w + 2 * x) = 128;  // U component
 //            *(buffer + y * 2 * img->w + 2 * x + 2) = 128;  // V component
 //        }
 //    }
 // }








 int plant = 0;
 int cnt_green = 0;
 int cnt = 0;
 int cnt_max = 0;
 int close_height = img->h;
 int close_width = img->w;
 int new_dir = close_height / 2;
 int obs_size = 0;
  // Iterate through all pixels in the image
 for (uint16_t y = 0; y < img->h; y++) {
   int obstacle = 1;
     for (uint16_t x = 0; x < close_width/2; x++) {
         // Check if the color is inside the specified values
         uint8_t *yp, *up, *vp;
         if (x % 2 == 0) {
             // Even x
             up = &buffer[y * 2 * img->w + 2 * x];      // U
             yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y1
             vp = &buffer[y * 2 * img->w + 2 * x + 2];  // V
             //yp = &buffer[y * 2 * img->w + 2 * x + 3]; // Y2
         } else {
             // Uneven x
             up = &buffer[y * 2 * img->w + 2 * x - 2];  // U
             //yp = &buffer[y * 2 * img->w + 2 * x - 1]; // Y1
             vp = &buffer[y * 2 * img->w + 2 * x];      // V
             yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y2
         }


           if ((20 < *yp && *yp < 80) && (80 < *up && *up < 150) && (80 < *vp && *vp < 150)){  // (y_min_threshold_black < *yp && *yp < y_max_threshold_black) && (cr_min_threshold_black < *up && *up < cr_max_threshold_black) && (cb_min_threshold_black < *vp && *vp < cb_max_threshold_black)
               cnt++;
               *yp = 255;
               obstacle = 0;
               break;


           } else if ((0 < *yp && *yp < 255) && (0 < *up && *up < 255) && (0 < *vp && *vp < 255)) { // (y_min_threshold_blue < *yp && *yp < y_max_threshold_blue) && (cr_min_threshold_blue < *up && *up < cr_max_threshold_blue) && (cb_min_threshold_blue < *vp && *vp < cb_max_threshold_blue)
               cnt++;
               *yp = 255;
               obstacle = 0;
               break;
           }
        
     }
     for (uint16_t x = close_width/2; x < close_width; x++) {
       uint8_t *yp, *up, *vp;
         if (x % 2 == 0) {
             // Even x
             up = &buffer[y * 2 * img->w + 2 * x];      // U
             yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y1
             vp = &buffer[y * 2 * img->w + 2 * x + 2];  // V
             //yp = &buffer[y * 2 * img->w + 2 * x + 3]; // Y2
         } else {
             // Uneven x
             up = &buffer[y * 2 * img->w + 2 * x - 2];  // U
             //yp = &buffer[y * 2 * img->w + 2 * x - 1]; // Y1
             vp = &buffer[y * 2 * img->w + 2 * x];      // V
             yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y2
         }
         if ((y_min_threshold_green < *yp && *yp < y_max_threshold_green) && (cr_min_threshold_green < *up && *up < cr_max_threshold_green) && (cb_min_threshold_green < *vp && *vp < cb_max_threshold_green)){  // (20 < *yp && *yp < 80) && (80 < *up && *up < 150) && (80 < *vp && *vp < 150)
               cnt_green++;
               *yp = 255;
           }
        
     }


     if (obstacle || cnt_green >= 30) {
         if (cnt > cnt_max) {
             cnt_max = cnt;
             if (y % 2 == 0) {
               new_dir = y - cnt / 2;}
             else{new_dir = y - (cnt-1) / 2;}
         }
         obs_size++;
         if (cnt_green >= 29){
               cnt = 0;
               obs_size = obs_size - 1;
         }




     } else {
         if (obs_size > 35) {
             // If the obstacle has a width greater than 35 pixels, neglect it (far away)
             cnt = 1;
         } else if (1 <= obs_size && obs_size <= 35) {
             // If the obstacle has a width between 1 and 35 pixels, color the row in red
             cnt += obs_size;
         }
         obs_size = 0;
     }
     cnt_green = 0;
 }


 // printf("Value of new_dir: %d\n", new_dir); // You may uncomment this line if needed


 return new_dir;
}




















//periodic function that will send the information to the orange_avoider


void color_object_detector_periodic(void)
{
  static struct color_object_t local_filters[2];
 pthread_mutex_lock(&mutex);
 memcpy(local_filters, global_filters, 2*sizeof(struct color_object_t));
 pthread_mutex_unlock(&mutex);


 if(local_filters[0].updated){
   AbiSendMsgVISUAL_DETECTION(COLOR_OBJECT_DETECTION1_ID, local_filters[0].x_c, local_filters[0].y_c,
       0, 0, local_filters[0].color_count, 0);  //we only need the information about if all pixels inside the Collision Area are black (no obstacle)
   local_filters[0].updated = false;
 }
 if(local_filters[1].updated){
   AbiSendMsgVISUAL_DETECTION(COLOR_OBJECT_DETECTION2_ID, local_filters[1].x_c, local_filters[1].y_c,
       0, 0, local_filters[1].color_count, 1);
   local_filters[1].updated = false;
 }
}









