/**
 * @file
 * @brief Dummy main.c to test compilation and linkage of ECU library. 
 * Calls and uses common API functions.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-01
 * @copyright Copyright (c) 2024
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

/* Application that uses ECU library. */
#include "led_strip.h"
#include "led.h"

/* ECU library under test. */
#include "ecu/asserter.h"
#include "ecu/timer.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("ecu/main.c")

/*------------------------------------------------------------*/
/*--------------------- FILE SCOPE DEFINES -------------------*/
/*------------------------------------------------------------*/

#define SECONDS_TO_CLOCK(x_) \
    ((ecu_tick_t)((x_) * (CLOCKS_PER_SEC)))

/*------------------------------------------------------------*/
/*---------------------- FILE SCOPE TYPES --------------------*/
/*------------------------------------------------------------*/

enum timer_id
{
    ONE_SECOND_TIMER = ECU_USER_OBJECT_ID_BEGIN,
    FIVE_SECOND_TIMER,
    TEN_SECOND_TIMER,
    /******************/
    NUMBER_OF_TIMER_IDS
};

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Sets up scanf() to return immediately, not echo
 * the character onto the terminal, and not require Enter.
 */
static void setup_terminal(void);

/**
 * @brief Restores terminal to original settings when program 
 * exits.
 */
static void restore_terminal(void);

/**
 * @brief Callback that executes when timer expires.
 * 
 * @param timer Timer that expired.
 * @param obj The timer's ID.
 */
static bool timer_expired(struct ecu_timer *timer, void *obj);

/**
 * @brief Turns the supplied "LED" on.
 * 
 * @param obj The LED's ID.
 */
static void turn_led_on(void *obj);

/**
 * @brief Turns the supplied "LED" off.
 * 
 * @param obj The LED's ID.
 */
static void turn_led_off(void *obj);

/*------------------------------------------------------------*/
/*------------------- FILE SCOPE VARIABLES -------------------*/
/*------------------------------------------------------------*/

static bool running = true;
static struct termios original_settings;
static const struct led_event BUTTON_PRESS_EVENT = {LED_BUTTON_PRESS_EVENT};
static struct ecu_tlist timers;
static struct ecu_timer timer_1s;
static ecu_object_id timer_1s_id = ONE_SECOND_TIMER;
static struct ecu_timer timer_5s;
static ecu_object_id timer_5s_id = FIVE_SECOND_TIMER;
static struct ecu_timer timer_10s;
static ecu_object_id timer_10s_id = TEN_SECOND_TIMER;
static struct led_strip led_strip1;
static struct led_strip led_strip2;
static struct led led1;
static unsigned int led1_id = 1;
static struct led led2;
static unsigned int led2_id = 2;
static struct led led3;
static unsigned int led3_id = 3;
static struct led led4;
static unsigned int led4_id = 4;
static struct led led5;
static unsigned int led5_id = 5;

/*------------------------------------------------------------*/
/*------------------ STATIC FUNCTION DEFINITIONS -------------*/
/*------------------------------------------------------------*/

static void setup_terminal(void)
{
    static bool setup = false;
    static struct termios new_settings;

    if (!setup)
    {
        tcgetattr(STDIN_FILENO, &original_settings);
        memcpy(&new_settings, &original_settings, sizeof(struct termios));
        new_settings.c_lflag &= ~( ((tcflag_t)(ICANON)) | ((tcflag_t)(ECHO)) );
        new_settings.c_cc[VMIN] = 1;
        new_settings.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
        atexit(&restore_terminal);
        setup = true;
    }
}

static void restore_terminal(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_settings);
    exit(0);
}

