/*!
 * \file movement.h
 * \author Jake Drahos
 * 
 * Simple interface to movement.
 *
 * Each MOVE_ function will cause the robot to move the
 * specified distance or turn the specified angle. Several
 * sensor events can cause the robot to abort the movement.
 * For example, a bumper hit will abort the movement.
 *
 * Each command returns the distace actually moved. It can be
 * determined whether or not the move was aborted by comparing this
 * distance with the requested distance. If the full distance was not
 * moved, check the current sensor state to see which sensor
 * triggered the abort.
 * 
 * Each command must be passed an already initialized oi_t
 * struct pointer. Initialize this before using these functions.
 */ 


#ifndef MOVEMENT_H_
#define MOVEMENT_H_


#include <stdint.h>
#include "open_interface.h"

/*!
 * 
 * Move forward up to distance millimeters.
 *
 * \param distance Millimeters to try to move
 * \param sensor_data Initialized oi_t
 * \return The distance, in millimeters, actually moved.
 */
uint16_t MOVE_forward(oi_t * sensor_data, uint16_t distance);

/*!
 *
 * Move backward up to distance millimeters.
 *
 * \param distance Millimeters to try to move
 * \param sensor_data Initialized oi_t
 * \return The distance, in millimeters, actually moved.
 */
uint16_t MOVE_backward(oi_t * sensor_data, uint16_t distance);

/*!
 * 
 * Turn left (counterclockwise) up to the specified angle.
 *
 * \param degrees Degrees to attempt to turn
 * \param sensor_data Initialized oi_t
 * \return Angle actually rotated
 */
uint16_t MOVE_turn_left(oi_t * sensor_data, uint16_t degrees);

/*!
 * 
 * Turn right (clockwise) up to the specified angle.
 *
 * \param degrees Degrees to attempt to turn
 * \param sensor_data Initialized oi_t
 * \return Angle actually rotated
 */
uint16_t MOVE_turn_right(oi_t * sensor_data, uint16_t degrees);

/*! Subtracted from each turn. Can be used to tweak the distance to
 * get an exact 90 degree turn. Not currently used
 */
#define TURN_TRIM 0

#endif /* MOVEMENT_H_ */