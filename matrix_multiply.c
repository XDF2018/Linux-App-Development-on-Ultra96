//----------------------------------------------------------------------------
//
//       ** **        **          **  ****      **  **********  ********** �
//      **   **        **        **   ** **     **  **              **
//     **     **        **      **    **  **    **  **              **
//    **       **        **    **     **   **   **  *********       **
//   **         **        **  **      **    **  **  **              **
//  **           **        ****       **     ** **  **              **
// **  .........  **        **        **      ****  **********      **
//    ...........
//                                    Reach Further�
//
//----------------------------------------------------------------------------
//
// This design is the property of Avnet.  Publication of this
// design is not authorized without written consent from Avnet.
//
// Please direct any questions to the Avnet Technical Community:
//    http://www.ultra96.com
//
// Disclaimer:
//    Avnet, Inc. makes no warranty for the use of this code or design.
//    This code is provided  "As Is". Avnet, Inc assumes no responsibility for
//    any errors, which may appear in this code, nor does it make a commitment
//    to update the information contained herein. Avnet, Inc specifically
//    disclaims any implied warranties of fitness for a particular purpose.
//                     Copyright(c) 2018 Avnet, Inc.
//                             All rights reserved.
//
//----------------------------------------------------------------------------
//
// Create Date:         Sep 11, 2018
// Design Name:         Ultra96 Matrix Multiply Example Application
// Module Name:         mmult.c
// Project Name:        Ultra96 Example Code
// Target Devices:      Xilinx Zynq UltraScale+ MPSoC
// Hardware Boards:     Ultra96
//
// Tool versions:       Xilinx Vivado 2018.2
//
// Description:         This example computes the results for a matrix
//                      multiply function.
//
// Dependencies:
//
// Revision:            Sep 11, 2018: 1.00 Initial version
//
//----------------------------------------------------------------------------

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/select.h>
#include <signal.h>

/* String formats used to build the file name path to specific LEDS
 * instances. */
#define FILE_FORMAT_LEDS_PATH          "/sys/class/leds"
#define FILE_FORMAT_LEDS_BRIGHTNESS    "/brightness"
#define USER_LED1_OFFSET               2
#define USER_LED2_OFFSET               3
#define USER_LED3_OFFSET               5 // These two get swapped to make the increment
#define USER_LED4_OFFSET               4 // pattern look more like a spinning wheel

// Array size to access for matrix multiply function.
#define DATA_SIZE 32

volatile sig_atomic_t flag = 0;

void sigint_handler(int sig)
{
	flag=1;
}

void handler (int sig)
{
  printf ("exiting...(%d)n", sig);
  exit (0);
}

void perror_exit (char *error)
{
  perror (error);
  handler (9);
}

void mmult_sw( int *in1,   // Input matrix 1
                int *in2,   // Input matrix 2
                int *out,   // Output matrix (out = A x B)
                int dim     // Size of one dimension of matrix
              )
{
    // Performs matrix multiplication out = in1 x in2
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            for (int k = 0; k < dim; k++)
            {
                out[i * dim + j] += in1[i * dim + k] * in2[k * dim  + j];
            }
        }
    }
}

int matrix_multiply(void)
{
    int dim = DATA_SIZE;
    size_t matrix_size_bytes = sizeof(int) * DATA_SIZE * DATA_SIZE;

    // Allocate memory
    int *in1 = (int *) malloc(matrix_size_bytes);
    int *in2 = (int *) malloc(matrix_size_bytes);
    int *sw_result = (int *) malloc(matrix_size_bytes);

    // Creates test data
    for(int index = 0; index < dim * dim; index++)
    {
        in1[index] = rand() % dim;
        in2[index] = rand() % dim;
        sw_result[index] = 0;
    }

    // Function call to perform matrix multiplication
    mmult_sw(in1, in2, sw_result, dim);
    // printf("Maxtrix Multiply Results Completed!\n");

    //Release Memory
    free(in1);
    free(in2);
    free(sw_result);

    return 0;
}

