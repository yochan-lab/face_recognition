//
// Created by daniel on 6/11/15.
//

#ifndef FACE_RECOGNITION_FACESORTER_H
#define FACE_RECOGNITION_FACESORTER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Person.h"

using namespace std;

class FaceSorter {

private:
    int generateLabel(); //store actual names or not?
    vector<Person> people;

public:
    FaceSorter();
    ~FaceSorter();
    vector<cv::Mat> getAllImages();
    vector<cv::string> getAllLabels();
    void addPerson(cv::Mat); //both of these should transfer ownership...
    int addImage(int ID, cv::Mat);

};


#endif //FACE_RECOGNITION_FACESORTER_H
