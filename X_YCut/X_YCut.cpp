#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp> 
#include <vector>
#include <string>
#include <stack>

using namespace std;
using namespace cv;

//Tree Structure Definition
typedef struct STreeNode* pSTreeNode;
typedef int TreeDataType;
typedef pair<std::vector<Mat>,std::vector<Rect>> MatRect;

struct STreeNode{
	TreeDataType data;
	Rect boundingRect;
	pSTreeNode pFirstChild;
	pSTreeNode pNextBrother;
	pSTreeNode pParent;
	
	STreeNode(TreeDataType Value,Rect rect){
		data = Value;
		boundingRect = rect;
		pFirstChild = NULL;
		pNextBrother = NULL;
		pParent = NULL;
	}
};

class CTree{
	public:
		CTree();
		CTree(TreeDataType Value,Rect rect);
		//~CTree();
	public:
		void Insert(TreeDataType parentValue,TreeDataType Value,Rect bRect);
		void InsertBrother(pSTreeNode pParentNode,TreeDataType Value,Rect bRect,pSTreeNode parentNode);
		pSTreeNode Search(pSTreeNode pNode,TreeDataType Value);
		pSTreeNode SearchParent(pSTreeNode pNode,TreeDataType Value);
		void print();
		void print(pSTreeNode node, int num);
	public:
		pSTreeNode pRoot;
};

CTree::CTree(){
	pRoot = NULL;
}

CTree::CTree(TreeDataType Value,Rect rect){
	pRoot = new STreeNode(Value,rect);
	if(pRoot == NULL)
		return;
}

void CTree::Insert(TreeDataType parentValue,TreeDataType Value,Rect bRect){
	if(pRoot == NULL)
		return;
	pSTreeNode pFindNode = Search(pRoot,parentValue);
	if(pFindNode == NULL)
		return;
	if(pFindNode->pFirstChild == NULL){
		pFindNode->pFirstChild = new STreeNode(Value,bRect);
		pFindNode->pFirstChild->pParent = pFindNode;
		return;
	}else{
		InsertBrother(pFindNode->pFirstChild,Value,bRect,pFindNode);
		return;
	}
}

void CTree::InsertBrother(pSTreeNode pBrotherNode,TreeDataType Value,Rect bRect,pSTreeNode parentNode){
	if(pBrotherNode->pNextBrother != NULL)
		InsertBrother(pBrotherNode->pNextBrother,Value,bRect,parentNode);
	else{
		pBrotherNode->pNextBrother = new STreeNode(Value,bRect);
		pBrotherNode->pNextBrother->pParent = parentNode;
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

pSTreeNode CTree::SearchParent(pSTreeNode pNode,TreeDataType Value){
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
					return pNode->pFirstChild;
				else{
					return Search(pNode->pNextBrother,Value);
				}

		}else
			return Search(pNode->pNextBrother,Value);
	}
}

void CTree::print()
{
    print(pRoot, 0);
}

void printSpace(int num)
{
    int i = 0;
    for(i = 0; i < num-3; i++)
        cout << " ";

    for(; i < num-2; ++i)
        cout << "|";
    for(; i < num; ++i)
        cout << "_";
}


void CTree::print(pSTreeNode node, int num)
{
    if(node != NULL){
        printSpace(num); 
        cout << node->data << endl;  
        print(node->pFirstChild, num+4);
        print(node->pNextBrother, num);
    }
}

