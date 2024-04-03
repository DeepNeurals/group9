#include "modules/orange_avoider/orange_avoider.h"
#include "firmwares/rotorcraft/navigation.h"
#include "generated/airframe.h"
#include "state.h"

#include "modules/core/abi.h"
#include <time.h>
#include <stdio.h>

#include <unistd.h> // Include the header for sleep()

#define NAV_C // needed to get the nav functions like Inside...
#include "generated/flight_plan.h"

#define ORANGE_AVOIDER_VERBOSE TRUE

#define PRINT(string,...) fprintf(stderr, "[orange_avoider->%s()] " string,__FUNCTION__ , ##__VA_ARGS__)
#if ORANGE_AVOIDER_VERBOSE
#define VERBOSE_PRINT PRINT
#else
#define VERBOSE_PRINT(...)
#endif


//function declarations
static uint8_t moveWaypointForward(uint8_t waypoint, float distanceMeters);
static uint8_t calculateForwards(struct EnuCoor_i *new_coor, float distanceMeters);
static uint8_t moveWaypoint(uint8_t waypoint, struct EnuCoor_i *new_coor);
static uint8_t increase_nav_heading(float incrementDegrees);
static uint8_t chooseRandomIncrementAvoidance(void);


//define possible navigation states
enum navigation_state_t {
  SAFE,
  OBSTACLE_FOUND,
  SEARCH_FOR_SAFE_HEADING,
  OUT_OF_BOUNDS
};


// define and initialise global variables
enum navigation_state_t navigation_state = SEARCH_FOR_SAFE_HEADING;
int32_t color_count = 0;            	// Row on the image corresponding to the optimal heading angle
int16_t obstacle_free_confidence = 0;   // a measure of how certain we are that the way ahead is safe.
float heading_increment = 5.f;      	// heading angle increment [deg]
float maxDistance = 2.25;           	// max waypoint displacement [m]

const int16_t max_trajectory_confidence = 5; // number of consecutive negative object detections to be sure we are obstacle free

/*
 * This next section defines an ABI messaging event (http://wiki.paparazziuav.org/wiki/ABI), necessary
 * any time data calculated in another module needs to be accessed. Including the file where this external
 * data is defined is not enough, since modules are executed parallel to each other, at different frequencies,
 * in different threads. The ABI event is triggered every time new data is sent out, and as such the function
 * defined in this file does not need to be explicitly called, only bound in the init function
 */
#ifndef ORANGE_AVOIDER_VISUAL_DETECTION_ID
#define ORANGE_AVOIDER_VISUAL_DETECTION_ID ABI_BROADCAST
#endif
static abi_event color_detection_ev;
static void color_detection_cb(uint8_t __attribute__((unused)) sender_id,
                           	int16_t __attribute__((unused)) pixel_x, int16_t __attribute__((unused)) pixel_y,
                           	int16_t __attribute__((unused)) pixel_width, int16_t __attribute__((unused)) pixel_height,
                           	int32_t quality, int16_t __attribute__((unused)) extra)
{
  color_count = quality;
}

/*
 * Initialisation function, setting the colour filter, random seed and heading_increment
 */
void orange_avoider_init(void)
{
  // Initialise random values
  srand(time(NULL));
  chooseRandomIncrementAvoidance();

  // bind our colorfilter callbacks to receive the color filter outputs
  AbiBindMsgVISUAL_DETECTION(ORANGE_AVOIDER_VISUAL_DETECTION_ID, &color_detection_ev, color_detection_cb);
}

/*
 * Periodic function, being the autpilot of the drone
 */

