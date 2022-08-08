#include "Calibration.hpp"

using namespace std;
using namespace cv;

Calibration::Calibration()
{

}

Calibration::Calibration(float calibrationSquareDimension, Size chessboardDimensions)
{
	this->calibrationSquareDimension = calibrationSquareDimension;
	this->chessboardDimensions = chessboardDimensions;
}


Calibration::~Calibration()
{
}

void Calibration::createKnownBoardPosition(Size boardSize, float squareEdgeLength, vector<Point3f>& corners) {
	for (int i = 0; i < boardSize.height; i++) {
		for (int j = 0; j < boardSize.width; j++)
		{
			corners.push_back(Point3f(j*squareEdgeLength, i*squareEdgeLength, .0f));
		}
	}
}

void  Calibration::getChessboardCorners(vector<Mat> images, vector<vector<Point2f>>& allFoundCorners, Size chessboardDimensions, bool showResult) {
	int counter = 0;
	for (vector<Mat>::iterator iter = images.begin(); iter != images.end(); iter++) {

		vector<Point2f> pointBuf;
		bool found = findChessboardCorners(*iter, chessboardDimensions, pointBuf, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
		if (found) {

			Mat viewGray;
			cvtColor(*iter, viewGray, COLOR_BGR2GRAY);
			cornerSubPix(viewGray, pointBuf, Size(11, 11), Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.0001));

			allFoundCorners.push_back(pointBuf);
			cout << "(+) :" << std::to_string(counter) << endl;
		}
		else {
			cout << "(-) :" << std::to_string(counter) << endl;
		}

		if (showResult) {
			drawChessboardCorners(*iter, chessboardDimensions, pointBuf, found);
			imshow("Looking for Corners", *iter);
			waitKey(100);
		}
		counter++;
	}
}

void Calibration::cameraCalibration(vector<Mat> images, Size boardSize, float squareEdgeLength, Mat& cameraMatrix, Mat& distanceCoefficients, bool showResult) {
	vector<vector<Point2f>> chessboardImageSpacePoints;
	getChessboardCorners(images, chessboardImageSpacePoints, boardSize, showResult);

	vector<vector<Point3f>> worldSpaceCornerPoints(1);
	createKnownBoardPosition(boardSize, squareEdgeLength, worldSpaceCornerPoints[0]);
	worldSpaceCornerPoints.resize(chessboardImageSpacePoints.size(), worldSpaceCornerPoints[0]);

	vector<Mat> rVectors, tVectors;
	distanceCoefficients = Mat::zeros(8, 1, CV_64F);
	double error = calibrateCamera(worldSpaceCornerPoints, chessboardImageSpacePoints, boardSize, cameraMatrix, distanceCoefficients, rVectors, tVectors);
	cout << "Calibration Error:" << error << endl;
}

bool Calibration::saveCameraCalibration(string name, Mat cameraMatrix, Mat distanceCoefficients, bool write_as_text) {
	
	ostringstream fileName;
	fileName << name << ".xml";
	
	FileStorage fs = FileStorage(fileName.str(), FileStorage::WRITE);
	fs.write("cameraMatrix", cameraMatrix);
	fs.write("distanceCoefficients", distanceCoefficients);
	fs.release();
	
	if (write_as_text) {
		fileName << ".txt";
		ofstream outSteam(fileName.str());
		if (outSteam) {
			uint16_t rows = cameraMatrix.rows;
			uint16_t cols = cameraMatrix.cols;

			for (int r = 0; r < rows; r++) {
				for (int c = 0; c < cols; c++) {
					double value = cameraMatrix.at<double>(r, c);
					outSteam << value << endl;
				}
			}

			rows = distanceCoefficients.rows;
			cols = distanceCoefficients.cols;

			for (int r = 0; r < rows; r++) {
				for (int c = 0; c < cols; c++) {
					double value = distanceCoefficients.at<double>(r, c);
					outSteam << value << endl;
				}
			}

			outSteam.close();
			return true;
		}
	}

	return false;
}

bool Calibration::loadCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoefficients, bool read_as_text) {
	try
	{
		ostringstream fileName;
		fileName << name << ".xml";

		FileStorage fs = FileStorage(fileName.str(), FileStorage::READ);
		
		if (!fs.isOpened()) {
			return false;
		}
		else {
			cout << "Opened!" << endl;
		}

		fs["cameraMatrix"] >> cameraMatrix;
		fs["distanceCoefficients"] >> distanceCoefficients;
		fs.release();

		if (read_as_text) {
			cout << "Camera Matrix:" << endl;
			uint16_t rows = cameraMatrix.rows;
			uint16_t cols = cameraMatrix.cols;
			for (int r = 0; r < rows; r++) {
				for (int c = 0; c < cols; c++) {
					double value = cameraMatrix.at<double>(r, c);
					cout << value << endl;
				}
			}

			cout << "Distance Coefficients:" << endl;
			rows = distanceCoefficients.rows;
			cols = distanceCoefficients.cols;
			for (int r = 0; r < rows; r++) {
				for (int c = 0; c < cols; c++) {
					double value = distanceCoefficients.at<double>(r, c);
					cout << value << endl;
				}
			}
		}
		//fs["cameraMatrix"] >> cameraMatrix;

	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
		return false;
	}
	return true;
}