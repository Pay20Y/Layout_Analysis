#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp> 
#include <vector>
#include <string>

using namespace std;
using namespace cv;

/*
//Tree Structure Definition
typedef struct STreeNode* pSTreeNode;
typedef int TreeDataType;

struct STreeNode{
	TreeDataType data;
	pSTreeNode pFirstChild;
	pSTreeNode pNextBrother;
	
	STreeNode(TreeDataType Value){
		data = Value;
		pFirstChild = NULL;
		pNextBrother = NULL;
	}
};

class CTree{
	public:
		CTree();
		CTree(TreeDataType Value);
		//~CTree();
	public:
		void Insert(TreeDataType parentValue,TreeDataType Value);
		void InsertBrother(pSTreeNode pParentNode,TreeDataType Value);
		pSTreeNode Search(pSTreeNode pNode,TreeDataType Value);
	public:
		pSTreeNode pRooot;
};

CTree::CTree(){
	pRooot = NULL;
}

CTree::CTree(TreeDataType Value){
	pRoot = new STreeNode(Value);
	if(pRooot == NULL)
		return;
}

void CTree::Insert(TreeDataType parentValue,TreeDataType Value){
	if(pRoot == NULL)
		return;
	pSTreeNode pFindNode = Search(pRoot,parentValue);
	if(pFindNode == NULL)
		return;
	if(pFindNode->pFirstChild == NULL){
		pFindNode->pFirstChild = new STreeNode(Value);
		return;
	}else{
		InsertBrother(pFindNode->pFirstChild,Value);
		return;
	}
}

void CTree::InsertBrother(pSTreeNode pBrotherNode,TreeDataType Value){
	if(pBrotherNode->pNextBrother != NULL)
		InsertBrother(pBrotherNode->pNextBrother,Value);
	else{
		pBrotherNode->pNextBrother = new STreeNode(Value);
		return;
	}
}

pSTreeNode CTree::Search(pSTreeNode pNode,TreeDataType Value){
	if(pNode == NULL)
		return NULL;
	
	if(pNode->data == Value)
		return pNode;
	
	if(pNode->pFirstChild == NULL && pNode->pNextBrother == NULL)
		return NULL;
	else{
		if(pNode->pFirstChild != NULL){
			pSTreeNode pNodeTemp = Search(pNode->pFirstChild,Value);
			if(pNodeTemp != NULL)
				return pNodeTemp;
			else{
				return Search(pNode->pNextBrother,Value);
			}
		}else
			return Search(pNode->pNextBrother,Value);
	}
}
// Tree Structure Definition end!
*/

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

	Mat paintY = Mat::zeros(binaryImage.rows,binaryImage.cols,CV_8UC1);
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
		uchar* ptr_y = paintY.ptr<uchar>(row);
		for(int col = 0;col < histHor[row];col++){
			ptr_y[col] = 255;
		}
	}

	imshow("axisY!",paintY);
	waitKey(-1);

	return histHor;
}
//checked no errors
vector<int> shrinkHist(vector<int>& hist,int& cursor1,int& cursor2){
	vector<int> thinHist;
	thinHist.reserve(hist.size());
	
	// int cursor1 = 0;
	// int cursor2 = hist.size();
	
	for(vector<int>::const_iterator it1 = hist.begin();it1 != hist.end();it1++){		
		if(*it1 != 0){
			break;
		}
		cursor1++;
	}

	for(vector<int>::const_iterator it2 = hist.end();it2 != hist.begin();it2--){
		if(*it2 != 0){
			break;
		}
		cursor2--;
	}

	for(int i = cursor1+1;i < cursor2;i++){
		thinHist.push_back(hist[i]);
	}
	
	return thinHist;
}