void orange_avoider_periodic(void)
{
  // only evaluate our state machine if we are flying
  if(!autopilot_in_flight()){
	return;
  }


  // The threshold helps to determine in what third of the image is the optimal direction located
  uint16_t thresh = front_camera.output_size.h/3;

 
  // If the optimal direction is more or less in the middle then increase the confidence to go forward
  if(thresh <= color_count && color_count <= (thresh*2)){
	obstacle_free_confidence++;
  } else {
  // Otherwise, reduce the confidence to go forward
	obstacle_free_confidence -= 2;  
  }
 
  //bound obstacle_free_confidence
  Bound(obstacle_free_confidence, 0, max_trajectory_confidence);
  VERBOSE_PRINT("Color_count1: %d  confidence1: %d threshold1: %d \n", color_count, obstacle_free_confidence, thresh);

  float moveDistance = fminf(maxDistance, 0.2f * obstacle_free_confidence);

 
 
  switch (navigation_state){
	case SAFE:
  	// Move waypoint forward
  	PRINT("_____________In SAFE mode ______");

  	moveWaypointForward(WP_TRAJECTORY, 1.5f * moveDistance);
  	if (!InsideObstacleZone(WaypointX(WP_TRAJECTORY),WaypointY(WP_TRAJECTORY))){
    	navigation_state = OUT_OF_BOUNDS;
  	} else if (obstacle_free_confidence == 0){
    	navigation_state = OBSTACLE_FOUND;
  	} else {
    	moveWaypointForward(WP_GOAL, moveDistance);
  	}

  	break;
	case OBSTACLE_FOUND:
  	PRINT("_____________In OBSTACLE FOUND MODE ______");
  	waypoint_move_here_2d(WP_GOAL);
  	waypoint_move_here_2d(WP_TRAJECTORY);

  	// the increment is actually fixed to always turn slightly to the left
  	chooseRandomIncrementAvoidance();

  	navigation_state = SEARCH_FOR_SAFE_HEADING;

  	break;

	case SEARCH_FOR_SAFE_HEADING:
  	PRINT("____________SEARCH FOR SAFE HEADING ______");
  	increase_nav_heading(heading_increment);

  	// Only go forward when the confidence is high enough
  	if (obstacle_free_confidence >= 2){
    	navigation_state = SAFE;
  	}
  	break;

	case OUT_OF_BOUNDS:
  	PRINT("____________OUT_OF_BOUNDS______");
  	increase_nav_heading(heading_increment);
  	moveWaypointForward(WP_TRAJECTORY, 1.5f);

  	if (InsideObstacleZone(WaypointX(WP_TRAJECTORY),WaypointY(WP_TRAJECTORY))){
    	// add offset to head back into arena
    	increase_nav_heading(heading_increment);

    	// reset safe counter
    	obstacle_free_confidence = 1;

    	// ensure direction is safe before continuing
    	navigation_state = SEARCH_FOR_SAFE_HEADING;
  	}
  	break;
	default:
  	break;
  }
  return;
}




uint8_t increase_nav_heading(float incrementDegrees)
{
  float new_heading = stateGetNedToBodyEulers_f()->psi + RadOfDeg(incrementDegrees);

  // normalize heading to [-pi, pi]
  FLOAT_ANGLE_NORMALIZE(new_heading);

  // set heading, declared in firmwares/rotorcraft/navigation.h
  nav.heading = new_heading;

  VERBOSE_PRINT("Increasing heading to %f\n", DegOfRad(new_heading));
  return false;
}

/*
 * Calculates coordinates of distance forward and sets waypoint 'waypoint' to those coordinates
 */
uint8_t moveWaypointForward(uint8_t waypoint, float distanceMeters)
{
  struct EnuCoor_i new_coor;
  calculateForwards(&new_coor, distanceMeters);
  moveWaypoint(waypoint, &new_coor);
  return false;
}

/*
 * Calculates coordinates of a distance of 'distanceMeters' forward w.r.t. current position and heading
 */
uint8_t calculateForwards(struct EnuCoor_i *new_coor, float distanceMeters)
{
  float heading  = stateGetNedToBodyEulers_f()->psi;

  // Now determine where to place the waypoint you want to go to
  new_coor->x = stateGetPositionEnu_i()->x + POS_BFP_OF_REAL(sinf(heading) * (distanceMeters));
  new_coor->y = stateGetPositionEnu_i()->y + POS_BFP_OF_REAL(cosf(heading) * (distanceMeters));
  //VERBOSE_PRINT("Calculated %f m forward position. x: %f  y: %f based on pos(%f, %f) and heading(%f)\n", distanceMeters,    
  //          	POS_FLOAT_OF_BFP(new_coor->x), POS_FLOAT_OF_BFP(new_coor->y),
  //        	stateGetPositionEnu_f()->x, stateGetPositionEnu_f()->y, DegOfRad(heading));
  return false;
}

/*
 * Sets waypoint 'waypoint' to the coordinates of 'new_coor'
 */
uint8_t moveWaypoint(uint8_t waypoint, struct EnuCoor_i *new_coor)
{
  //VERBOSE_PRINT("Moving waypoint %d to x:%f y:%f\n", waypoint, POS_FLOAT_OF_BFP(new_coor->x),
  //         	POS_FLOAT_OF_BFP(new_coor->y));
  waypoint_move_xy_i(waypoint, new_coor->x, new_coor->y);
  return false;
}

/*
 * Sets the variable 'heading_increment' randomly positive/negative
 */
uint8_t chooseRandomIncrementAvoidance(void)
{
  heading_increment = -5.f;
 
  return false;
}