int set_next_rotate_pattern(int user_led)
{
    const int char_buf_size = 80;
    char led_brightness_setting_off[5];
    char led_brightness_setting_on[5];
    int test_result = 0;
    char formatted_file_name[char_buf_size];

    FILE  *fp_led1;
    FILE  *fp_led2;
    FILE  *fp_led3;
    FILE  *fp_led4;

    // Used to turn LED OFF.
    strcpy(led_brightness_setting_on, "1");
    // Used to turn LED OFF.
    strcpy(led_brightness_setting_off, "0");

    // Open the LEDs brightness property so that it can be read/written.
    test_result = snprintf(formatted_file_name, (char_buf_size - 1), FILE_FORMAT_LEDS_PATH"/ds%d"FILE_FORMAT_LEDS_BRIGHTNESS, USER_LED1_OFFSET);
    if ((test_result < 0) ||
        (test_result == (char_buf_size - 1)))
    {
        printf("Error formatting string, check the LEDs specified\r\n");
        printf(formatted_file_name);
        return -1;
    }
    fp_led1 = fopen(formatted_file_name, "r+");
    if (fp_led1 == 0)
    {
        // There was an error opening the LED /sys/class/leds/ file.
        return -1;
    }

    // Open the LEDs brightness property so that it can be read/written.
    test_result = snprintf(formatted_file_name, (char_buf_size - 1), FILE_FORMAT_LEDS_PATH"/ds%d"FILE_FORMAT_LEDS_BRIGHTNESS, USER_LED2_OFFSET);
    if ((test_result < 0) ||
        (test_result == (char_buf_size - 1)))
    {
        printf("Error formatting string, check the LEDs specified\r\n");
        printf(formatted_file_name);
        return -1;
    }
    fp_led2 = fopen(formatted_file_name, "r+");
    if (fp_led2 == 0)
    {
        // There was an error opening the LED /sys/class/leds/ file.
        return -1;
    }

    // Open the LEDs brightness property so that it can be read/written.
    test_result = snprintf(formatted_file_name, (char_buf_size - 1), FILE_FORMAT_LEDS_PATH"/ds%d"FILE_FORMAT_LEDS_BRIGHTNESS, USER_LED3_OFFSET);
    if ((test_result < 0) ||
        (test_result == (char_buf_size - 1)))
    {
        printf("Error formatting string, check the LEDs specified\r\n");
        printf(formatted_file_name);
        return -1;
    }
    fp_led3 = fopen(formatted_file_name, "r+");
    if (fp_led3 == 0)
    {
        // There was an error opening the LED /sys/class/leds/ file.
        return -1;
    }

    // Open the LEDs brightness property so that it can be read/written.
    test_result = snprintf(formatted_file_name, (char_buf_size - 1), FILE_FORMAT_LEDS_PATH"/ds%d"FILE_FORMAT_LEDS_BRIGHTNESS, USER_LED4_OFFSET);
    if ((test_result < 0) ||
        (test_result == (char_buf_size - 1)))
    {
        printf("Error formatting string, check the LEDs specified\r\n");
        printf(formatted_file_name);
        return -1;
    }
    fp_led4 = fopen(formatted_file_name, "r+");
    if (fp_led4 == 0)
    {
        // There was an error opening the LED /sys/class/leds/ file.
        return -1;
    }

    // Write next rotate pattern to LEDs.
    if (user_led == 1)
    {
        // Now turn the specified LED ON.
        fwrite(&led_brightness_setting_on, sizeof(char), 1, fp_led1);
    }
    else
    {
        // Now turn LED1 off cause it was not specified;
        fwrite(&led_brightness_setting_off, sizeof(char), 1, fp_led1);
    }
    fflush(fp_led1);

    // Write next rotate pattern to LEDs.
    if (user_led == 2)
    {
        // Now turn the specified LED ON.
        fwrite(&led_brightness_setting_on, sizeof(char), 1, fp_led2);
    }
    else
    {
        // Now turn LED1 off cause it was not specified;
        fwrite(&led_brightness_setting_off, sizeof(char), 1, fp_led2);
    }
    fflush(fp_led2);

    // Write next rotate pattern to LEDs.
    if (user_led == 3)
    {
        // Now turn the specified LED ON.
        fwrite(&led_brightness_setting_on, sizeof(char), 1, fp_led3);
    }
    else
    {
        // Now turn LED1 off cause it was not specified;
        fwrite(&led_brightness_setting_off, sizeof(char), 1, fp_led3);
    }
    fflush(fp_led3);

    // Write next rotate pattern to LEDs.
    if (user_led == 4)
    {
        // Now turn the specified LED ON.
        fwrite(&led_brightness_setting_on, sizeof(char), 1, fp_led4);
    }
    else
    {
        // Now turn LED4 off cause it was not specified;
        fwrite(&led_brightness_setting_off, sizeof(char), 1, fp_led4);
    }
    fflush(fp_led4);

	// Close the GPIO value property files.
	fclose(fp_led1);
	fclose(fp_led2);
	fclose(fp_led3);
	fclose(fp_led4);

	test_result = 0;

	return test_result;
}

