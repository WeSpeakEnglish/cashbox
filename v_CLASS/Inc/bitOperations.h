/*
 * bitOperations.h
 *
 * Created: 10.04.2016 10:56:32
 *  Author: Waffle
 */ 


#ifndef BITOPERATIONS_H_
#define BITOPERATIONS_H_

#define SET_REG_(r, n)	(r = n)
#define CLR_REG_(r)		(r = 0)

#define SET_BIT_(r, b)   (r |=  (1 << b))
#define CLR_BIT_(r, b)   (r &= ~(1 << b))
#define TGL_BIT_(r, b)   (r ^=  (1 << b))
#define CHK_BIT_(r, b)   (r &   (1 << b))

#endif /* BITOPERATIONS_H_ */