// Tree Structure Definition end!

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
	// imshow("axisX!",paintX);
	// waitKey(-1);
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

	// imshow("axisY!",paintY);
	// waitKey(-1);

	return histHor;
}
//checked no errors
vector<int> shrinkHist(vector<int>& hist,int& cursor1,int& cursor2){
	vector<int> thinHist;
	thinHist.reserve(hist.size());
	
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

	while((cursor2 != hist.size()) && (cursor1 != hist.size())){
		if(hist[cursor1] == 0){
			cursor2 = cursor1;
			preBlock_end = cursor1 - 1;
			while((hist[cursor2] == 0) && (cursor2 != hist.size())){
				cursor2++;
			}

			if((cursor2 - cursor1 + 1) >= 15){
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
	// cout<<"cursor2: "<<cursor2<<endl;
	// segments1.push_back(top + cursor2 - 1);
	if(cursor1 - preBlock >= 3){
		segments1.push_back(top + preBlock - 1);
		segments2.push_back(top + cursor1 + 1);
	}
	// cout<<"start: "<<segments1[0]<<"    end: "<<segments2[0]<<endl;
 	cout<<"segments1' size is: "<<segments1.size()<<endl;
	cout<<"segments2' size is: "<<segments2.size()<<endl;	

}

std::vector<Mat> cropImage(Mat binaryImage/*,Mat input*/,bool triker,int index,std::vector<Rect>& cropRects){

	std::vector<Mat> cropMats;
	// std::vector<Rect> cropRects;
	// std::vector<Point> blockPoints;

	if(triker){
		std::vector<int> histHor = calcHistHor(binaryImage);
		int cursor1 = 0;
		int cursor2 = histHor.size();
		std::vector<int> thinHist = shrinkHist(histHor,cursor1,cursor2);
		
		std::vector<int> segments1;
		std::vector<int> segments2;
		
		countZero(thinHist,segments1,segments2,cursor1,cursor2);
		// cout<<"countZero end!"<<endl;

		if((segments1.size() > 0) && (segments2.size() > 0) && (segments1.size() == segments2.size())){
			if(segments1.size() == segments2.size()){
				for(int i = 0;i < segments1.size();i++){
					int y = segments1[i];
					int x = 0;
					int height = segments2[i] - segments1[i];
					int width = binaryImage.cols;

					Rect cropRect(x,y,width,height);
					cropRects.push_back(cropRect);
					// Mat image_cut = Mat(input,cropRect);
					Mat image_cut = Mat(binaryImage,cropRect);
					cropMats.push_back(image_cut);
				}
			}else{
				cout<<"error! 2 vectors' size are not equal!"<<endl;
			}
		}
	}else{
		std::vector<int> histVer = calcHistVer(binaryImage);
		int cursor1 = 0;
		int cursor2 = histVer.size();
		std::vector<int> thinHist = shrinkHist(histVer,cursor1,cursor2);
		std::vector<int> segments1;
		std::vector<int> segments2;

		countZero(thinHist,segments1,segments2,cursor1,cursor2);

		if((segments1.size() > 0) && (segments2.size() > 0) && (segments1.size() == segments2.size())){
			for(int i = 0;i < segments1.size();i++){
				int x = segments1[i];
				int y = 0;
				int height = binaryImage.rows;
				int width = segments2[i] - segments1[i];
				Rect cropRect(x,y,width,height);
				cropRects.push_back(cropRect);
				Mat image_cut = Mat(binaryImage,cropRect);
				cropMats.push_back(image_cut);
			}
		}else{
			cout<<"error! 2 vectors' size are not equal!"<<endl;
		}
	}
	
	if(cropMats.size() == 1){
		imwrite("result/" + to_string(index) + ".jpg",cropMats[0]);
	}
	/*
	if((cropMats.size() == 1) && (cropRects.size() == 1)){
		rectangle(input,cropRects[0].tl(),cropRects[0].br(),cv::Scalar(0,0,255),1,1,0);
	}*/

	cout<<"here the end of FUNC-cropImage cropMats' size is: "<<cropMats.size()<<endl;
	return cropMats;
}

void makeTree(Mat binaryImage,TreeDataType parentNode,CTree& docTree,int& totalNode,bool& tricker,std::vector<Mat>& leafBlocks,std::vector<int>& leafNodes){
	// imshow("makeTree...",binaryImage);
	// waitKey(-1);
	// CTree docTree = CTree(1);	
	// int totalNode = 1;
	std::vector<Rect> childBrects;
	std::vector<Mat> childImages = cropImage(binaryImage,tricker,totalNode,childBrects);
	// std::vector<Mat> leafImages;
    tricker = !tricker; // row & col alternative!
	TreeDataType parentIndex = totalNode;
	if(childImages.size() > 1){
		int index = 0;
		for(std::vector<Mat>::const_iterator itc = childImages.begin();itc != childImages.end();itc++){
			
			docTree.Insert(parentIndex,++totalNode,childBrects[index]);
			
			bool innerTrick = tricker;
			// cout<<"tricker: "<<tricker<<endl;
			makeTree(*itc,totalNode,docTree,totalNode,innerTrick,leafBlocks,leafNodes);
			index++;
		}
	}else{
		leafBlocks.push_back(childImages[0]);	
		leafNodes.push_back(totalNode);	
		return;
	}
}

std::vector<Rect> boundBlock(CTree& docTree,std::vector<int>& leafNodes){
	std::vector<Rect> boundRects;
	for(std::vector<int>::const_iterator it = leafNodes.begin();it != leafNodes.end();it++){
		cout<<"Now process "<<*it<<endl;
		pSTreeNode leafTreeNode = docTree.Search(docTree.pRoot,*it);
		
		int x = leafTreeNode->boundingRect.x;
		int y = leafTreeNode->boundingRect.y;
		int width = leafTreeNode->boundingRect.width;
		int height = leafTreeNode->boundingRect.height;
		
		// pSTreeNode parentTreeNode = leafTreeNode->pParent;
		pSTreeNode parentTreeNode = leafTreeNode;
		
		// x += parentTreeNode->boundingRect.x;
		// y += parentTreeNode->boundingRect.y;

		while(parentTreeNode->pParent != NULL){
			parentTreeNode = parentTreeNode->pParent;
			cout<<"parent is: "<<parentTreeNode->data<<endl;
			x += parentTreeNode->boundingRect.x;
			y += parentTreeNode->boundingRect.y; 
		}
		Rect targetRect(x,y,width,height);
		boundRects.push_back(targetRect);
	}
	cout<<"boundingRect's size is: "<<boundRects.size()<<endl;
	return boundRects;
}

/*
void boundBlock(CTree& docTree){
	std::vector<Rect> bRects;
	stack<pSTreeNode> s;
	pSTreeNode root = docTree.pRoot;
	s.push(root);
	while(){
		pSTreeNode top = s.top();
		if(top.pFirstChild == NULL){
			bRects.push_back(pFirstChild.boundingRect);
			s.pop();
		}

	}

}*/

int main(int argc, char ** argv){
	if(argc != 2){
		cout<<"usage: ./X_YCut XXX.jpg"<<endl;
		return -1;
	}

	// load the input image
	string filepath = argv[1];
	// preprocess gray-scale binarylization
	// Mat input = loadImage("1.png");
	Mat input = loadImage(filepath);
	Mat input_clone = input.clone();
	Mat grayImage = convert2gray(input);
	Mat binaryImage = binaryzation(grayImage);
	Mat binaryImageReg = ~binaryImage;
	
	// Mat boundInput = input.clone();
	// warning!
	// recursive construct the document tree
	Rect rootRect(0,0,input.cols,input.rows);
	CTree docTree = CTree(1,rootRect);
	int totalNode = 1;
	bool tricker = true;

	std::vector<Mat> leafBlocks;
	std::vector<int> leafNodes;
	makeTree(binaryImageReg,1,docTree,totalNode,tricker,leafBlocks,leafNodes);

	docTree.print();

	cout<<"leaf's size is: "<<leafBlocks.size()<<endl;
	std::vector<Rect> blockRects = boundBlock(docTree,leafNodes);

	for(std::vector<Rect>::const_iterator itr = blockRects.begin();itr!= blockRects.end();itr++){
		rectangle(input_clone,itr->tl(),itr->br(),cv::Scalar(0,0,255),1,1,0);
	}
	imshow("result!",input_clone);
	waitKey(-1);
	imwrite("result.jpg",input_clone);
}