void countZero(vector<int>& hist,std::vector<int>& segments1,std::vector<int>& segments2,int top,int bottom){
	int cursor1 = 0;
	int cursor2 = 0;
	int preBlock = 0;
	int preBlock_end = 0;
	// std::vector<int> segments1;
	// std::vector<int> segments2;
	// Mat checkImage = Mat::zeros(binaryImage.rows,binaryImage.cols,CV_8UC1);
	// for(vector<int>::const_iterator it1 = hist.begin();it1 != hist.end();it1++){
	while((cursor2 != hist.size()) && (cursor1 != hist.size())){
		if(hist[cursor1] == 0){
			cursor2 = cursor1;
			preBlock_end = cursor1 - 1;
			while((hist[cursor2] == 0) && (cursor2 != hist.size())){
				cursor2++;
			}

			if((cursor2 - cursor1 + 1) >= 30){
				segments1.push_back(top + preBlock - 1);
				segments2.push_back(top + preBlock_end + 1);
				preBlock = cursor2;
			}
			
			cursor1 = cursor2;
			
		}else{
			cursor1 ++;
		}	
	}

	//add the last segments to vector
	segments1.push_back(top + cursor2 - 1);
	segments2.push_back(top +cursor1 + 1);

	/*
	for(int i = 0;i < segments1.size();i++){
		line(input,Point(0,segments1[i] + top),Point(input.cols,top + segments1[i]),Scalar(255,0,0));
		line(input,Point(0,segments2[i] + top),Point(input.cols,top + segments2[i]),Scalar(255,0,0));
	}
	*/
	/*
	for(int i = 0;i < segments1.size();i++){
		cout<<"segments1: "<<segments1[i]<<" ";
	}
	cout<<endl;
	for(int i = 0;i < segments1.size();i++){
		cout<<"segments2: "<<segments2[i]<<" ";
	}
	*/
	cout<<"segments1' size is: "<<segments1.size()<<endl;
	cout<<"segments2' size is: "<<segments2.size()<<endl;
	
	// imshow("lines",input);
	// waitKey(-1);
	/*
	for(std::vector<int>::const_iterator it1 = segments.begin();it1 != segments.end();it1++){
		for(int index = 0;index < binaryImage.cols;index++){
			checkImage.at<uchar>(*it1,index) = 255;
		}
	}
	imshow("segments",checkImage);
	waitKey(-1);
	*/
	// }
}

std::vector<Mat> cropImage(Mat binaryImage,Mat input,bool triker){

	// bool triker = true; // the var is to change orientation of each recursive
	std::vector<Mat> cropMats;
	
	if(triker){
		std::vector<int> histHor = calcHistHor(binaryImage);
		int cursor1 = 0;
		int cursor2 = histHor.size();
		std::vector<int> thinHist = shrinkHist(histHor,cursor1,cursor2);
		std::vector<int> segments1;
		std::vector<int> segments2;
		


		countZero(thinHist,segments1,segments2,cursor1,cursor2);

		cout<<"countZero end!"<<endl;
		if((segments1.size() > 0) && (segments2.size() > 0) && (segments1.size() == segments2.size())){
			if(segments1.size() == segments2.size()){
				for(int i = 0;i < segments1.size();i++){
					int y = segments1[i];
					int x = 0;
					int height = segments2[i] - segments1[i];
					int width = binaryImage.cols;
					Rect cropRect(x,y,width,height);
					Mat image_cut = Mat(input,cropRect);
					cropMats.push_back(image_cut);
				}
			}else{
				cout<<"error! 2 vectors' size are not equal!";
			}
		}
	}else{

	}
	/*
	for(std::vector<Mat>::const_iterator itm = cropMats.begin();itm != cropMats.end();itm++){
		imshow("...",*itm);
		waitKey(-1);
	}*/

	return cropMats;
}

/*
void makeTree(){
	CTree docTree = CTree(1);
	
}*/

int main(){
	// preprocess gray-scale binarylization
	Mat input = loadImage("1.png");
	Mat input_clone = input.clone();
	Mat grayImage = convert2gray(input);
	Mat binaryImage = binaryzation(grayImage);
	Mat binaryImageReg = ~binaryImage;

	std::vector<Mat> crops = cropImage(binaryImageReg,input_clone,true);
	
	/*
	// projection on axis x & y
	std::vector<int> histHor = calcHistHor(binaryImageReg);
	std::vector<int> histVer = calcHistVer(binaryImageReg);	
	cout<<"histHor's size is: "<<histHor.size()<<endl;
	cout<<"histVer's size is: "<<histVer.size()<<endl;

	// segment the image
	std::vector<int> segments1;
	std::vector<int> segments2;

	int cursor1 = 0;
	int cursor2 = histHor.size();
	std::vector<int> thinHist = shrinkHist(histHor,cursor1,cursor2);	
	countZero(thinHist,segments1,segments2,input_clone,cursor1,cursor2);
	*/		
}