static bool timer_expired(struct ecu_timer *timer, void *obj)
{
    (void)timer;
    ECU_RUNTIME_ASSERT( (obj) );
    ecu_object_id id = *((ecu_object_id *)obj);

    switch (id)
    {
        case ONE_SECOND_TIMER:
        {
            printf("1 second timer expired!\n");
            break;
        }

        case FIVE_SECOND_TIMER:
        {
            printf("5 second timer expired!\n");
            break;
        }

        case TEN_SECOND_TIMER:
        {
            printf("10 second timer expired!\n");
            break;
        }

        default:
        {
            ECU_RUNTIME_ASSERT( (false) );
            break;
        }
    }

    return true;
}

static void turn_led_on(void *obj)
{
    ECU_RUNTIME_ASSERT( (obj) );
    unsigned int id = *((unsigned int *)obj);
    printf("LED%u turned on!\n", id);
}

static void turn_led_off(void *obj)
{
    ECU_RUNTIME_ASSERT( (obj) );
    unsigned int id = *((unsigned int *)obj);
    printf("LED%u turned off!\n", id);
}

/*------------------------------------------------------------*/
/*--------------------------- MAIN ---------------------------*/
/*------------------------------------------------------------*/

int main(void)
{
    /* Construct LEDs and LED strips. */
    int c;
    clock_t t_prev;
    clock_t t_current;
    clock_t t_elapsed;
    led_strip_ctor(&led_strip1);
    led_strip_ctor(&led_strip2);
    led_ctor(&led1, &turn_led_on, &turn_led_off, &led1_id);
    led_ctor(&led2, &turn_led_on, &turn_led_off, &led2_id);
    led_ctor(&led3, &turn_led_on, &turn_led_off, &led3_id);
    led_ctor(&led4, &turn_led_on, &turn_led_off, &led4_id);
    led_ctor(&led5, &turn_led_on, &turn_led_off, &led5_id);

    /* Construct timers. */
    ecu_tlist_ctor(&timers);
    ecu_timer_ctor(&timer_1s, &timer_expired, &timer_1s_id);
    ecu_timer_ctor(&timer_5s, &timer_expired, &timer_5s_id);
    ecu_timer_ctor(&timer_10s, &timer_expired, &timer_10s_id);

    /* Create LED strips. */
    led_strip_add(&led_strip1, &led1);
    led_strip_add(&led_strip1, &led2);
    led_strip_add(&led_strip1, &led3);
    led_strip_add(&led_strip2, &led4);
    led_strip_add(&led_strip2, &led5);

    /* Setup timers. */
    ecu_timer_set(&timer_1s, SECONDS_TO_CLOCK(1), ECU_TIMER_TYPE_ONE_SHOT);
    ecu_timer_set(&timer_5s, SECONDS_TO_CLOCK(5), ECU_TIMER_TYPE_ONE_SHOT);
    ecu_timer_set(&timer_10s, SECONDS_TO_CLOCK(10), ECU_TIMER_TYPE_ONE_SHOT);

    /* Setup terminal. */
    setup_terminal();
    printf("Press '1' to toggle LED1.\n");
    printf("Press '2' to toggle LED2.\n");
    printf("Press '3' to toggle LED3.\n");
    printf("Press '4' to toggle LED4.\n");
    printf("Press '5' to toggle LED5.\n");
    printf("Press 'A' to turn LED strip 1 ON.\n");
    printf("Press 'S' to turn LED strip 1 OFF.\n");
    printf("Press 'D' to turn LED strip 2 ON.\n");
    printf("Press 'F' to turn LED strip 2 OFF.\n");
    printf("Press 'Q' to destroy both LED strips and exit.\n");
    printf("Press 'Z' to arm the 1 second timer.\n");
    printf("Press 'X' to disarm the 1 second timer.\n");
    printf("Press 'C' to arm the 5 second timer.\n");
    printf("Press 'V' to disarm the 5 second timer.\n");
    printf("Press 'B' to arm the 10 second timer.\n");
    printf("Press 'N' to disarm the 10 second timer.\n");

    /* Get the starting timestamp. */
    t_prev = clock();
    ECU_RUNTIME_ASSERT( (t_prev >= (clock_t)0) );

    while (running)
    {
        c = getchar();
        if (c != EOF)
        {
            switch (tolower(c))
            {
                case '1':
                {
                    printf("LED1 toggled!\n");
                    led_dispatch(&led1, &BUTTON_PRESS_EVENT);
                    printf("\n");
                    break;
                }

                case '2':
                {
                    printf("LED2 toggled!\n");
                    led_dispatch(&led2, &BUTTON_PRESS_EVENT);
                    printf("\n");
                    break;
                }

                case '3':
                {
                    printf("LED3 toggled!\n");
                    led_dispatch(&led3, &BUTTON_PRESS_EVENT);
                    printf("\n");
                    break;
                }

                case '4':
                {
                    printf("LED4 toggled!\n");
                    led_dispatch(&led4, &BUTTON_PRESS_EVENT);
                    printf("\n");
                    break;
                }
                
                case '5':
                {
                    printf("LED5 toggled!\n");
                    led_dispatch(&led5, &BUTTON_PRESS_EVENT);
                    printf("\n");
                    break;
                }

                case 'a':
                {
                    printf("LED strip 1 turned ON!\n");
                    led_strip_on(&led_strip1);
                    printf("\n");
                    break;
                }

                case 's':
                {
                    printf("LED strip 1 turned OFF!\n");
                    led_strip_off(&led_strip1);
                    printf("\n");
                    break;
                }        

                case 'd':
                {
                    printf("LED strip 2 turned ON!\n");
                    led_strip_on(&led_strip2);
                    printf("\n");
                    break;
                }

                case 'f':
                {
                    printf("LED strip 2 turned OFF!\n");
                    led_strip_off(&led_strip2);
                    printf("\n");
                    break;
                }

                case 'q':
                {
                    printf("Exiting! Destroying both LED strips!\n");
                    led_strip_destroy(&led_strip1);
                    led_strip_destroy(&led_strip2);
                    printf("\n");
                    running = false;
                    break;
                }

                case 'z':
                {
                    printf("Arming 1 second timer!\n");
                    ecu_tlist_timer_rearm(&timers, &timer_1s);
                    printf("\n");
                    break;
                }

                case 'x':
                {
                    printf("Disarming 1 second timer!\n");
                    ecu_timer_disarm(&timer_1s);
                    printf("\n");
                    break;
                }

                case 'c':
                {
                    printf("Arming 5 second timer!\n");
                    ecu_tlist_timer_rearm(&timers, &timer_5s);
                    printf("\n");
                    break;
                }

                case 'v':
                {
                    printf("Disarming 5 second timer!\n");
                    ecu_timer_disarm(&timer_5s);
                    printf("\n");
                    break;
                }

                case 'b':
                {
                    printf("Arming 10 second timer!\n");
                    ecu_tlist_timer_rearm(&timers, &timer_10s);
                    printf("\n");
                    break;
                }

                case 'n':
                {
                    printf("Disarming 10 second timer!\n");
                    ecu_timer_disarm(&timer_10s);
                    printf("\n");
                    break;
                }

                default:
                {
                    /* Ignore all other characters. */
                    break;
                }
            }
        }

        /* Service the timers. */
        t_current = clock();
        ECU_RUNTIME_ASSERT( (t_current >= 0) );
        t_elapsed = t_current - t_prev;

        if (t_elapsed > (clock_t)0)
        {
            t_prev = clock();
            ECU_RUNTIME_ASSERT( (t_prev >= 0) );

            if (t_elapsed > ECU_TICK_MAX)
            {
                t_elapsed = ECU_TICK_MAX;
            }
            
            ecu_tlist_service(&timers, (ecu_tick_t)t_elapsed);
        }
    }

    return 0;
}

/*------------------------------------------------------------*/
/*---------------- ASSERT HANDLER DEFINITION -----------------*/
/*------------------------------------------------------------*/

void ecu_assert_handler(const char *file, int line)
{
    (void)file;
    (void)line;
    while(1)
    {

    }
}
