/*
 * PoI.h
 *
 *  Created on: Aug 11, 2020
 *      Author: angelo
 */

#ifndef POI_H_
#define POI_H_

#include "MyCoord.h"

class UAV;

class PoI {
public:
	PoI(MyCoord posCoord);
	PoI(MyCoord posCoord, int id_new);

public:
	static void generateRandomPoIs(std::list<PoI *> &pl, int ss, int np);

	void init(int npkt_sec);

public:
	MyCoord actual_coord;

	int id;
	static int idPoIGen;

	double start;

	int packetPerSecond;

	int nPacket2Generate;
	int generationIntervalSlots;
	int next_packet_generation_tk;

	// used fro CommunicationManager for tree building
	UAV *father;
};

#endif /* POI_H_ */
