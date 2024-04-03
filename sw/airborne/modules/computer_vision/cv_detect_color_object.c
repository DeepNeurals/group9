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


uint32_t find_object_centroid(struct image_t *img);                 	 

 

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




// the count variable represents the row in the image corresponding to the optimal heading angle for the robot to avoid colliding with an obstacle
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
#define OPENCVTHEO_FPS 0   	///< Default FPS (zero means run at camera fps)
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
    // Define minimum and maximum threshold values for dark gray pixels (edges of the ground)
    uint8_t y_min_threshold_black = 10; // Adjust this threshold value as needed
    uint8_t y_max_threshold_black = 110; // Adjust this threshold value as needed
    uint8_t cr_min_threshold_black = 110; // Adjust this threshold value as needed
    uint8_t cr_max_threshold_black = 135; // Adjust this threshold value as needed
    uint8_t cb_min_threshold_black = 125; // Adjust this threshold value as needed
    uint8_t cb_max_threshold_black = 150; // Adjust this threshold value as needed

    // Define minimum and maximum threshold values for blue pixels (edges of the ground)
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



    uint8_t *buffer = img->buf;



    int cnt_green = 0; // counting the number of green pixels inside a row
    int cnt = 0; // counting the number of uninterrupted rows without obstacle
    int cnt_max = 0; // greatest number of rows uninterrupted without obstacle
    int close_height = img->h;
    int close_width = img->w;
    int new_dir = close_height / 2; // new_dir is the row corresponding to the optimal heading angle. It is initialized to be in the center of the image
    int obs_size = 0; // counting the number of uninterrupted rows with obstacle
    
 
    // Iterate through all the rows of the image
    for (uint16_t y = 0; y < img->h; y++) {
   	 	int obstacle = 1;
    
    // Iterate through the first half of columns (bottom)
    for (uint16_t x = 0; x < close_width/2; x++) {
      	// Extract the YUV values of the pixel
      	uint8_t *yp, *up, *vp;
      	if (x % 2 == 0) {
          	// Even x
          	up = &buffer[y * 2 * img->w + 2 * x];  	// U
          	yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y1
          	vp = &buffer[y * 2 * img->w + 2 * x + 2];  // V
          	//yp = &buffer[y * 2 * img->w + 2 * x + 3]; // Y2
      	} else {
          	// Uneven x
          	up = &buffer[y * 2 * img->w + 2 * x - 2];  // U
          	//yp = &buffer[y * 2 * img->w + 2 * x - 1]; // Y1
          	vp = &buffer[y * 2 * img->w + 2 * x];  	// V
          	yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y2
      	}
    
    
    
    // If the pixel is between the thresholds of either blue or dark gray, then it means that there is not obstacle in that row of pixel
        	if ( (y_min_threshold_black < *yp && *yp < y_max_threshold_black) && (cr_min_threshold_black < *up && *up < cr_max_threshold_black) && (cb_min_threshold_black < *vp && *vp < cb_max_threshold_black)){  
       		 // one more row without obstacle
            	cnt++;
            	// no obstacle
            	obstacle = 0;
            	break;

        	} else if ((y_min_threshold_blue < *yp && *yp < y_max_threshold_blue) && (cr_min_threshold_blue < *up && *up < cr_max_threshold_blue) && (cb_min_threshold_blue < *vp && *vp < cb_max_threshold_blue)) {
       		 // one more row without obstacle
            	cnt++;
            	// no obstacle
            	obstacle = 0;
            	break;
        	}   
  	}
 	 
 	 
  	 
  	// Iterate through the second half of columns (top)
  	for (uint16_t x = close_width/2; x < close_width; x++) {
    	uint8_t *yp, *up, *vp;

      // Extract the YUV values of the pixel
      	if (x % 2 == 0) {
          	// Even x
          	up = &buffer[y * 2 * img->w + 2 * x];  	// U
          	yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y1
          	vp = &buffer[y * 2 * img->w + 2 * x + 2];  // V
          	//yp = &buffer[y * 2 * img->w + 2 * x + 3]; // Y2
      	} else {
          	// Uneven x
          	up = &buffer[y * 2 * img->w + 2 * x - 2];  // U
          	//yp = &buffer[y * 2 * img->w + 2 * x - 1]; // Y1
          	vp = &buffer[y * 2 * img->w + 2 * x];  	// V
          	yp = &buffer[y * 2 * img->w + 2 * x + 1];  // Y2
      	}


      // If the pixel is between the thresholds, then it means that it might belong to a plant
      	if ((y_min_threshold_green < *yp && *yp < y_max_threshold_green) && (cr_min_threshold_green < *up && *up < cr_max_threshold_green) && (cb_min_threshold_green < *vp && *vp < cb_max_threshold_green)){  
     		 // one more pixel potentially belonging to a plant
            	cnt_green++;
        	}
   		 
  	}



    // Checking if an obstacle is detected in that row (either "regular" obstacle or a plant)
   	 if (obstacle || cnt_green >= 30) {
   		 // The previous obstacle-free zone is compared to the greatest stored
   		 if (cnt > cnt_max) {
       		 cnt_max = cnt;
       		 
          	// Update the new optimal heading angle row if necessary
          	if (y % 2 == 0) {
       		   new_dir = y - cnt / 2;}
          	else{new_dir = y - (cnt-1) / 2;}
   		 }
   		 obs_size++;
     	 
        // Necessary to dissociate plants from "regular" obstacles
      	if (cnt_green >= 30){
            	cnt = 0;
            	obs_size = obs_size - 1;
      	}

    
    // In the case where no obstacle is detected in the current row
   	 } else {
   		 // If the previous row belonged to an obstacle
   		 if (obs_size > 35) {
       		 // Only consider that last obstacle if it has a width greater than 35 pixels
       		 cnt = 1;
   		 } else if (1 <= obs_size && obs_size <= 35) {
       		 // Otherwise, don't consider it (probably far away)
       		 cnt += obs_size;
   		 }
   		 obs_size = 0;
   	 }
  	// When switching to next row reinitialize the green counter to zero
  	cnt_green = 0;
    }

 
  return new_dir;

}




//periodic function that will send the optimal heading angle row to orange_avoider.c

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



