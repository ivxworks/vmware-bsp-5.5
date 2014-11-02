/* configHtml.h - HTML configuration header file */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,18apr02,rhe  Added C++ Protection
01e,25feb99,rlp  added the Secure Socket Layer support.
01d,28nov98,ics  New configuration mechanism.
01c,27nov98,lcs  Remove SCREEN_SIZE definition, this is now defined in
                 htmlENavigatorConfig.h
01b,26nov98,lcs  Add #define SCREEN_SIZE to enable selection of correct gui 
		 toolbar from the bsp
01a,08oct98,rlp  written.
*/

/*
This module contains the HTML configuration parameters for the
PC [34]486/Pentium/Pentiumpro.
*/


#ifndef INCconfigHtmlh
#define INCconfigHtmlh

#ifdef __cplusplus
extern "C" {
#endif

/*
 * HTML products.
 * Define one of the following.
 */

#undef	INCLUDE_HTMLWORKS		/* include HTMLWorks product */
#undef	INCLUDE_ENAVIGATOR		/* include eNavigator product */


/*
 * Which UI and splash screen to select -- select the UI closest
 * to the actual screen size.
 */
#define HTML_SCREEN_SIZE	HTML_SCREEN_VGA

/* include the Secure Socket Layer support */

#undef  INCLUDE_HTML_SSL

/*
 * Additional configuration options from target/src/config/usrHtml.c
 * can be overridden here.
 */

#ifdef __cplusplus
}
#endif

#endif /* INCconfigHtmlh */

