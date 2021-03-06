/*
 * animaly_detection_util.cpp
 *
 * Author: 313361560 Shahar Rapp, 205866163 Ze'ev Binnes.
 */

#include <math.h>
#include "anomaly_detection_util.h"
#include <vector>

using namespace std;

float avg(vector<float> x, int size){
    if (size <= 0) return NAN;
    float sum = 0;
    for (int i = 0; i < size ; i++){
        sum += x[i];
    }
	return sum/(float)size;
}

// returns the variance of X and Y
float var(vector<float> x, int size){
    float mu = avg(x, size);
    float sum2 = 0;
    for (int i = 0; i < size ; i++){
        sum2 += pow(x[i], 2);
    }
	return (sum2 / (float)size) - pow(mu, 2);
}

// returns the covariance of X and Y
float cov(vector<float> x, vector<float> y, int size){
    if (size <= 0) return NAN;
    float sum = 0;
    for (int i = 0; i < size ; i++){
        sum += (x[i] - avg(x,size))*(y[i] - avg(y,size));
    }
    return sum / (float)size;
}


// returns the Pearson correlation coefficient of X and Y
float pearson(vector<float> x, vector<float> y, int size){
    float denominator = (sqrt(var(x,size))*sqrt(var(y,size)));
    if (denominator == 0) return NAN;
	return cov(x,y,size)/denominator;
}

// performs a linear regression and returns the line equation
Line linear_reg(Point** points, int size){
    vector<float> x;
    vector<float> y;
    for (int i = 0; i < size; i++) {
        x[i] = points[i]->x;
        y[i] = points[i]->y;
    }
    Line line = linear_reg(x, y, size);

    return line;
}

// performs a linear regression from vector<float>, and returns the line equation
Line linear_reg(vector<float> x, vector<float> y, int size) {
    float a = (cov(x, y, size) / var(x, size));
    float b = avg(y, size) - (a * avg(x, size));
    return Line(a, b);
}

// returns the deviation between point p and the line equation of the points
float dev(Point p,Point** points, int size){
    Line line = linear_reg(points, size);
    return dev(p, line);
}

// returns the deviation between point p and the line
float dev(Point p,Line l){
    return fabs(l.f(p.x) - p.y);
}




