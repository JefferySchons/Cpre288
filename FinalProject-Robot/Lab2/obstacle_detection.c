/*
 * obstacle_detection.c
 *
 * Created: 4/9/2015 1:41:01 PM
 * \author Jake Drahos
 */ 

#include "movement.h"
#include "lcd.h"
#include "stdint.h"
#include "util.h"
#include "avr_usart.h"
#include "ir.h"
#include "ping.h"
#include "servo.h"
#include "obstacle_detection.h"

static uint16_t median(uint16_t * array, uint8_t size)
{
	uint8_t i;
	uint8_t j;
	uint16_t tmp;
	
	for(i = 0; i < size-1; i++)
	{
		for(j = i+1; j < size; j++)
		{
			if(array[j] < array[i])
			{
				tmp = array[j];
				array[j] = array[i];
				array[i] = tmp;
			}
		}
	}
	return array[size/2];
}

uint8_t OBSTACLE_SweepAndDetect(obstacle_t * obstacles, uint8_t max_obstacles, uint8_t max_dist)
{
	/* Used for for loops */
	uint16_t i;
	uint16_t j;
	
	/* Temporary buffer for serial printing */
	char format_buffer[80];
	
	/* Distances measured by sensors */
	uint16_t ping_distance;
	uint16_t ir_distance_array[10];
	uint16_t ir_distance;
	
	/* Obstacle storage */
	uint8_t obstacle_index = 0;
	obstacle_t * current_obstacle;
	
	/* Arrays of measured sensor values */
	uint8_t ping_array[DEGREES_IN_FRONT / STEP_SIZE];
	uint8_t ir_array[DEGREES_IN_FRONT / STEP_SIZE];
	
	/* Tracks the position at which the most recent object was entered */
	uint8_t current_object_start = NO_CURRENT_OBJECT;
	
	/* Reset servo blockingly */
	SERVO_SetDegreesBlocking(0);
	
	for(i = 0; i < DEGREES_IN_FRONT; i+= STEP_SIZE)
	{
		SERVO_SetDegreesNonBlocking(i);
		wait_ms(50);
		ping_distance = PING_MeasureCM();
		
		/* Median of 5 samples */
		for(j = 0; j < 10; j++)
		{
			ir_distance_array[j] = IR_GetDistBlocking();
		}
		ir_distance = median(ir_distance_array, 10);
		
		/* Handle min/max situations */
		if ((ir_distance > 2000) ||(ir_distance == 0))
			ir_distance = 2000;
		
		if (ping_distance > 200)
			ping_distance = 200;
		
		/* Debug printing over bluetooth while scanning */
		sprintf(format_buffer,
				"Degrees: %d\tPing Distance: %dcm\tIR Distance: %dmm\t\r\n",
				i,
				ping_distance,
				ir_distance);
		
		USART_Puts(format_buffer);
		
		/* Store for calculation */
		ir_array[i/STEP_SIZE] = ir_distance / 10; /* mm -> cm */
		ping_array[i/STEP_SIZE] = ping_distance;
	}
	
	/* Prepare for object detection during data analysis */
	current_object_start = NO_CURRENT_OBJECT;
	obstacle_index = 0;
	
	for (i = 0; i < DEGREES_IN_FRONT / STEP_SIZE; i++)
	{
		if (current_object_start == NO_CURRENT_OBJECT)
		{
			/* If no current object and new object starts. Do not go over max_obstacles */
			if ( (obstacle_index < max_obstacles) && (ir_array[i] < max_dist) && (ping_array[i] < max_dist))
				current_object_start = i;
		}
		else /* In an object */
		{
			/* If there is no longer an object blocking IR */
			if (ir_array[i] >= max_dist)
			{
				/* Set the current_obstacle convenience pointer */
				current_obstacle = obstacles + obstacle_index;
				
				/* Convert measured values to degrees and store */
				current_obstacle->start_angle = current_object_start * STEP_SIZE;
				current_obstacle->end_angle = i * STEP_SIZE;
				current_obstacle->center_angle = (current_object_start + (i - current_object_start)) * STEP_SIZE;
				
				/* Store sonar value of center angle */
				current_obstacle->sonar_dist = ping_array[(current_obstacle->center_angle / STEP_SIZE)];
				
				/* Calculate angular size */
				current_obstacle->angular_size = current_obstacle->end_angle - current_obstacle->start_angle;
				
				/* Use trig to find the linear width (2dtan(1/2(theta))*/
				current_obstacle->linear_size = 2 * current_obstacle->sonar_dist * tan(DEG_TO_RAD((current_obstacle->angular_size)/2));
				
				current_object_start = NO_CURRENT_OBJECT;
				obstacle_index++;
			}
		}
	}
	
	/* Reset servo */
	SERVO_SetDegreesNonBlocking(0);
	
	/* Return number of obstacles seen */
	return obstacle_index;
}

int8_t OBSTACLE_FindSmallest(obstacle_t * obstacles, uint8_t num_obstacles)
{
	/* Used in loops */
	uint8_t i;
	
	/* Track smallest */
	uint8_t current_smallest_obstacle = 0;
	
	/* Error condition */
	if(num_obstacles == 0)
		return -1;
	
	/* No current smallest obstacle yet */
	current_smallest_obstacle = 0;
	for (i = 0; i < num_obstacles; i++)
	{				
		/* Find smallest */
		if(obstacles[i].linear_size < obstacles[current_smallest_obstacle].linear_size)
			current_smallest_obstacle = i;
	}
	return current_smallest_obstacle;
}
