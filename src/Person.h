//
// Created by daniel on 6/17/15.
//

#ifndef FACE_RECOGNITION_PERSON_H
#define FACE_RECOGNITION_PERSON_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


class Person {
private:
    string ID;
    vector<int> labels;
    vector<cv::Mat> images;

public:
    void addImage(cv::Mat image);
    vector<int> &getLabels();
    vector<cv::Mat> &getImages();
};


#endif //FACE_RECOGNITION_PERSON_H
