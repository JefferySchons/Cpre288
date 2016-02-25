/*!
 * \file obstacle_detection.h
 *
 *  \author Jake Drahos
 *
 * Sensor sweep and obstacle detection API.
 */ 


#ifndef OBSTACLE_DETECTION_H_
#define OBSTACLE_DETECTION_H_


#include <math.h>
#include <stdio.h>

/*! Convert degrees to radians for trig functions */
#define DEG_TO_RAD(x) ((x / 360.0f) * 6.2820f)

/*! Sweep state meaning that no object is currently seen. */
#define NO_CURRENT_OBJECT 0xff

/*! Configuration option. Resolution in degrees */
#define STEP_SIZE 1

/*! Configuration option. FOV in degrees */
#define DEGREES_IN_FRONT 180

/*! Obstacle struct. An array of these stores obstacle data */
typedef struct
{
	/*! Sonar distance at center of obstacle */
	uint16_t sonar_dist;

	/*! Start angle of obstacle measured with IR */
	uint16_t start_angle;
	
	/*! End angle of obstacle measured with IR */
	uint16_t end_angle;
	
	/*! Center angle of obstacle measured with IR */
	uint16_t center_angle;
	
	/*! Angular size obstacle measured with IR */
	uint16_t angular_size;
	
	/*! Linear size, calculated with trigonometry using angles and sonar distance*/
	uint16_t linear_size;
} obstacle_t;


/*!
 * Perform a sweep with the sensors, then create an array of all found obstacles.
 *
 * This function will take a while to perform the sweep, then populate the given
 * obstacles array with the detected obstacles. See the obstacl_t struct.
 
 * \param obstacles Array to populate after sweep
 * \param max_obstacles Maximum number of obstacles the array can hold
 * \param max_dist Ignore obstacles beyond max_dist. This allows us to see obstacles against a backdrop
 * \return Number of obstacles found
 */
uint8_t OBSTACLE_SweepAndDetect(obstacle_t * obstacles, uint8_t max_obstacles, uint8_t max_dist);

/*! Find the smallest obstacle in the array
 *
 * \param obstacles Array to search
 * \param num_obstacles Size of the array
 * \return Index of the smallest obstacle
 */
int8_t OBSTACLE_FindSmallest(obstacle_t * obastacles, uint8_t num_obstacles);

#endif /* OBSTACLE_DETECTION_H_ */