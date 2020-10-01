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

	coveringPoi = nullptr;
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
			newU->coveringPoi = poi;
			ul.push_back(newU);

			leftUAVs -= needed;
		}
	}
}

void UAV::generateMSTUAVs(std::list<UAV *> &ul, std::list<PoI *> &pl, int nu, double dist) {
	std::list<PoI *> poiTree;
	std::list<PoI *> startPool;

	std::list<std::pair<PoI *, PoI *>> edgeTree;

	PoI *bsPOI = new PoI(MyCoord::ZERO);

	poiTree.push_back(bsPOI);
	for (auto& pp : pl) {
		startPool.push_back(pp);
	}

	while (!startPool.empty()) {
		double midist = std::numeric_limits<double>::max();
		PoI *pstart = nullptr;
		PoI *pend = nullptr;
		std::list<PoI *>::iterator pend_it;

		for (auto& p1 : poiTree) {
			std::list<PoI *>::iterator p2it = startPool.begin();
			while (p2it != startPool.end()) {
				auto p2 = *p2it;

				if (p1->actual_coord.distance(p2->actual_coord) < midist) {
					midist = p1->actual_coord.distance(p2->actual_coord);
					pstart = p1;
					pend = p2;
					pend_it = p2it;
				}

				p2it++;
			}
		}

		edgeTree.push_back(std::make_pair(pstart, pend));
		poiTree.push_back(pend);

		startPool.erase(pend_it);
	}

	for (auto& ee : edgeTree) {
		std::cout << "P " << ee.first->actual_coord << " - P " << ee.second->actual_coord << std::endl;
	}

	int leftUAVs = nu;
	std::list<PoI *> queueTree;
	queueTree.push_back(bsPOI);
	while (!queueTree.empty()) {
		PoI *nextp = queueTree.front();
		queueTree.pop_front();

		for (auto& pp : edgeTree) {
			if (pp.first->id == nextp->id) {
				int needed = ceil(pp.second->actual_coord.distance(nextp->actual_coord) / dist);
				if (needed <= leftUAVs) {

					if (needed > 1) {
						MyCoord offset = (pp.second->actual_coord - pp.first->actual_coord) / needed;
						MyCoord nextUAV = pp.first->actual_coord + offset;

						for (int i = 1; i < needed; i++) {
							UAV *newU = new UAV(nextUAV);
							ul.push_back(newU);

							nextUAV = nextUAV + offset;
						}
					}

					UAV *newU = new UAV(pp.second->actual_coord);
					newU->coveringPoi = pp.second;
					ul.push_back(newU);

					leftUAVs -= needed;

					queueTree.push_back(pp.second);
				}
			}
		}

	}
}





