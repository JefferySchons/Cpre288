/*
 * command.c
 *
 * Created: 4/16/2015 2:10:41 PM
 *  Author: drahos
 */ 

#include "avr_usart.h"
#include "movement.h"
#include "open_interface.h"
#include "stdio.h"
#include "stdlib.h"
#include "lcd.h"
#include "command.h"
#include "obstacle_detection.h"
#include "util.h"

/* Parameters used to avoid buffer overruns during command parsing */
#define MAX_COMMAND_LENGTH 50
#define MAX_TYPE_LENGTH 12
#define MAX_FIELDS 10

/* Threshold to detect either the border or the target spot */
#define WHITE_THRESHOLD 850

/* Sets the size of the array for obstacle storage */
#define MAX_NUMBER_OF_OBSTACLES 20

/* Static helper functions */
/* Parse something that we already know is a command */
static void handleCommand(char * command);
/* Format and send the move result, including abort reason */
static void sendMoveResult(char * cmd, char * cmd_id, uint16_t distance_moved, uint16_t distance_to_move);
/* Perform a scan and respond with the detected objects */
static void doScan(uint16_t cmd_id, uint8_t max_dist);
/* Get the values of the cliff sensors and return them */
static void getSensorData(uint16_t cmd_id);

/* These are declared static mostly because they are big so they are allocated at compile-time */
/* Obstacle storage array */
static obstacle_t obstacles[MAX_NUMBER_OF_OBSTACLES];
/* Buffer used to temporarily store and parse commands */
static char commandBuffer[MAX_COMMAND_LENGTH];
/* Index within the commandBuffer */
static uint8_t byteIndex = 0;

/* OI state */
static oi_t * sensor_data;

/* Mario death music */
#define SONG_0_LENGTH 10
static uint8_t song_0_notes[SONG_0_LENGTH] = {60, 65, 0, 65, 65, 64, 62, 60, 55, 48};
static uint8_t song_0_durations[SONG_0_LENGTH] = {15, 20, 10, 15, 20, 20, 20, 30, 30, 40};

void COMMAND_Initialize()
{
	memset(commandBuffer, 0, MAX_COMMAND_LENGTH);
	sensor_data = oi_alloc();
	oi_init(sensor_data);	
	oi_load_song(0, SONG_0_LENGTH, song_0_notes, song_0_durations);
}

static void handleCommand(char * command) {
	/* Parser variables */
	int fieldCount = 1;
	int currentField = 0;
	int len = 0;
	char * fields[MAX_FIELDS];
	
	/* Multi-purpose for-loop i */
	int i;
	
	/* Distance actually and distance to move */
	int distance_moved;
	uint16_t distance_to_move;
	
	len = strlen(command);
	
	if (command[0] != '!') return; //invalid command


	/* Count the number of fields to prepare the split */
	for(i = 0; i < len; i++) {
		if (command[i] == ',') fieldCount++;
	}

	fields[currentField] = command + 1; //first field begins at command[1]

	/* Split on , into fields[] */
	for(i = 0; i < len; i++) {
		if (command[i] == ',') {
			command[i] = 0; //set the , to an ascii null to let strcmp work
			fields[++currentField] = ((command + i) + 1); //the next field begins at comma + 1
		}
	}

	command[len - 1] = 0;  	// replace stop character with ascii null.
	// The command buffer now holds just \0 separated fields.
	//Each field is pointed to by fields[]
	
	/* The type of the command */
	lprintf(fields[0]);

	/* Find out which kind of command it is, then do the needful.
	 * All of the movement commands are similar: pull out the distance to move,
	 * perform the move, then call sendMoveResult. sendMoveResult takes care
	 * of deciding if there was an abort or not and putting all of that information
	 * into the status string */
	if (strncmp("mvfwd", fields[0], MAX_TYPE_LENGTH) == 0)
	{
		distance_to_move = atoi(fields[2]);
		lprintf("Moving Forward %d", distance_to_move);
		
		distance_moved = MOVE_forward(sensor_data, distance_to_move);
		sendMoveResult(fields[0], fields[1], distance_moved, distance_to_move);
	}
	else if (strncmp("mvrev", fields[0], MAX_TYPE_LENGTH) == 0)
	{
		distance_to_move = atoi(fields[2]);
		lprintf("Moving Reverse %d", distance_to_move);
		
		distance_moved = MOVE_backward(sensor_data, distance_to_move);
		sendMoveResult(fields[0], fields[1], distance_moved, distance_to_move);
	}
	else if (strncmp("rtclk", fields[0], MAX_TYPE_LENGTH) == 0)
	{
		distance_to_move = atoi(fields[2]);
		lprintf("Turning CW %d", distance_to_move);
		
		distance_moved = MOVE_turn_right(sensor_data, distance_to_move);
		sendMoveResult(fields[0], fields[1], distance_moved, distance_to_move);
	}
	else if (strncmp("rtctc", fields[0], MAX_TYPE_LENGTH) == 0)
	{
		distance_to_move = atoi(fields[2]);
		lprintf("Turning CCW %d", distance_to_move);
		
		distance_moved = MOVE_turn_left(sensor_data, distance_to_move);
		sendMoveResult(fields[0], fields[1], distance_moved, distance_to_move);
	}
	else if (strncmp("scan", fields[0], MAX_TYPE_LENGTH) == 0)
	{
		lprintf("Scanning!");
		doScan(atoi(fields[1]), atoi(fields[2]));
	}
	else if (strncmp("sense", fields[0], MAX_TYPE_LENGTH) == 0)
	{
		getSensorData(atoi(fields[1]));
	}
	else if (strncmp("music", fields[0], MAX_TYPE_LENGTH) == 0)
	{
		lprintf("Playing song %d!", atoi(fields[0]));
		oi_play_song(atoi(fields[2]));
		
		/* Also flash the LED while playing music */
		for(i = 0; i < 35; i++)
		{
			oi_set_leds(1,1,1,255);
			wait_ms(50);
			oi_set_leds(0,0,1,0);
			wait_ms(50);
		}
	}
}

