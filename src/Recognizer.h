//
// Created by daniel on 6/11/15.
//

#ifndef FACE_RECOGNITION_FACERECOGNIZER_H
#define FACE_RECOGNITION_FACERECOGNIZER_H

#include <time.h>

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <std_msgs/Int32.h>

#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>

using namespace cv;

struct attempt {
    double confidence;
    int label;
    void data(int _label, double _confidence) {
        this->label = _label;
        this->confidence = _confidence;
    }
};


class Recognizer {
    ros::NodeHandle nh;
    ros::Publisher face_pub;
    image_transport::ImageTransport it;
    image_transport::Subscriber image_source;
    string trainingData;

private:
    int isTrained;
    Ptr<cv::FaceRecognizer> model;
    vector<cv::Mat> newImages;
    vector<int> newLabels;
    vector<double> newConfidence;
    //double computeDeviation();

public:
    Recognizer();
    ~Recognizer();
    void makePrediction(const sensor_msgs::ImageConstPtr &msg);
};


#endif //FACE_RECOGNITION_FACERECOGNIZER_H
