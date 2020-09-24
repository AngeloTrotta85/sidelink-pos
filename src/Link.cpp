/*
 * Link.cpp
 *
 *  Created on: Sep 24, 2020
 *      Author: angelo
 */


#include <complex>      // std::complex, std::real

#include "Link.h"
#include "RandomGenerator.h"

double Link::getUAVChannelLoss (double freq, double tx_height, double rx_height, double dist, bool randomness) {
	//double C = 0;
	//double temp = rx_height * (1.1 * log10(freq) - 0.7) - (1.56 * log10(freq) - 0.8);
	double sigma = 8; // Standard Deviation for Shadowing Effect

	//double path_loss = 41.1 + 20.9 * log10(dist);
	//double path_loss = 41.1 + 41.8 * log10(dist);
	double path_loss = 41.1 + 41.8 * log10(dist);

	if (randomness) {
		//double path_loss = 46.3 + 33.9 * log10(freq) - 13.82 * log10(tx_height) - temp + log10(dist/1000.0)*(44.9 - 6.55 * log10(tx_height))+C;
		double channel_loss = -path_loss + (-1 * sigma * RandomGenerator::getInstance().getRealNormal(0, 1));

		return channel_loss;
	}
	else {
		return path_loss;
	}
}

double Link::rss(MyCoord src, MyCoord dst, bool randomness) {

	double distance = src.distance(dst);

	double UAV_TX_pt_db = 24;
		//double GAIN_ag = pow(10.0, 0.6);  //% Transmiter Antenna Gain (6 dB)
		double GAIN_ag_db = 6;
		double freq = 3410;
		double tx_height = 30;
		double rx_height = 30;
		//double distance = u_src->actual_coord.distance(u_dst->actual_coord);
		double loss_dB = getUAVChannelLoss(freq,tx_height,rx_height,distance,randomness);

		double receivedPower_db = UAV_TX_pt_db + GAIN_ag_db + loss_dB;
		double receivedPower = pow(10.0, receivedPower_db / 10.0) / 1000.0;
		//double receivedPower = UAV_TX_pt * GAIN_ag * pow(10.0, loss_dB/10.0) ;//(10.^((loss_dB)/10)); // received power including path loss,shadowing
		if (randomness) {

			double fading_variance = 1.59; // Fading model is Complex Gaussian Random Variable
			double chan_real = sqrt(fading_variance/2) * RandomGenerator::getInstance().getRealNormal(0, 1);
			double chan_complex = RandomGenerator::getInstance().getRealNormal(0, 1);
			std::complex<double> chan (chan_real, chan_complex);
			//chan = sqrt(fading_variance/2) * RandomGenerator::getInstance().getRealNormal(0, 1) + 1i*RandomGenerator::getInstance().getRealNormal(0, 1);
			double chan_value = std::abs(chan); // fading loss

			receivedPower *= chan_value;
		}

		return receivedPower;
}


