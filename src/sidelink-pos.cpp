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

	int scenarioSize = 10000;
	int nUAV = 15;
	int nPoI = 10;
	int nChan = 5;
	double dtGen = 100;
	double ddist = 1000;
	double dmax = 2500;
	int tend = 2500;
	int tstop = 2000;
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
	const std::string &out_string = input.getCmdOption("-fout");
	if (!out_string.empty()) {
		fout = out_string;
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

	// Randomize UAVs
	//UAV::generateRandomUAVs(uavsList, scenarioSize, nUAV);
	//UAV::generateStarUAVs(uavsList, poisList, nUAV, ddist);
	UAV::generateMSTUAVs(uavsList, poisList, nUAV, ddist);

	// Calculate Noise Parameters
	double temperature = 290; // Kelvin
	double k = 1.3806488 * pow(10.0, -23.0); // Boltzman Constant
	double bw = 9*1e6; // Effective Bandwidth of channel (9 MHz)
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
		int poiIDX = 1;
		for (auto& p : poisList) {
			for (auto& u : uavsList) {
				if ((u->coveringPoi != nullptr) && (u->coveringPoi->id == p->id)) {
					//f_out << "POI:" << (p->id + 1) << ";U:" << (u->id + 1) << endl;
					f_out << "POI:" << poiIDX << ";U:" << (u->id + 1) << endl;
					++poiIDX;
				}
			}
			//f_out << "POI:" << (p->id + 1) << ";x:" << p->actual_coord.x << ";y:" << p->actual_coord.y << ";s:" << p->start << endl;
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
		//f_out << "K1:" << 5 << ";K2:" << 0.0333 << endl;

		//L1 and L2
		//f_out << "L1:" << -5 << ";L2:" << 25 << endl;

		//K1 and K2
		f_out << "K1SNR:" << 5 << ";K2SNR:" << 0.0333 << ";K1SINR:" << 5 << ";K2SINR:" << 0.0333 << endl;

		//L1 and L2
		f_out << "L1SNR:" << -5 << ";L2SNR:" << 25 << ";L1SINR:" << -5 << ";L2SINR:" << 25 << endl;

		//Tstop e Tend
		f_out << "TS:" << tstop << ";TE:" << tend << endl;

		//Dmax (U2U) and Dbs(U2G)
		f_out << "DM:" << dmax << ";DB:" << (1000 + 1) << endl;

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
	for (int ii = 500; ii <= 50000; ii+=500) {
	//for (int ii = 2500; ii <= 2500; ii+=500) {
		long double sum_jj = 0;
		long double count_jj = 0;
		for (int jj = 0; jj < 2000; jj++) {
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

	cout << endl;

	for (int dd = 1000; dd <= 10000; dd+=500) {
		for (int i = 0; i <= 10; i++) {
			long double sum_tt = 0;
			long double count_tt = 0;

			for (int tt = 0; tt < 10000; tt++) {
				double interference = 0;

				//			long double sum_jj = 0;
				//			long double count_jj = 0;
				//			for (int jj = 0; jj < 2000; jj++) {
				//				sum_jj += ll->rss_with_fading(MyCoord::ZERO, MyCoord(0, ddist));
				//				count_jj += 1;
				//			}
				//			double rss_rand_at_RCV = sum_jj / count_jj;

				double rss_rand_at_RCV = ll->rss_with_fading(MyCoord::ZERO, MyCoord(0, dd));

				for (int j = i; j > 0; j--) {
					MyCoord poipos = MyCoord::ZERO;
					//double intrange = ((double) scenarioSize) / 2.0;
					double intrange = dd*2;
					poipos.x = RandomGenerator::getInstance().getRealUniform(-intrange, intrange);
					poipos.y = RandomGenerator::getInstance().getRealUniform(-intrange, intrange);

					interference += ll->rss_with_fading(MyCoord::ZERO, poipos);
				}

				double sinr_rand = linear2dBm(rss_rand_at_RCV/(interference + total_noise));

				sum_tt += getProb_linear(sinr_rand);
				//sum_tt += getProb_sigmoid(sinr_rand);
				count_tt++;
			}

			cout << "At dist " << dd
					<< " and " << i << " node interfering, we have rcv probability of: " << (sum_tt / count_tt) << endl;
		}

		cout << endl;
	}


	cout << "Finish!!!" << endl;
	return 0;
}








