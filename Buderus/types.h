/*
 * types.h
 *
 *  Created on: 03.02.2014
 *      Author: googy
 */

#ifndef TYPES_H_
#define TYPES_H_


// Fehler Speichervariable
struct {
	unsigned diesel_t_error:		1;
	unsigned wwasser_t_error:		1;
	unsigned FM241_error:			1;
	unsigned hk2_t_error:			1;
} errors;



#endif /* TYPES_H_ */
