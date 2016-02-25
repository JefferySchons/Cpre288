/*!
 * \file command.h
 *
 * Created: 4/16/2015 2:10:50 PM
 *  Author: drahos
 */ 


#ifndef COMMAND_H_
#define COMMAND_H_

/*! Initialize command handling. 
 * 
 * This calls oi_init, so
 * the robot must be powered on.
 */
void COMMAND_Initialize();

/*! Check UART and parse commands
 * 
 * This function is mostly asynchronous. It checks
 * the UART receive buffer for characters, then
 * adds them to a temporary parse buffer. It looks at
 * the parse buffer for command strings. If a string
 * found, it will parse it and perform the action. 
 * 
 * COMMAND_CheckAndParse() only blocks if it performs an
 * action that blocks.
 */
void COMMAND_CheckAndParse();

/*! Abort reasons

 * These abort reasons correspond to the integer string keys used
 * by PyRobot to translate the abort reasons into a human-readable
 * abort message.
 */
enum ABORT_REASON
{
	ABORT_NO_ABORT = 0,
	
	ABORT_LEFT_BUMPER = 1,
	ABORT_RIGHT_BUMPER = 2,
	ABORT_RIGHT_WHEELDROP = 3,
	ABORT_LEFT_WHEELDROP = 4,
	ABORT_WHEELDROP_CASTER = 5,
	ABORT_WALL = 6,
	ABORT_CLIFF_LEFT = 7,
	ABORT_CLIFF_RIGHT = 8,
	ABORT_CLIFF_FRONTLEFT = 9,
	ABORT_CLIFF_FRONTRIGHT = 10,
	ABORT_VIRTUAL_WALL = 11,
	ABORT_WHITE_LEFT = 12,
	ABORT_WHITE_RIGHT = 13,
	ABORT_WHITE_FRONTLEFT = 14,
	ABORT_WHITE_FRONTRIGHT = 15,
	
	/*! Miscellaneous abort reason */
	ABORT_OTHER_REASON = 100
};



#endif /* COMMAND_H_ */