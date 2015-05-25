/*
 *  LinearRegression.cpp
 *  hog2
 *
 *  Created by Nathan Sturtevant on 6/1/06.
 *  Copyright 2006 Nathan Sturtevant. All rights reserved.
 *
 */


#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <iostream>
#include "LinearRegression.h"

LinearRegression::LinearRegression(int _inputs) {
	inputs = _inputs;
	weights.resize(inputs+1);
}

void LinearRegression::train(std::vector<double> &input, double target) {
    double featuresOn = 0;
    unsigned long input_size = input.size();
    for (int x = 0; x < input_size; x++) {
        if (input[x]) { ++featuresOn; }
    }
    rate = 1 / (10 * featuresOn);
    //std::cerr << "Rate: " << rate << " Features on: " << featuresOn << std::endl;
	double output = test(input);
	for (int x = 0; x < input_size; x++) {
        if (input[x] == 0) { continue; }
		weights[x] += rate*(target-output)*input[x];
	}
	weights[input_size] += rate*(target-output);
}

double LinearRegression::test(const std::vector<double> &input) {
	double result = 0;
    unsigned long input_size = input.size();
	for (int x = 0; x < input_size; x++) {
        if (input[x] == 0) { continue; }
		result += input[x]*weights[x];
	}
	result += weights.back();
	return result;
}

void LinearRegression::Print() {
	for (int x = 0; x <= inputs; x++) {
		printf("%1.3f  ", weights[x]);
	}
	printf("\n");
}
