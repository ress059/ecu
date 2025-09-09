/**
 * @file
 * @brief
 * @rst
 * See :ref:`asserter.h section <asserter_h>` in Sphinx documentation.
 * @endrst
 *
 * @author Ian Ress
 * @version 0.1
 * @date 2024-03-02
 * @copyright Copyright (c) 2024
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*---------------------- GLOBAL VARIABLES --------------------*/
/*------------------------------------------------------------*/

/* Actually allocate memory for array used in custom static assert implementation. */
const char ecu_static_assert_fired_[1] ECU_ATTRIBUTE_UNUSED = {0};
