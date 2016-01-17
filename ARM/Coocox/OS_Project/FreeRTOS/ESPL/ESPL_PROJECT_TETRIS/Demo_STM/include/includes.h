/**
 * Main include file.
 *
 * @author: Eduardo Ocete, Natalia Paredes, Ovie Osimiry
 *
 **/
#include "stdio.h"
#include "stdlib.h"
#include <time.h>

/* Board includes */
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f429i_discovery_lcd.h"


/* FreeRTOS includes */
//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"


//## My additions
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"
#include "tm_stm32f4_rng.h"

//##

/* uGFX includes. */
#include "gfx.h"

/* convenience functions and init includes */

#include "ESPL_functions.h"
#include "Demo.h"

//##
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_ltdc.h"
#include "stm32f4xx_adc.h"
