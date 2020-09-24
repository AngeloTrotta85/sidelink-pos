/*
 * PoI.cpp
 *
 *  Created on: Aug 11, 2020
 *      Author: angelo
 */

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

#include "RandomGenerator.h"
#include "PoI.h"

int PoI::idPoIGen = 0;

PoI::PoI(MyCoord posCoord) {
	actual_coord = posCoord;
	id = idPoIGen++;

	father = nullptr;
	packetPerSecond = 100;
	nPacket2Generate = 10;
	generationIntervalSlots = 100;
	next_packet_generation_tk = 0;
	start = 0;
}

PoI::PoI(MyCoord posCoord, int id_new) {
	actual_coord = posCoord;
	id = id_new;

	father = nullptr;
	packetPerSecond = 100;
	nPacket2Generate = 10;
	generationIntervalSlots = 100;
	next_packet_generation_tk = 0;
	start = 0;
}

void PoI::generateRandomPoIs(std::list<PoI *> &pl, int ss, int np) {
	//for (int i : boost::irange(0, nu)) { // i goes from 0 to nu-1
	for (int i = 0; i < np; i++) {

		MyCoord poipos = MyCoord::ZERO;

		double intrange = ((double) ss) / 2.0;
		poipos.x = RandomGenerator::getInstance().getRealUniform(-intrange, intrange);
		poipos.y = RandomGenerator::getInstance().getRealUniform(-intrange, intrange);

		PoI *newP = new PoI(poipos);
		newP->start = RandomGenerator::getInstance().getRealUniform(0, 1);
		pl.push_back(newP);

		//std::cout << "UAV: " << i << " --> " << newU->recharge_coord << " - Energy:" << newU->max_energy << std::endl;
	}
}


void PoI::init(int npkt_sec) {
	//next_packet_generation_tk = RandomGenerator::getInstance().getIntUniform(0, 1000);
	next_packet_generation_tk = 0;


	packetPerSecond = npkt_sec;
}





