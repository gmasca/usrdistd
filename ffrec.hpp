#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

#define VGA Size(640,480)
#define QVGA Size(320, 240)

#define TRAINING_DIR "TRAINING_DIR"
#define HAAR_CASCADE_FILE "HAAR_CASCADE_FILE"
#define FFREC_LOAD_FILE "FFREC_TRAINED_FILE"
#define USE_RECOGNITION "USE_RECOGNITION"


using namespace std;
using namespace cv;

class FFrec{
	
	private:
		string haar_file;
		string images_folder;
		string ffrec_trained;
		string conf_file;
		
		string username;

		bool use_recognition;

		Ptr<FaceRecognizer> model; 
		CascadeClassifier haar_cascade;

		VideoCapture cap;
		Mat frame;

		int p_width;
		long l_dist;

	public:
		FFrec(string conf);

		void readConfigFile(string conf); //ok

		bool setnewTraining(); //ok
		bool retrieveTrainingData(); //ok

		bool setupClassifier();

		bool findFace();
		bool findRecogFace();

		bool find();

		bool getFrame(); //ok
		long getEyesDistance();//ok
		bool show();//ok
};
