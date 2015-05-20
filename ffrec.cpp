#include "ffrec.hpp"

FFrec::FFrec(string user){
	username = user;
	conf_file = user+".conf";

	use_recognition=false;
	
	ifstream test(conf_file);
	if(!test.is_open()){
		cout<<"generating default config file.."<<endl;
		ifstream in("default.conf");
		ofstream out(conf_file, ios::app);
		out<<"#####################################"<<endl;
		out<<"#Configuration file for user '"<<user<<"'#"<<endl;
		out<<"#####################################"<<endl;
		out<<in.rdbuf();
		in.close();
		out.close();
	}
	test.close();

	readConfigFile(conf_file);

	if(use_recognition){
		retrieveTrainingData();
	}

	setupClassifier();
	
	cout<<"ffrec start..."<<endl;
	//try to open cam
	try{
		cap.open(-1);
	}catch(Exception e){
		cout<<"impossible to read from webcam... bye"<<endl;
		exit(60);
	}

	if(!cap.isOpened()){
			cout<<"cam not avaible... bye"<<endl;
			exit(20);
		}
	cout<<"cam opened"<<endl;
}


bool
FFrec::setnewTraining(){
	if(images_folder.empty()){
		cout<<"the images directory wasn't setted, check in the config file the option 'TRAINING_DIR'"<<endl;
		exit(32);
	}
	vector<Mat> images;
    vector<int> labels;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (images_folder.c_str())) != NULL) {
    	int i=0;
    	while ((ent = readdir (dir)) != NULL) {
        	if(strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")){
            	cout<<format("readed images %d\r", ++i);
            	images.push_back(imread(format("training//%s",ent->d_name), 0));
            	labels.push_back(atoi(ent->d_name));
        	}
    	}
      	closedir (dir);
      	cout<<endl;
	} else {
   		cout<<"cannot open the directory... bye"<<endl;
   		exit(31);
    }

    cout<<"creating model (please wait... could take a while)"<<endl;
    model = createFisherFaceRecognizer();
    model->train(images, labels);

    cout<<"the model was successfull created"<<endl;
    
    ffrec_trained=username+".model";
    std::remove(ffrec_trained.c_str());
    model->save(ffrec_trained);

    	ifstream testfile(ffrec_trained);
    	if(testfile.is_open()){
    		cout<<"file "+ffrec_trained+" correctly created"<<endl;
    		testfile.close();
		 	ofstream configfile(conf_file, ios::app);
		 	configfile<<"#auto-generated file containing the training data of a specific user"<<endl;
		 	configfile<<FFREC_LOAD_FILE<<" "<<ffrec_trained<<endl;
		 	configfile.close();
		 }
    
return true;	
}

bool
FFrec::retrieveTrainingData(){
	if(ffrec_trained.empty()){
		cout<<"no training set was previously created\n in order to use recognition is required to create e new model"<<endl;
		FFrec::setnewTraining();
	}else{
		try{
			model = createFisherFaceRecognizer();
			model->load(ffrec_trained);
		}catch(Exception e){
			cout<<"something appen loading the model.. bye"<<endl;
			exit(40);
		}

		cout<<"ffrec model loaded..."<<endl;
	}

return true;
}

void
FFrec::readConfigFile(string configfile){
	ifstream conf(configfile);

	if(!conf.is_open()){
		cout<<configfile + " doesn't exist or cannot be accessed... bye"<<endl;
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
	cout<<"config file readed"<<endl;
}

bool
FFrec::getFrame(){
	try{
		cap.open(-1);
	
	}catch(Exception e){
		cout<<"impossible to read from webcam... bye"<<endl;
		exit(60);
	}
	cap>>frame;
	cap>>frame;
	cap>>frame;
	cap>>frame;
	cap>>frame;
	cap>>frame;
	cap>>frame;
	cap>>frame;
	resize(frame, frame, VGA, 0,0);
	cout<<"new frame"<<endl;
	cap.release();
 
return true;
}

bool
FFrec::setupClassifier(){
	if(haar_file.empty()){
		cout<<"no haar classifier fire was setted... bye"<<endl;
		exit(32);
	}
	
	ifstream file(haar_file);
	if(!file.is_open()){
		cout<<haar_file + " doesn't exist or cannot be accessed... bye"<<endl;
		exit(33);
	}
	file.close();

	try{
		haar_cascade.load(haar_file);
	//	cout<<"feature type:" << haar_cascade.getFeatureType()<<endl;
	}catch(Exception e){
		cout<<"something happen during classifier loading... bye"<<endl;
		exit(21);
	}
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
    //int prediction = model->predict(resized);
    rectangle(frame, faces[maxface], CV_RGB(0, 255,0), 1);
    cout<<"rect distance: "<<faces[maxface].width<<endl;
	p_width = faces[maxface].width;
}

long
FFrec::getEyesDistance(){
	l_dist = (long)((-50*(p_width)+21550)/186);
	cout<<"distance: "<<l_dist<<" cm"<<endl;
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
	 		cout<<"no face detected"<<endl;
	 	}

	 	imshow("img", frame);
    	if(waitKey(20));
     }
	
return true;
}
