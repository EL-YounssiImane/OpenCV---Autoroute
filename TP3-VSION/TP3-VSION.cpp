//
//  main.cpp
//  VSION-TP1
//
//  Created by Jean-Marie Normand on 04/02/2016.
//  Copyright © 2016 Jean-Marie Normand. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>

// no idea why on the virtual box the keycode is completely wrong
#define ESC_KEY 27 // should be 27
#define Q_KEY 	113 // should be 113

using namespace std;
using namespace cv;


string videoName;
VideoCapture cap;
int nbFrames;
double fps = 30;
bool videoRead = false;
Mat im;
Mat imGray;
Mat grayImage;
Mat grayIm;

Mat grayA;
Mat grayB;
Mat diff_image, dst, dilated;
Mat temp2;
Mat frame;
float duration;

int height;
int width;

vector<Mat> videoframes;
vector<Mat> videoframes1;

int M, M1;



int main(int argc, const char* argv[]) {
	int qst;
	cout << "Saisissez le numero de la question?" << endl;
	cin >> qst;
	
	if (argc > 1) {
		videoName = argv[1];
		cout << "video name: " << videoName << endl;
	}
	else {
		videoName = "video.avi";
	}

	// Reading the image (and forcing it to grayscale)	   
	cap.open(videoName);

	// Making sure the capture has opened successfully
	if (cap.isOpened()) {
		cap.read(im);
		
	}
	else {
		// capture opening has failed we cannot do anything :'(
		return 1;
	}

	// Creating a window to display some images
	//namedWindow("Original video");

	
	// Waiting for the user to press ESCAPE before exiting the application	
	int key = 0;

	switch (qst) {
	case 1:
		while ((key != ESC_KEY) && (key != Q_KEY)) {
			//Getting the total number of frames

			imshow("Original video", im);
			//imshow("Gray video", imGray);

			cap.read(im);
			key = waitKey(30); // video is XXfps
		}
		break;
	case 2 :
		//Getting the total number of frames
		nbFrames = cap.get(CAP_PROP_FRAME_COUNT);
		cout << "____________________________" << endl;
		cout << "Total Number of images are:" << nbFrames << endl;
		cout << "____________________________" << endl;
		break;
	case 3:
		nbFrames = cap.get(CAP_PROP_FRAME_COUNT);
		duration = nbFrames / fps;
		cout << "____________________________" << endl;
		cout << "The duration of the video is : " << duration << endl;
		cout << "____________________________" << endl;
		break;
	case 4 :
		width = cap.get(CAP_PROP_FRAME_WIDTH);
		height = cap.get(CAP_PROP_FRAME_HEIGHT);
		cout << "___________________" << endl;
		cout << "Resolution video : " << width << "x" << height << endl;
		cout << "___________________" << endl;
		break;
	case 5 :
		nbFrames = cap.get(CAP_PROP_FRAME_COUNT);
		for (int i = 0; i < nbFrames; i++) {
			imshow("Original video", im);
			cap.read(im);
			videoframes.push_back(im.clone());
			key = waitKey(30); // video is XXfps
			
		}
		cout << "___________________" << endl;
		cout << "La taille du vecteur est " << videoframes.size() << endl;
		cout << "___________________" << endl;
		break;
	case 6 :
		namedWindow("Gray video");
		while ((key != ESC_KEY) && (key != Q_KEY)) {
			//Getting the total number of frames
			imshow("Original video", im);
			cap.read(im);
			// Turning im into grayscale and storing it in imGray
			cvtColor(im, imGray, COLOR_BGR2GRAY);
			imshow("Gray video", imGray);
			//cout << im.at<unsigned char>(1,0) << endl;
			key = waitKey(30); // video is XXfps
		}
		break;

	case 8:
	case 9:
	{
		nbFrames = cap.get(CAP_PROP_FRAME_COUNT);
		cout << "_________________________________________________________________________" << endl;
		cout << "Saisissez le nombre d'images sur lesquelles la moyenne doit etre calculee" << endl;
		cin >> M1;
		if (M1 > nbFrames) M1 = nbFrames;
		for (int i = 0; i < M1; i++) {
			imshow("Original video", im);
			cap.read(im);
			videoframes.push_back(im.clone());
			key = waitKey(30); // video is XXfps

		}
		// Create a 0 initialized image to use as accumulator
		Mat meanImage1(videoframes[0].rows, videoframes[0].cols, CV_64FC3);
		meanImage1.setTo(Scalar(0, 0, 0, 0));

		// Use a temp image to hold the conversion of each input image to CV_64FC3
		// This will be allocated just the first time, since all your images have
		// the same size.
		Mat temp1;
		for (int i = 0; i < videoframes.size(); ++i)
		{
			// Convert the input images to CV_64FC3 ...
			videoframes[i].convertTo(temp1, CV_64FC3);

			// ... so you can accumulate
			meanImage1 += temp1;
		}

		// Convert back to CV_8UC3 type, applying the division to get the actual mean
		meanImage1.convertTo(meanImage1, CV_8U, 1. / videoframes.size());
		//
		// Show result
		imshow("Mean image", meanImage1);
		waitKey();
		break;
	}
		


	case 10:
	{ 
		nbFrames = cap.get(CAP_PROP_FRAME_COUNT);
		cout << "___________________" << endl;
		cout << "Saisissez le nombre d'images sur lesquelles la moyenne doit etre calculee" << endl;
		cin >> M;
		if (M > nbFrames) M = nbFrames-1;
		for (int i = 0; i < M; i++) {
			//imshow("Original video", im);
			cap.read(im);
			cvtColor(im, grayIm, COLOR_BGR2GRAY);
			videoframes.push_back(grayIm.clone());
		}
		Mat meanImage(videoframes[0].rows, videoframes[0].cols, CV_8UC1);
		for (int i = 0; i < videoframes[0].rows; i++)
		{
			for (int j = 0; j < videoframes[0].cols; j++)
			{
				float mean = 0;
				for (int k = 0; k < M; k++)
				{
					mean += videoframes[k].at<uchar>(i, j);
				}
				meanImage.at<uchar>(i, j) = mean / (float)M;
			}
		}

		Mat mask(videoframes[0].rows, videoframes[0].cols, CV_8UC1), kernel;
		int thresh;
		thresh = meanImage.at<uchar>(meanImage.rows / 2, meanImage.cols / 2); cout << thresh << endl;
		for (int i = 0; i < videoframes[0].rows; i++)
		{
			for (int j = 0; j < videoframes[0].cols; j++)
			{
				if (meanImage.at<uchar>(i, j) < 1.85 * thresh && meanImage.at<uchar>(i, j) > 0.85 * thresh && i > 50 && j - i < 150)
				{
					mask.at<uchar>(i, j) = 255;
				}
				else
				{
					mask.at<uchar>(i, j) = 0;
				}
			}
		}

		kernel = getStructuringElement(MORPH_RECT, Size(2, 2));
		erode(mask, mask, kernel);
		///imshow("Mask Result of Erosion", mask);

		kernel = getStructuringElement(MORPH_RECT, Size(19, 19));
		dilate(mask, mask, kernel);
		Mat res;
		int i = 0;
		while ((key != ESC_KEY) && (key != Q_KEY)) {
			videoframes[i].copyTo(res, mask);
			imshow("After masking", res);
			imshow("meanIMAGE", meanImage);
			imshow("Mask", mask);
			key = waitKey(30); // video is XXfps
			i++;
			if (i == videoframes.size())
				i = 0;
		}
		break;
	}

	case 11:
	{
		for (int i = 0; i < 613; i++) {
			//imshow("Original video", im);
			cap.read(im);
			cvtColor(im, grayIm, COLOR_BGR2GRAY);
			videoframes.push_back(grayIm.clone());
		}
		Mat kernel;
		int i = 0;
		while ((key != ESC_KEY) && (key != Q_KEY)) {
			absdiff(videoframes[i], videoframes[i + 1], diff_image);
			kernel = getStructuringElement(MORPH_RECT, Size(1, 1));
			erode(diff_image, diff_image, kernel);

			kernel = getStructuringElement(MORPH_RECT, Size(1, 1));
			dilate(diff_image, diff_image, kernel);
			imshow("Car detection", diff_image);
			key = waitKey(30); // video is XXfps
			i++;
			if (i == videoframes.size())
				i = 0;
		}
		break;
	}
	case 12:
	{
		for (int i = 0; i < 613; i++) {
			//imshow("Original video", im);
			cap.read(im);
			cvtColor(im, grayIm, COLOR_BGR2GRAY);
			videoframes.push_back(grayIm.clone());
		}
		Mat kernel;
		Point p1(0, 180), p2(384, 180);
		int i = 0;
		while ((key != ESC_KEY) && (key != Q_KEY)) {
			absdiff(videoframes[i], videoframes[i + 1], diff_image);
			kernel = getStructuringElement(MORPH_RECT, Size(1, 1));
			erode(diff_image, diff_image, kernel);

			kernel = getStructuringElement(MORPH_RECT, Size(1, 1));
			dilate(diff_image, diff_image, kernel);
			line(diff_image, p1, p2, Scalar(255, 0, 0), 2, LINE_AA);
			imshow("Output", diff_image);
			key = waitKey(30); // video is XXfps
			i++;
			if (i == videoframes.size()-1)
				i = 0;
		}
		break;
	}
	case 13:
	{
		for (int i = 0; i < 613; i++) {
			//imshow("Original video", im);
			cap.read(im);
			cvtColor(im, grayIm, COLOR_BGR2GRAY);
			videoframes.push_back(grayIm.clone());
		}
		int nLabels=0;
		int i = 0;
		Mat kernel;
		while ((key != ESC_KEY) && (key != Q_KEY)) {
			absdiff(videoframes[i], videoframes[i + 1], diff_image);
			kernel = getStructuringElement(MORPH_RECT, Size(1, 1));
			erode(diff_image, diff_image, kernel);

			kernel = getStructuringElement(MORPH_RECT, Size(1, 1));
			dilate(diff_image, diff_image, kernel);
			Mat testCC(diff_image.size(), CV_32S);
			// on compte le nombre de composantes connexes
			diff_image = diff_image > 128;
			nLabels = connectedComponents(diff_image, testCC, 4);
			// Les lignes suivantes sont pour l’affichage de ces composantes connexes !
			std::vector<Vec3b> colors(nLabels);
			colors[0] = Vec3b(0, 0, 0); //background color
			// on genere des couleurs aleatoires
			for (int label = 1; label < nLabels; ++label) {
				colors[label] = Vec3b(0, 255, 0);
			}
			// on affiche dans l’image ’dstCC’ les composantes connexes en couleur
			Mat dstCC(videoframes[i].size(), CV_8UC3);
			for (int r = 0; r < dstCC.rows; ++r) {
				for (int c = 0; c < dstCC.cols; ++c) {
					int label = testCC.at<int>(r, c);
					Vec3b& pixel = dstCC.at<Vec3b>(r, c);
					pixel = colors[label];
				}
			}
			// on affiche l’image resultat
			imshow("Connected comps", dstCC);
			waitKey(30);
			i++;
			if (i == videoframes.size() - 1)
				break;
		}
		cout << "___________________________" << endl;
		cout << "NLabels: " << nLabels << endl;
		cout << "___________________________" << endl;
		break;
	}
	case 15:
		for (int i = 0; i < 613; i++) {
			//imshow("Original video", im);
			cap.read(im);
			cvtColor(im, grayIm, COLOR_BGR2GRAY);
			videoframes.push_back(grayIm.clone());
		}
		Mat kernel;
		Point p1(0, 180), p2(384, 180);
		int i = 0;
		while ((key != ESC_KEY) && (key != Q_KEY)) {
			absdiff(videoframes[i], videoframes[i + 1], diff_image);
			Mat output = Mat::zeros(diff_image.size(), CV_8UC3);
			/// Find contours
			vector<vector<Point> > contours;
			vector<Vec4i> hierarchy;
			RNG rng(12345);
			// detection de contours dans ’img’
			findContours(diff_image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
			// Draw contours
			Mat drawing = Mat::zeros(videoframes[i].size(), CV_8UC3);
			for (int i = 0; i < contours.size(); i++)
			{
				Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
				drawContours(output, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
			}
			imshow("output", output);
			key = waitKey(30); // video is XXfps
			i++;
			if (i == videoframes.size() - 1)
				i = 0;
		}
		break;
		
	}
	

	// Destroying all OpenCV windows
	destroyAllWindows();

	return 0;
}

