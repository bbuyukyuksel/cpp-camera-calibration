#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "Calibration.hpp"

#include <iostream>
#include <filesystem>
#include <string>

//#define CREATE_CHESS_BOARD

using namespace cv;
namespace fs = std::filesystem;

void createChessBoardImage(int square_block_size=75, int square_size=8) {
	int blockSize = square_block_size;
	int imageSize = blockSize * square_size;
	Mat chessBoard(imageSize, imageSize, CV_8UC3, Scalar::all(0));
	unsigned char color = 0;

	for (int i = 0; i < imageSize; i = i + blockSize) {
		color = ~color;
		for (int j = 0; j < imageSize; j = j + blockSize) {
			Mat ROI = chessBoard(Rect(i, j, blockSize, blockSize));
			ROI.setTo(Scalar::all(color));
			color = ~color;
		}
	}
	using namespace std;
	string filename = "chessboard_"s + to_string(square_block_size) + "_" + to_string(square_size) + ".png";

	imshow("Chess board", chessBoard);
	imwrite(filename, chessBoard);
	waitKey(0);
}


int main()
{
#if defined(CREATE_CHESS_BOARD)
	createChessBoardImage(150, 12);
	exit(EXIT_SUCCESS);
#else
	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
	Mat distanceCoefficients;
	float squareEdgeLength = 0.01905f; // Real world size of chessboard square dimension.
	Size chessboardDimensions = Size(8, 6);		 // Chessboard dimension.

	std::string path = "D:\\02#OTOKAR\\Projects\\ROKOS-Calibration\\RokosCalibration\\Images";
	std::vector<Mat> images;
	
	for (const auto & entry : fs::directory_iterator(path)) {
		std::cout << entry.path() << std::endl;
		images.push_back(imread(entry.path().string(), IMREAD_COLOR));
	}

	Calibration calibration = Calibration(squareEdgeLength, chessboardDimensions);
	calibration.cameraCalibration(images, chessboardDimensions, squareEdgeLength, cameraMatrix, distanceCoefficients, true);
	calibration.saveCameraCalibration("calib_01", cameraMatrix, distanceCoefficients);


	for (std::vector<Mat>::iterator iter = images.begin(); iter != images.end(); ++iter)
	{
		Mat undistorted_image;
		undistort(*iter, undistorted_image, cameraMatrix, distanceCoefficients);
		
		imshow("Original", *iter);
		imshow("Undistorted", undistorted_image);
		waitKey(0);
	}
	return  0;
#endif	
	//Mat img = imread(image_path, IMREAD_COLOR);
	//if (img.empty())
	//{
	//	std::cout << "Could not read the image: " << image_path << std::endl;
	//	return 1;
	//}
	//imshow("Display window", img);
	//int k = waitKey(0); // Wait for a keystroke in the window
	//if (k == 's')
	//{
	//	imwrite("starry_night.png", img);
	//}
	//return 0;
}