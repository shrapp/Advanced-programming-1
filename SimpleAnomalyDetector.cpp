/*
 * SimpleAnomalyDetector.cpp
 *
 * Author: 313361560 Shahar Rapp, 205866163 Ze'ev Binnes.
 */

#include "SimpleAnomalyDetector.h"

SimpleAnomalyDetector::SimpleAnomalyDetector() {
}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
}

/*
This function will iterate trough a time series table,
find pair of correlated features, and add them to the list of correlated fatures,
which is a member of the "SimpleAnomalyDetector" Class.
*/
void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts){
    // initialize parameters:
    // correlation should be bigger than threshold
    float threshold = (0.5 > this->correlationThreshold) ? this->correlationThreshold : 0.5;
    float enlarge_allowed_dev = 1.1;    // allow small anomaly, bigger than the max deviation by this.
    cf.clear();
    // Iterate through the columns.
    for (int i = 0; i < ts.getSize(); i++) {
        float maxPearson = threshold;      // the pearson with the most matching column.
        int c = -1;     // will save the matching column.
        vector<float> col1 = ts.getColumn(i);
        // check the next columns, and choose the one that has the best match.
        for (int j = i + 1; j < ts.getSize(); j++) {
            vector<float> col2 = ts.getColumn(j);
            float p = std::fabs(pearson(col1, col2, ts.getLength()));
            if (p > maxPearson) {
                maxPearson = p;  // save the best pearson.
                c = j;  // save the best match.
            }
        }
        // If any match was found, save the best one to the correlated features list.
        if (c != -1) {
            vector<float> col2 = ts.getColumn(c);
            struct correlatedFeatures cf1;
            cf1.feature1 = ts.getHeadLine(i);
            cf1.feature2 = ts.getHeadLine(c);
            cf1.corrlation = maxPearson;
            cf1.lin_reg = linear_reg(col1, col2, col1.size());
            find_threshold(cf1, col1, col2);
            cf1.threshold = enlarge_allowed_dev * cf1.threshold;  // allow a small deviation.
            this->cf.push_back(cf1);    // add the pair to the list of correlated features.
        }
    }
}

void SimpleAnomalyDetector::find_threshold(struct correlatedFeatures & cf1, vector<float> col1, vector<float> col2) {
    cf1.threshold = 0;
    // Find the max offset from the line.
    for (int k = 0; k < col1.size(); k++) {
        float d = dev(Point(col1[k], col2[k]), cf1.lin_reg);
        if (d > cf1.threshold)
        cf1.threshold = d;
    }
}

vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts){
    vector<AnomalyReport> report;
    // iterate through the rows.
    for (int i = 0; i < ts.getLength(); i++) {
        vector<float> row = ts.getRow(i);
        // for each row, check that all the features are OK.
        for (correlatedFeatures cf1 : this->cf) {
            // get the right columns by name: I'm not sure the order will always be the same.
            int feature1Index = ts.getFeaturesIndex(cf1.feature1);
            int feature2Index = ts.getFeaturesIndex(cf1.feature2);
            Point point = Point(row[feature1Index], row[feature2Index]);
            float deviation = findDeviation(point, cf1);
            // if the deviation is to big, report a problem.
            if (deviation > cf1.threshold)
                report.push_back(AnomalyReport(cf1.feature1+"-"+cf1.feature2, i+1));
        }
    }
    return report;
}

float SimpleAnomalyDetector::findDeviation(Point point, struct correlatedFeatures cf1) {
    return dev(point, cf1.lin_reg);
}