int main (int argc, char *argv[])
{
    struct input_event ev;
    int exit_requested = 0;
    int test_result = 0;
    int led_loop_count = 0;
    int max_loop_count = 20;
    int user_led = 0;
    int fd;
    int rv;
    FILE  *fp;
    char default_device[256] = "/dev/input/event0";
    char leds_setting[5];
    char *input_device = NULL;
    fd_set set;
    struct timeval timeout;

    // Setup and check to see if another input device was specified to override
    // the default.
    if (argv[1] == NULL)
    {
    	input_device = default_device;
        printf("Please specify (on the command line) the path to the input device event interface device if desired\n");
    }
    else
    {
    	input_device = argv[1];
    }

    // Check to see if current user even has access to the input event device.
    if ((getuid ()) != 0)
    {
        printf ("non-root user detected, this may not work...n");
    }

    // Display the lab name in the application banner.
    printf("*********************************************************\n");
    printf("*                                                       *\n");
    printf("*   Linux Debug on Ultra96: Lab 1 - MMult Application   *\n");
    printf("*                                                       *\n");
    printf("*                                                       *\n");
    printf("*   Press SW4 to exit                                   *\n");
    printf("*                                                       *\n");
    printf("*********************************************************\n");

    // Set the trigger property of the LED DS2.
    fp = fopen("/sys/class/leds/ds2/trigger", "r+");
    if (fp == NULL)
    {
        printf("Error opening /sys/class/leds/ds2/trigger node\n");
    }
    else
    {
        // Set the trigger property to "none".
        strcpy(leds_setting, "none");
        fwrite(&leds_setting, sizeof(char), 4, fp);
        fflush(fp);
        fclose(fp);
    }

    // Set the trigger property of the LED DS3.
    fp = fopen("/sys/class/leds/ds3/trigger", "r+");
    if (fp == NULL)
    {
        printf("Error opening /sys/class/leds/ds3/trigger node\n");
    }
    else
    {
        // Set the trigger property to "none".
        strcpy(leds_setting, "none");
        fwrite(&leds_setting, sizeof(char), 4, fp);
        fflush(fp);
        fclose(fp);
    }

    // Set the trigger property of the LED DS4.
    fp = fopen("/sys/class/leds/ds4/trigger", "r+");
    if (fp == NULL)
    {
        printf("Error opening /sys/class/leds/ds4/trigger node\n");
    }
    else
    {
        // Set the trigger property to "none".
        strcpy(leds_setting, "none");
        fwrite(&leds_setting, sizeof(char), 4, fp);
        fflush(fp);
        fclose(fp);
    }

    // Set the trigger property of the LED DS5.
    fp = fopen("/sys/class/leds/ds5/trigger", "r+");
    if (fp == NULL)
    {
        printf("Error opening /sys/class/leds/ds5/trigger node\n");
    }
    else
    {
        // Set the trigger property to "none".
        strcpy(leds_setting, "none");
        fwrite(&leds_setting, sizeof(char), 4, fp);
        fflush(fp);
        fclose(fp);
    }

    // Open the input event device so that events can be read from it.
    if ((fd = open (input_device, O_RDONLY)) == -1)
    {
        printf ("%s is not a valid device\n", input_device);
    }
    else
    {
    	printf("Opened device %s successfully\n", input_device);
    }

    // Continue to read the input from Push Buttons SW4 and if an event
    // happens, update the exit_requested flag.
    test_result = 0;
    user_led = 1;
    led_loop_count = 0;

    while ((exit_requested == 0) &&
           (test_result == 0) &&
		   (led_loop_count < max_loop_count))
    {
    	FD_ZERO(&set); /* clear the set */
    	FD_SET(fd, &set); /* add our file descriptor to the set */

    	timeout.tv_sec = 0;
    	timeout.tv_usec = 10000;

    	rv = 0;

    	rv = select(fd + 1, &set, NULL, NULL, &timeout);

    	if(rv == -1)
    	{
    		// An error occurred during select.
    		perror("select");
    	}
    	else if(rv == 0)
    	{
    		// A timeout occurred during select.
    		//printf("timeout");
    	}
    	else
    	{
    		// Select indicated there was data to read.
    		read(fd, &ev, sizeof(struct input_event));
    	}

    	if(ev.type == 1)
    	{
    		if(ev.value == 0)
    		{
    			printf(" : [key %i]\n ", ev.code);
    			printf("Push on SW4 button detected, exiting...");
    			exit_requested = 1;
    		}
    	}

    	// Perform the matrix multiplication operation.
    	for (int count = 0; count < 1024; count++)
    	{
    		matrix_multiply();
    	}

    	printf("1024 Matrix Multiply Results Completed!\n");

    	// Set the next output pattern on the user LEDs.
    	if (user_led == 5)
    	{
    		user_led = 1;
    	}
    	test_result = set_next_rotate_pattern(user_led);
    	user_led += 1;

    	signal(SIGINT, sigint_handler);
    	if(flag)
    	{
    		printf("\r\n<ctrl>-c Pressed!  Exit gracefully.\r\n");
    		exit_requested = 1;
    	}

    	led_loop_count = 1;
    }

    printf("Exiting application...");
    return 0;
}

