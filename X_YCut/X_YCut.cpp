#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp> 
#include <vector>
#include <string>

using namespace std;
using namespace cv;

Mat loadImage(string filepath){
	Mat image = imread(filepath);
	
	if (image.empty()) {
        cout<<"read image failed"<<endl;
        return Mat();
    }

    cvtColor(image, image, CV_BGR2RGB);
    return image;
}

Mat convert2gray(Mat input){
	Mat grayImage( input.size(), CV_8UC1 );
    cvtColor ( input, grayImage, CV_RGB2GRAY );
    return grayImage;
}

Mat binaryzation(Mat grayImage){
	if(grayImage.channels() != 1){
		cout<<"binarylize must be the gray image"<<endl;
		return Mat();
	}
	Mat binaryImage;

	threshold(grayImage,binaryImage,125,255,THRESH_BINARY);
	cout<<binaryImage.rows<<" "<<binaryImage.cols<<endl;
	return binaryImage;
}

std::vector<int> calcHistVer(Mat binaryImage){
	assert(binaryImage.channels() == 1);

	Mat paintX = Mat::zeros(binaryImage.rows,binaryImage.cols,CV_8UC1);
	std::vector<int> histVer;
	histVer.reserve(binaryImage.cols);
	for(int col = 0;col < binaryImage.cols;col++){
		histVer.push_back(0);
	}

	for(int col = 0;col < binaryImage.cols;col++){
		
		for(int row = 0;row < binaryImage.rows;row++){
			const uchar* ptr = (const uchar*) binaryImage.ptr(row);
			if(ptr[col] > 0){
				histVer[col]++;
			}
			
		}
	}
	cout<<histVer.size()<<endl;

	for(int col = 0;col < binaryImage.cols;col++){
		for(int row = 0;row < histVer[col];row++){
			uchar * ptr_x = paintX.ptr<uchar>(row);
			ptr_x[col] = 255;
		}
	}

	imshow("axisX!",paintX);
	waitKey(-1);

	return histVer;
}

std::vector<int> calcHistHor(Mat binaryImage){
	assert(binaryImage.channels() == 1);

	Mat paintX = Mat::zeros(binaryImage.rows,binaryImage.cols,CV_8UC1);
	std::vector<int> histHor;
	histHor.reserve(binaryImage.rows);
	for(int row = 0;row < binaryImage.rows;row++){
		histHor.push_back(0);
	}
	for(int row = 0;row < binaryImage.rows;row++){
		const uchar* ptr = (const uchar*) binaryImage.ptr(row);
		for(int col = 0;col < binaryImage.cols;col++){
			if(ptr[col] > 0){
				histHor[row]++;
			}
		}
	}

	for(int row = 0;row<binaryImage.rows;row++){
		const uchar* ptr_y = paintY.ptr<uchar>(row);
		for(int col = 0;col < histHor[row];col++){
			ptr_y[col] = 255;
		}
	}

	imshow("axisY!",paintY);
	waitKey(-1);

	return histHor;
}

int main(){
	Mat input = loadImage("1.png");
	Mat grayImage = convert2gray(input);
	Mat binaryImage = binaryzation(grayImage);
	Mat binaryImageReg = ~binaryImage;
	std::vector<int> histHor = calcHistHor(binaryImageReg);
	std::vector<int> histVer = calcHistVer(binaryImageReg);
	
	cout<<"histHor's size is: "<<histHor.size()<<endl;
	for(std::vector<int>::const_iterator it1 = histHor.begin();it1 != histHor.end();it1++){
		cout<<*it1<<" ";
	}
	cout<<endl;
	cout<<"histVer's size is: "<<histVer.size()<<endl;
	for(std::vector<int>::const_iterator it2 = histVer.begin();it2 != histVer.end();it2++){
		cout<<*it2<<" ";
	}
	cout<<endl;
	
}