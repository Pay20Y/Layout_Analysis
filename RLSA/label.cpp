#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp> 
#include <vector>
#include <string>
#include <fstream>
#include <sys/types.h> //to get file from folder
#include <dirent.h>

using namespace std;
using namespace cv;

Mat loadImage(string filename){
	string imagepath = "/home/george/data/POD/" + filename;
    Mat image = imread(imagepath);
    if (image.empty()) {
        cout<<"read image failed"<<endl;
        return Mat();
    }
    cvtColor(image, image, CV_BGR2RGB);
    return image;
}

int main(){
	DIR* pDir;
    struct dirent* ptr; 
    if(!(pDir = opendir("/home/george/data/POD")))
        return -1;

    int textIndex = 0;
    int tableIndex = 0;
    int imageIndex = 0;
    int formulaIndex = 0;
    int errorIndex = 0;

    while((ptr = readdir(pDir)) != 0){
    	string filename = ptr->d_name;
        
        if(ptr->d_type == 4){
            cout<<"a dir so continue..."<<endl;
            continue;
        }
        
        cout<<"Now process "<<filename<<" ..."<<endl;

        Mat input = loadImage(filename);
        Mat labelInput = input.clone();

        imshow("label please!",input);
        waitKey(-1);
        
        int classLabel;
        
        string output = "";
        
        cout<<"label please! 1-text 2-table 3-image 4-formula "<<endl;
        cin >> classLabel;
        switch(classLabel){
        	case 1 :
        		output = "text_" + to_string(textIndex);
        		textIndex++;
        		break;
        	case 2:
        		output = "table_" + to_string(tableIndex);
        		tableIndex++;
        		break;
        	case 3:
        		output = "image_" + to_string(imageIndex);
        		imageIndex++;
        		break;
        	case 4:
        		output = "formula_" + to_string(formulaIndex);
        		formulaIndex++;
        		break;
        	default:
        		output = "error_" + to_string(errorIndex);
        		errorIndex++;
        		break;
        }

        imwrite(output + ".jpg",labelInput);

    }
}