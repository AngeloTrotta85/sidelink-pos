/*
 * Link.h
 *
 *  Created on: Sep 24, 2020
 *      Author: angelo
 */

#ifndef LINK_H_
#define LINK_H_

#include "MyCoord.h"

class Link {
public:
	Link() {};
	virtual ~Link(){};

	double getUAVChannelLoss (double freq, double tx_height, double rx_height, double dist, bool randomness);
	double rss(MyCoord src, MyCoord dst, bool randomness);
};

#endif /* LINK_H_ */
