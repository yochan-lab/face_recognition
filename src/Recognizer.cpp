//
// Created by daniel on 6/11/15.
//

#include "Recognizer.h"

Recognizer::Recognizer():it(nh) {
    if(!ros::param::get("face_recognizer/training_file", trainingData))
        ROS_ERROR("No training data file specified. Please set the 'training_file' parameter");
    image_source = it.subscribe("/face_finder/image_output", 1, &Recognizer::makePrediction, this);

    model = createLBPHFaceRecognizer();
    isTrained = 1;
    try {
        model->load(trainingData);
    }
    catch(Exception &e) {
        isTrained = 0;
        ROS_ERROR("Could not find existing training data. A new file will be created");
    }
}

Recognizer::~Recognizer() { };

void Recognizer::makePrediction(const sensor_msgs::ImageConstPtr &msg) {
    //create a pointer to an opencv image.
    cv_bridge::CvImagePtr imageSample;
    try {
        imageSample = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_8UC1);
    }
    catch (cv_bridge::Exception &e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    vector<Mat> newImage; newImage.push_back(imageSample->image);
    vector<int> newLabel;

    if(isTrained == 0) {
        newLabel.push_back(rand());
        model->train(newImage,newLabel);
	isTrained = 1;
	return;
    }

    attempt anAttempt;
    anAttempt.data(-1, 0.0);0;
    model->predict(imageSample->image, anAttempt.label, anAttempt.confidence); //this may not be correct

    if(anAttempt.label == -1 || anAttempt.confidence <= .1) { //we met a new person!
        newLabel.push_back(rand());
        model->update(newImage, newLabel);
    }
    else { //we saw a familiar face
        newLabel.push_back(anAttempt.label);
        model->update(newImage, newLabel);
    }

    model->save(trainingData);
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "face_recognition");
    Recognizer r;
    ros::spin();
    return 0;
}
