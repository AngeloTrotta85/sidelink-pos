/*
 * UAV.cpp
 *
 *  Created on: Aug 11, 2020
 *      Author: angelo
 */

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

#include "RandomGenerator.h"
#include "UAV.h"

using namespace std;

int UAV::idUAVGen = 0;

UAV::UAV(MyCoord recCoord) {
	id = idUAVGen++;
	actual_coord = recCoord;
}

void UAV::generateRandomUAVs(std::list<UAV *> &ul, int ss, int nu) {
	//for (int i : boost::irange(0, nu)) { // i goes from 0 to nu-1
	for (int i = 0; i < nu; i++) {

		MyCoord pos = MyCoord::ZERO;

		double intrange = ((double) ss) / 2.0;
		pos.x = RandomGenerator::getInstance().getRealUniform(-intrange, intrange);
		pos.y = RandomGenerator::getInstance().getRealUniform(-intrange, intrange);

		UAV *newU = new UAV(pos);
		ul.push_back(newU);

		//std::cout << "UAV: " << i << " --> " << newU->recharge_coord << " - Energy:" << newU->max_energy << std::endl;
	}
}

void UAV::generateStarUAVs(std::list<UAV *> &ul, std::list<PoI *> &pl, int nu, double dist) {
	int leftUAVs = nu;
	MyCoord dummyBS = MyCoord::ZERO;

	for (auto& poi : pl) {
		int needed = ceil(dummyBS.distance(poi->actual_coord) / dist);
		if (needed <= leftUAVs) {

			if (needed > 1) {
				MyCoord offset = poi->actual_coord / needed;
				MyCoord nextUAV = offset;

				for (int i = 1; i < needed; i++) {
					UAV *newU = new UAV(nextUAV);
					ul.push_back(newU);

					nextUAV = nextUAV + offset;
				}
			}

			UAV *newU = new UAV(poi->actual_coord);
			ul.push_back(newU);

			leftUAVs -= needed;
		}
	}
}


