#pragma once
#include <iostream>
#include <fstream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2\calib3d.hpp>
#include <opencv2\aruco.hpp>

class Calibration
{
public:
	Calibration();
	Calibration(float calibrationSquareDimension, cv::Size chessboardDimensions);
	~Calibration();

	float calibrationSquareDimension;
	float arucoSquareDimension;
	cv::Size chessboardDimensions;

	void cameraCalibration(std::vector<cv::Mat> images, cv::Size boardSize, float squareEdgeLength, cv::Mat& cameraMatrix, cv::Mat& distanceCoefficients, bool showResult = false);
	bool saveCameraCalibration(std::string name, cv::Mat cameraMatrix, cv::Mat distanceCoefficients, bool write_as_text = false);
	bool loadCameraCalibration(std::string name, cv::Mat& cameraMatrix, cv::Mat& distanceCoefficients, bool read_as_text = false);


private:
	void createKnownBoardPosition(cv::Size boardSize, float squareEdgeLength, std::vector<cv::Point3f>& corners);
	void getChessboardCorners(std::vector<cv::Mat> images, std::vector<std::vector<cv::Point2f>>& allFoundCorners, cv::Size chessboardDimensions, bool showResults = false);

};

