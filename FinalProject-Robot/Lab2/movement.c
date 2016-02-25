/*
 * movement.c
 *
 * Created: 1/29/2015 2:01:00 PM
 *  Author: drahos
 */ 

#include "movement.h"

/* Enable or disable abort sources */
#define ENABLE_BUMPERS 1
#define ENABLE_FRONTCLIFF 1
#define ENABLE_CLIFF 1
#define ENABLE_WHEELDROP 1
#define ENABLE_WHITE 1
#define ENABLE_FRONTRIGHT 1

#define WHITE_THRESHOLD 850

/* Returns true if a sensor is triggered that should abort the move */
static uint8_t checkAbort(oi_t * sensor_data)
{
	return (((sensor_data->bumper_left || sensor_data->bumper_right) && ENABLE_BUMPERS) ||
			((sensor_data->cliff_frontright || sensor_data->cliff_frontleft) && ENABLE_FRONTCLIFF) ||
			((sensor_data->cliff_left || sensor_data->cliff_right) && ENABLE_CLIFF) ||
			((sensor_data->wheeldrop_caster || sensor_data->wheeldrop_left || sensor_data->wheeldrop_right) && ENABLE_WHEELDROP) ||
			(((sensor_data->cliff_left_signal > WHITE_THRESHOLD) || (sensor_data->cliff_right_signal > WHITE_THRESHOLD)) && ENABLE_WHITE) ||
			(((sensor_data->cliff_frontleft_signal > WHITE_THRESHOLD) || (sensor_data->cliff_frontright_signal > WHITE_THRESHOLD)) && ENABLE_FRONTRIGHT));
}

/* Move forward until distance is moved or an abort reason is encountered */
uint16_t MOVE_forward(oi_t * sensor_data, uint16_t distance)
{
	uint16_t distance_moved = 0;
	
	oi_update(sensor_data);
	oi_set_wheels(100, 100);
	while (distance_moved < distance)
	{
		/* Poll OI to see if we are done yet */
		oi_update(sensor_data);
		distance_moved += sensor_data->distance;
		
		/* Abort the move if a sensor is triggered */
		if(checkAbort(sensor_data))
		{
			oi_set_wheels(0,0);
			return distance_moved;
		}
	}
	oi_set_wheels(0,0);
	
	return distance_moved;
}

/* Move backward the given distance. Will not abort on sensors, allowing us
 * to back out of abort situations */
uint16_t MOVE_backward(oi_t * sensor_data, uint16_t distance)
{
	int16_t distance_moved = 0;
	int16_t actual_distance = 0 - distance;
	
	oi_update(sensor_data);
	oi_set_wheels(-100, -100);
	while (distance_moved > actual_distance)
	{
		/* Poll OI to see if we are done yet */
		oi_update(sensor_data);
		distance_moved += sensor_data->distance;
	}
	oi_set_wheels(0,0);
	
	return distance_moved;
}

/* Turn right. Will not trigger aborts, so we can turn away from abort situations */
uint16_t MOVE_turn_right(oi_t * sensor_data, uint16_t degrees)
{
	int16_t degrees_turned = 0;
	int16_t actual_degrees = 0 - (degrees - TURN_TRIM);
	
	oi_update(sensor_data);
	oi_set_wheels(-150, 150);
	while (degrees_turned > actual_degrees)
	{
		/* Poll OI to see if we are done yet */
		oi_update(sensor_data);
		degrees_turned += sensor_data->angle;
	}
	oi_set_wheels(0,0);
	
	return degrees_turned;
}

/* Turn left. Will not trigger aborts, so we can turn away from abort situations */
uint16_t MOVE_turn_left(oi_t * sensor_data, uint16_t degrees)
{
	int16_t degrees_turned = 0;
	
	oi_update(sensor_data);
	oi_set_wheels(150, -150);
	while (degrees_turned < (degrees- TURN_TRIM))
	{
		/* Poll OI to see if we are done yet */
		oi_update(sensor_data);
		degrees_turned += sensor_data->angle;
	}
	oi_set_wheels(0,0);
	
	return degrees_turned;	
}
