//============================================================================
// Name        : sidelink-pos.cpp
// Author      : Angelo
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <algorithm>    // std::find
#include <vector>       // std::vector
#include <list>       // std::list
#include <stack>
#include <map>       // std::list
#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>


#include <complex>      // std::complex, std::real

#include "RandomGenerator.h"
#include "UAV.h"
#include "PoI.h"
#include "Link.h"

using namespace std;



class InputParser{
public:
	InputParser (int &argc, char **argv){
		for (int i=1; i < argc; ++i)
			this->tokens.push_back(std::string(argv[i]));
	}
	const std::string& getCmdOption(const std::string &option) const{
		std::vector<std::string>::const_iterator itr;
		itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
		if (itr != this->tokens.end() && ++itr != this->tokens.end()){
			return *itr;
		}
		static const std::string empty_string("");
		return empty_string;
	}
	bool cmdOptionExists(const std::string &option) const{
		return std::find(this->tokens.begin(), this->tokens.end(), option)
		!= this->tokens.end();
	}
private:
	std::vector <std::string> tokens;
};

double linear2dBm(double x) {
	return (10.0 * log10(x * 1000.0));
}

double dbm2linear(double x) {
	return pow(10.0, (x-30.0)/10.0);
	//return (10.0 * log10(x * 1000.0));
}

double getProb_sigmoid(double sinr) {
	return (1.0 / (1.0 + exp((10.0 - sinr)*0.25)));
}

double getProb_linear(double sinr) {
	if (sinr <= -5.0) {
		return 0.0;
	}
	else if (sinr >= 25.0) {
		return 1.0;
	}
	else {
		return ((sinr+5.0)/30.0);
	}
}

int main(int argc, char **argv) {
	std::list<UAV *> uavsList;
	std::list<PoI *> poisList;

	//MyCoord bs_pos = MyCoord::ZERO;

	InputParser input(argc, argv);

	int scenarioSize = 500;
	int nUAV = 10;
	int nPoI = 3;
	int nChan = 5;
	double dtGen = 1000;
	double ddist = 100;
	string fout = string("static_pos.txt");

	const std::string &seedUser = input.getCmdOption("-seed");
	if (!seedUser.empty()) {
		int seedR = atoi(seedUser.c_str());
		RandomGenerator::getInstance().setSeed(seedR);
	}
	else {
		unsigned seedR = std::chrono::system_clock::now().time_since_epoch().count();
		RandomGenerator::getInstance().setSeed(seedR);
	}
	const std::string &ss_string = input.getCmdOption("-ss");
	if (!ss_string.empty()) {
		scenarioSize = atoi(ss_string.c_str());
	}
	const std::string &nu_string = input.getCmdOption("-nu");
	if (!nu_string.empty()) {
		nUAV = atoi(nu_string.c_str());
	}
	const std::string &np_string = input.getCmdOption("-np");
	if (!np_string.empty()) {
		nPoI = atoi(np_string.c_str());
	}
	const std::string &nc_string = input.getCmdOption("-nc");
	if (!nc_string.empty()) {
		nChan = atoi(nc_string.c_str());
	}
	const std::string &dg_string = input.getCmdOption("-dg");
	if (!dg_string.empty()) {
		dtGen = atof(dg_string.c_str());
	}
	const std::string &d_string = input.getCmdOption("-dist");
	if (!d_string.empty()) {
		ddist = atof(d_string.c_str());
	}

	// Randomize PoIs
	PoI::generateRandomPoIs(poisList, scenarioSize, nPoI);

	// Randomize PoIs
	UAV::generateRandomUAVs(uavsList, scenarioSize, nUAV);

	// Calculate Noise Parameters
	double temperature = 290; // Kelvin
	double k = 1.3806488 * pow(10.0, -23.0); // Boltzman Constant
	double bw = 9*1e6; // Efective Bandwidth of channel (9 MHz)
	double ue_noise_figure = 7 ; // 7 dB noise figure is considered
	double noise = linear2dBm(k * temperature * bw);
	double total_noise_dBm = ue_noise_figure + noise;
	double total_noise = pow(10.0, total_noise_dBm/10.0) / 1000.0;

	stringstream filename;
	filename << fout.c_str();
	ofstream f_out(filename.str(), ofstream::out);
	if (f_out.is_open()) {

		//UAVs
		for (auto& u : uavsList) {
			f_out << "U:" << (u->id + 1) << ";x:" << u->actual_coord.x << ";y:" << u->actual_coord.y << endl;
		}

		//PoIs
		for (auto& p : poisList) {
			f_out << "POI:" << (p->id + 1) << ";x:" << p->actual_coord.x << ";y:" << p->actual_coord.y << ";s:" << p->start << endl;
		}

		//BS
		f_out << "BS:0;x:0;y:0" << endl;

		//Number of channels
		f_out << "NC:" << nChan << endl;

		//Data rate
		f_out << "DR:" << dtGen << endl;

		//N0
		f_out << "N0:" << total_noise << endl;

		//K1 and K2
		f_out << "K1:" << 5 << ";K2:" << 0.0333 << endl;

		//L1 and L2
		f_out << "L1:" << -5 << ";L2:" << 25 << endl;

		//RSSs
		Link *ll = new Link();
		for (auto& u1 : uavsList) {
			for (auto& u2 : uavsList) {
				if (u1->id != u2->id) {
					long double sum_jj = 0;
					long double count_jj = 0;
					for (int jj = 0; jj < 2000; jj++) {
						sum_jj += ll->rss_with_fading(u1->actual_coord,u2->actual_coord);
						count_jj += 1;
					}
					double rss_rand = sum_jj / count_jj;

					f_out << "U:" << (u1->id + 1) << ";U:" << (u2->id + 1) << ";RSS:" << rss_rand << endl;
				}
			}
		}

		f_out.close();
	}

	//TEST
	Link *ll = new Link();
	for (int ii = 50; ii <= 5000; ii+=50) {
		long double sum_jj = 0;
		long double count_jj = 0;
		for (int jj = 0; jj < 10000; jj++) {
			sum_jj += ll->rss_with_fading(MyCoord::ZERO, MyCoord(0, ii));
			count_jj += 1;
		}
		double rss_rand = sum_jj / count_jj;
		double rss_static = ll->rss(MyCoord::ZERO, MyCoord(0, ii));

		double sinr_static = linear2dBm(rss_static/total_noise);
		double sinr_rand = linear2dBm(rss_rand/total_noise);

		cout << "Dist " << ii
				//<< "; N0 " << total_noise << "[" << total_noise_dBm << "]"
				<< "; N0 " << total_noise_dBm
				<< "; PL " << ll->getUAVChannelLoss(3410,30,30,ii)
				//<< "; rss(" << ii << ")"
				<< " -> no-fading: " << rss_static << "[" << linear2dBm(rss_static) << "; S=" << sinr_static << "; P=" << getProb_linear(sinr_static) << "]" <<
				"; fading: " << rss_rand << "[" << linear2dBm(rss_rand) << "; S=" << sinr_rand << "; P=" << getProb_linear(sinr_rand) << "]" << endl;

	}

	cout << "Finish!!!" << endl;
	return 0;
}








