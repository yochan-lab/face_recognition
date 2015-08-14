//
// Created by daniel on 6/11/15.
//

#include "Recognizer.h"

Recognizer::Recognizer():it(nh) {
    srand(time( NULL ));
    if(!ros::param::get("face_recognizer/training_file", trainingData))
        ROS_ERROR("No training data file specified. Please set the 'training_file' parameter");
    image_source = it.subscribe("/face_finder/image_output", 1, &Recognizer::makePrediction, this);

    face_pub = nh.advertise<std_msgs::Int32>("label_output", 1, false);

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

    newImages.push_back(imageSample->image); //add image for either training or updating

    if(isTrained == 0) {
        newLabels.push_back(rand()); //assign random ID for first person.
        model->train(newImages,newLabels);
        newImages.erase(newImages.begin(), newImages.end()); //erase any files, to prevent duplicate training.
        newLabels.erase(newLabels.begin(), newLabels.end()); //I could just use pop_back, but I want to be sure...
        newConfidence.erase(newConfidence.begin(), newConfidence.end());// this is probably unnecessary.
	    isTrained = 1;
	    return;
    }
    ROS_INFO("processing face");
    int resultLabel; double resultConfidence;
    //model->set("threshold", 55);
    model->predict(imageSample->image, resultLabel, resultConfidence); //this may not be correct
    newConfidence.push_back(resultConfidence);


    if(newConfidence.size() > 10 && resultConfidence >= 60) { //if consistently above normal we met a new person!
        int newID = rand();
        newLabels.push_back(newID); //generate new ID

        newImages.erase(newImages.begin(), newImages.begin()+1); //erase the first (usually deficient) data.
        newLabels.erase(newLabels.begin(), newLabels.begin()+1);
        newConfidence.erase(newConfidence.begin(), newConfidence.begin()+1);

        for(int i=0; i<newLabels.size(); i++) //assign the new ID to the training data.
            newLabels[i] = newID;

        model->update(newImages, newLabels);
	    ROS_INFO("new face: %i, P=%3.2f", newLabels.back(), resultConfidence);
    }
    else if(resultLabel == -1 || resultConfidence >= 60) { //if above normal, there may be a new person
        //if (newConfidence.size() > 20) {
	//    newImages.erase(newImages.begin(), newImages.end()-10);
	//    newLabels.erase(newLabels.begin(), newLabels.end()-10);
	//    newConfidence.erase(newConfidence.begin(), newConfidence.end()-10);
	//}

	newLabels.push_back(-1);
        ROS_INFO("possible new face: P=%3.2f", resultConfidence);
        return;
    }
    else { //we saw a familiar face
        newLabels.push_back(resultLabel);
        model->update(newImages, newLabels);
        ROS_INFO("familiar face: %i, P=%3.2f", newLabels.back(), resultConfidence);
    }

    std_msgs::Int32 label_msg;
    label_msg.data = newLabels.back();
    face_pub.publish(label_msg);

    model->save(trainingData);

    newImages.erase(newImages.begin(), newImages.end()); //erase any files, to prevent duplicate training.
    newLabels.erase(newLabels.begin(), newLabels.end());
    newConfidence.erase(newConfidence.begin(), newConfidence.end());
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "face_recognition");
    Recognizer r;
    ros::spin();
    return 0;
}

