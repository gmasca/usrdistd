#include "ffrec.hpp"

#define KRED "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYLL "\x1B[33m"
#define RSET "\x1B[0m"

FFrec::FFrec(string user, int mode){
	username = user;
	conf_file = user+".conf";

	use_recognition=false;
	
	FFrec::mode=mode;
	cout<<"mode: ";
	if(mode==MOD_VIDEO)
		cout<<KYLL<<"video_mode"<<RSET<<endl;
	else
		cout<<KYLL<<"frame_mode"<<RSET<<endl;


	ifstream test(conf_file);
	if(!test.is_open()){
		cout<<"generating default config file... "<<endl;
		ifstream in("default.conf");
		ofstream out(conf_file, ios::app);
		out<<"#####################################"<<endl;
		out<<"#Configuration file for user '"<<user<<"'#"<<endl;
		out<<"#####################################"<<endl;
		out<<in.rdbuf();
		in.close();
		out.close();
		cout<<KGRN<<"[OK]"<<RSET<<endl;
	}
	test.close();

	readConfigFile(conf_file);

	if(use_recognition){
		retrieveTrainingData();
	}

	setupClassifier();
	
	cout<<"start... "<<KGRN<<"[OK]"<<RSET<<endl;

	//try to open cam
	cout<<"cam check... ";
	cap.set(CV_CAP_PROP_FPS, 1);
	try{
		cap.open(-1);
	}catch(Exception e){
		cout<<KRED<<"impossible to read from webcam... bye"<<" [ERR]"<<RSET<<endl;
		exit(60);
	}

	if(!cap.isOpened()){
			cout<<KRED<<"cam not avaible... bye"<<" [ERR]"<<RSET<<endl;
			exit(20);
		}

	cap.set(CV_CAP_PROP_FPS, 30);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, VGA_w);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, VGA_h);
	
	cout<<KGRN<<"[OK]"<<RSET<<endl;
	cap.release();
}


bool
FFrec::setnewTraining(){
	cout<<"creating a new ff recognizer:"<<endl;
	cout<<"\tlooking for training dir... ";
	if(images_folder.empty()){
		cout<<KRED<<"the images directory was not setted, check in the config file the option 'TRAINING_DIR'"<<" [ERR]"<<RSET<<endl;
		exit(32);
	}
	cout<<KGRN<<"[OK]"<<RSET<<endl;
	
	vector<Mat> images;
    vector<int> labels;

    cout<<"\tloading images form directory:"<<endl;;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (images_folder.c_str())) != NULL) {
    	int i=0;
    	while ((ent = readdir (dir)) != NULL) {
        	if(strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")){
            	cout<<format("\r\t\treaded images %d", ++i);
            	images.push_back(imread(format("training//%s",ent->d_name), 0));
            	labels.push_back(atoi(ent->d_name));
        	}
    	}
      	closedir (dir);
      	cout<<"... "<<KGRN<<"[OK]"<<RSET<<endl;
	} else {
   		cout<<KRED<<"cannot open the directory... bye"<<" [ERR]"<<RSET<<endl;
   		exit(31);
    }
	
    cout<<"creating ff rec model (please wait... could take a while)"<<endl;
    
    int pid=fork();
    if(!pid)
    	while(1){
    		cout<<".";
    		sleep(1);
    		fflush(stdout);
    	}

    model = createFisherFaceRecognizer();
    model->train(images, labels);
    
	cout<<KGRN<<"[OK]"<<RSET<<endl;
    ffrec_trained=username+".model";
    cout<<"saving ffrec model ["<<ffrec_trained<<"]"<<endl;
    std::remove(ffrec_trained.c_str());
    model->save(ffrec_trained);
   
    	ifstream testfile(ffrec_trained);
    	if(testfile.is_open()){
    		testfile.close();
		 	ofstream configfile(conf_file, ios::app);
		 	configfile<<"#auto-generated file containing the training data of a specific user"<<endl;
		 	configfile<<FFREC_LOAD_FILE<<" "<<ffrec_trained<<endl;
		 	configfile.close();
		 }
	    kill(pid, SIGKILL);
	cout<<KGRN<<"[OK]"<<RSET<<endl<<endl;
    
return true;	
}

bool
FFrec::retrieveTrainingData(){
	cout<<"loading user's ff recognizer model ["<<ffrec_trained<<"]...";
	fflush(stdout);

	if(ffrec_trained.empty()){
		cout<<KYLL<<"no training set was previously created!\nin order to use recognition is required to create e new model"<<RSET<<endl;
		FFrec::setnewTraining();
	}else{
		try{
			model = createFisherFaceRecognizer();
			model->load(ffrec_trained);
		}catch(Exception e){
			cout<<KRED<<"something appen loading the model... bye"<<" [ERR]"<<RSET<<endl;
			exit(40);
		}

	cout<<KGRN<<"[OK]"<<RSET<<endl;
	}

return true;
}