static void sendMoveResult(char * cmd, char * cmd_id, uint16_t distance_moved, uint16_t distance_to_move)
{
	uint16_t abort_reason;
	char sprintf_buffer[20];
	
	/* Select abort reason, if we didn't move the full distance */
	if (distance_moved >= distance_to_move)
		abort_reason = ABORT_NO_ABORT;
	else if (sensor_data->bumper_left)
		abort_reason = ABORT_LEFT_BUMPER;
	else if (sensor_data->bumper_right)
		abort_reason = ABORT_RIGHT_BUMPER;	
	else if (sensor_data->wheeldrop_right)
		abort_reason = ABORT_RIGHT_WHEELDROP;
	else if (sensor_data->wheeldrop_left)
		abort_reason = ABORT_LEFT_WHEELDROP;
	else if (sensor_data->wheeldrop_caster)
		abort_reason = ABORT_WHEELDROP_CASTER;
	else if (sensor_data->wall)
		abort_reason = ABORT_WALL;
	else if (sensor_data->virtual_wall)
		abort_reason = ABORT_VIRTUAL_WALL;
	else if (sensor_data->cliff_left)
		abort_reason = ABORT_CLIFF_LEFT;
	else if (sensor_data->cliff_right)
		abort_reason = ABORT_CLIFF_RIGHT;
	else if (sensor_data->cliff_frontleft)
		abort_reason = ABORT_CLIFF_FRONTLEFT;
	else if (sensor_data->cliff_frontright)
		abort_reason = ABORT_CLIFF_FRONTRIGHT;
	else if (sensor_data->cliff_left_signal > WHITE_THRESHOLD)
		abort_reason = ABORT_WHITE_LEFT;
	else if (sensor_data->cliff_right_signal > WHITE_THRESHOLD)
		abort_reason = ABORT_WHITE_RIGHT;
	else if (sensor_data->cliff_frontleft_signal > WHITE_THRESHOLD)
		abort_reason = ABORT_WHITE_FRONTLEFT;
	else if (sensor_data->cliff_frontright_signal > WHITE_THRESHOLD)
		abort_reason = ABORT_CLIFF_FRONTRIGHT;
	else
		abort_reason = ABORT_OTHER_REASON;
		
	/* Check if we need to immediately back up! */
	if ((abort_reason == ABORT_LEFT_BUMPER) || (abort_reason == ABORT_RIGHT_BUMPER)
		|| (abort_reason == ABORT_WALL) || (abort_reason == ABORT_VIRTUAL_WALL))
	{
		distance_moved -= MOVE_backward(sensor_data, 20);
	}
	
	/* Format and print */
	sprintf(sprintf_buffer, "!%s,%d,%d,%d$", cmd, atoi(cmd_id), distance_moved, abort_reason);
	USART_Puts(sprintf_buffer);
}

static void doScan(uint16_t cmd_id, uint8_t max_dist)
{
	uint8_t num_obstacles = 0;
	uint8_t i = 0;
	char sprintf_buffer[50];
	
	/* Get an obstacle array */
	num_obstacles = OBSTACLE_SweepAndDetect(obstacles, MAX_NUMBER_OF_OBSTACLES, max_dist);
	
	/* Format and send a status for each obstacle */
	for(i = 0; i < num_obstacles; i++)
	{
		lprintf("Sending statuses for %d obstacles", num_obstacles);
		sprintf(sprintf_buffer, "!scan,%d,%d,%d,%d$", cmd_id, obstacles[i].center_angle, obstacles[i].sonar_dist, obstacles[i].linear_size);
		USART_Puts(sprintf_buffer);
		
		/* Throttle to avoid overfilling the tx buffer */
		wait_ms(5);
	}
}

static void getSensorData(uint16_t cmd_id)
{
	char sprintf_buffer[50];
	oi_update(sensor_data);
	
	/* Grab all 4 cliff sensor signals and send a status string */
	sprintf(sprintf_buffer, "!sense,%d,0,%d$", cmd_id, sensor_data->cliff_left_signal);
	USART_Puts(sprintf_buffer);
	
	sprintf(sprintf_buffer, "!sense,%d,1,%d$", cmd_id, sensor_data->cliff_frontleft_signal);
	USART_Puts(sprintf_buffer);
	
	sprintf(sprintf_buffer, "!sense,%d,2,%d$", cmd_id, sensor_data->cliff_frontright_signal);
	USART_Puts(sprintf_buffer);
	
	sprintf(sprintf_buffer, "!sense,%d,3,%d$", cmd_id, sensor_data->cliff_right_signal);
	USART_Puts(sprintf_buffer);
}

void COMMAND_CheckAndParse() {
	char byte;

	/* Loop until we have emptied hte rx buffer into the parse buffer */
	for (;;) {
		if (USART_Getc(&byte) != STATUS_SUCCESS)
		{
			break; /* No more bytes in rx buffer */
		}

		/* Restart the parse buffer upon start character or maximum command length */
		if ((byteIndex == MAX_COMMAND_LENGTH) || (byte == '!')) {
			byteIndex = 0;
			memset(commandBuffer, 0, MAX_COMMAND_LENGTH);
		}

		commandBuffer[byteIndex] = byte;
		byteIndex ++;

		/* When we see a stop character, parse the parse buffer and handle the command */
		if (byte == '$') 
		{
			handleCommand(commandBuffer);
		}
	}
}