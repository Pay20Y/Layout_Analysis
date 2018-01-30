import numpy as np
import cv2 as cv

def loadImage(path):
	input = cv.imread(path)
	grayImage = cv.cvtColor(input,cv.COLOR_BGR2GRAY)
	return grayImage

def binaryzation(grayImage):
	retval,binaryImage = cv.threshold(grayImage,125,255,cv.THRESH_BINARY)