void
FFrec::readConfigFile(string configfile){
	ifstream conf(configfile);

	cout<<"reading of config file ["<<configfile<<"]... ";
	
	if(!conf.is_open()){
		cout<<KRED<<configfile + " doesn't exist or cannot be accessed... bye"<<" [ERR]"<<RSET<<endl;
		exit(30);
	}

	string line;

	while(getline(conf, line)){
	   if(line[0]=='#' || line[0]=='\n' || line[0]=='\r' || line.empty()){continue;}
	    smatch sm;
	    if(regex_search(line, sm, regex(TRAINING_DIR)))
			images_folder = line.substr(sm[0].length()+1, 256);

		if(regex_search(line, sm, regex(HAAR_CASCADE_FILE)))
			haar_file = line.substr(sm[0].length()+1, 256);

		if(regex_search(line, sm, regex(FFREC_LOAD_FILE)))
			ffrec_trained = line.substr(sm[0].length()+1,256);
		
		if(regex_search(line, sm, regex(USE_RECOGNITION)))
			use_recognition = true;
	}

	conf.close();
	cout<<KGRN<<"[OK]"<<RSET<<endl;
}

bool
FFrec::getFrame(){
	try{
		cap.open(-1);
	}catch(Exception e){
		cout<<KRED<<"impossible to read from webcam... bye"<<" [ERR]"<<RSET<<endl;
		exit(60);
	}

cap>>frame;
cap>>frame;

	
if(!mode){
	cap>>frame;
	cap>>frame;
	cap>>frame;
	cap.release();
}
	
return true;
}

bool
FFrec::setupClassifier(){
	cout<<"loading classifier ["<<haar_file<<"]... ";
	if(haar_file.empty()){
		cout<<KRED<<"no haar classifier fire was setted... bye"<<" [ERR]"<<RSET<<endl;
		exit(32);
	}
	
	ifstream file(haar_file);
	if(!file.is_open()){
		cout<<KRED<<haar_file + " does not exist or cannot be accessed... bye"<<" [ERR]"<<RSET<<endl;
		exit(33);
	}
	file.close();

	try{
		haar_cascade.load(haar_file);
	//	cout<<"feature type:" << haar_cascade.getFeatureType()<<endl;
	}catch(Exception e){
		cout<<KRED<<"something happen during classifier loading... bye"<<" [ERR]"<<RSET<<endl;
		exit(21);
	}
	cout<<KGRN<<"[OK]"<<RSET<<endl;
}

bool
FFrec::find(){
	if(use_recognition)
		return findRecogFace();
	else
		return findFace();
}

bool
FFrec::findRecogFace(){
	Mat gray = frame.clone();
	cvtColor(frame, gray, CV_BGR2GRAY);
	vector< Rect_<int> > faces;
    haar_cascade.detectMultiScale(gray, faces); //detect function

    //search for the biggest one
    long maxarea=0;
    long area=0;
    int maxface = -1;

    for(int i=0; i<faces.size(); i++){
    	area=(faces[i].width*faces[i].height);
    	if(area >maxarea){
    		maxarea= area;
    		maxface = i;
    	}
    }

    Mat current;
    if(maxface>-1)
    	current = gray(faces[maxface]);
    else
    	return false;
    
    Mat resized;
    resize(current, resized, QVGA, 1.0, 1.0, INTER_CUBIC);
    int prediction = model->predict(resized);
    rectangle(frame, faces[maxface], CV_RGB(0, 255,0), 1);

    cout<<"rect distance: "<<faces[maxface].width<<endl;
	p_width = faces[maxface].width;
}

bool
FFrec::findFace(){
	Mat gray = frame.clone();
	cvtColor(frame, gray, CV_BGR2GRAY);
	vector< Rect_<int> > faces;
    haar_cascade.detectMultiScale(gray, faces);

    //search for the biggest one
    long maxarea=0;
    long area=0;
    int maxface=-1;

    for(int i=0; i<faces.size(); i++){
    	area=(faces[i].width*faces[i].height);
    	if(area >maxarea){
    		maxarea= area;
    		maxface = i;
    	}
    }

    Mat current;
    if(maxface>-1)
    	current = gray(faces[maxface]);
    else
    	return false;
    
    cout<<"\t[face]"<<endl;
    Mat resized;
    resize(current, resized, QVGA, 1.0, 1.0, INTER_CUBIC);
    //int prediction = model->predict(resized);
    rectangle(frame, faces[maxface], CV_RGB(0, 255,0), 1);
       
    cout<<"\t[width: "<<faces[maxface].width<<"]"<<endl;
	p_width = faces[maxface].width;
}

long
FFrec::getEyesDistance(){
	l_dist = (long)((-50*(p_width)+21550)/186);
	cout<<"\t[distance: "<<l_dist<<" cm]"<<endl;
	return l_dist;
}

bool
FFrec::show(){
	namedWindow("img", 3);
 	
    while(1){
	 	this->getFrame();
	 	if(this->find()){
	 		this->getEyesDistance();
	 	}else{
	 		l_dist=0;
	 		cout<<"\t[no face]"<<endl;
	 	}

	 	imshow("img", frame);
    	if(waitKey(20));
     }
	
return true;
}

