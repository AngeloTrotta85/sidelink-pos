/*
 * UAV.h
 *
 *  Created on: Aug 11, 2020
 *      Author: angelo
 */

#ifndef UAV_H_
#define UAV_H_

#include <map>

#include "MyCoord.h"
#include "PoI.h"

#define BS_ID 1000000

class UAV {


public:
	UAV(MyCoord recCoord);

	static void generateRandomUAVs(std::list<UAV *> &ul, int ss, int nu);
	static void generateStarUAVs(std::list<UAV *> &ul, std::list<PoI *> &pl, int nu, double dist);
	static void generateMSTUAVs(std::list<UAV *> &ul, std::list<PoI *> &pl, int nu, double dist);

public:
	MyCoord actual_coord;

	PoI *coveringPoi;

	int id;
	static int idUAVGen;
};

#endif /* UAV_H_ */
