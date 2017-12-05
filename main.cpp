#include<iostream>
#include<opencv2\opencv.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;


void YCgCr(double b, double g, double r, double &y, double &cg, double& cr)
{
	y = 16 + 0.256*r + 0.5041*g + 0.0979*b;
	cg = 128 - 0.3180*r - 0.4392*g - 0.1212*b;
	cr = 128 + 0.4392*r - 0.3677*g - 0.0714*b; 
	
}
void Dilation(Mat & src,int dilationElem,int dilationSize)
{
	Mat temp;
	int dilationType;
	if (dilationElem == 0){ dilationType = MORPH_RECT; }
	else if (dilationElem == 1){ dilationType = MORPH_CROSS; }
	else if (dilationElem == 2) { dilationType = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(dilationType,
		Size(2 * dilationSize + 1, 2 * dilationSize + 1),
		Point(dilationSize, dilationSize));
	/// Apply the dilation operation
	dilate(src, src, element);
	
}
Mat lightening(Mat img)
{
	Mat src,dst,ycrcb;
	vector<Mat> channels;
	img.copyTo(src);
	
	if (img.channels() == 3){
		cout << "converting to gray from 3\n";
		//convert to 1 channel because of equalizehist 
		cvtColor(src, ycrcb, CV_BGR2YCrCb);
		split(ycrcb, channels);

		//apply histogram equalization to light the pic 
		//there is another algorithms like reference white ( it is slow )
		//so i decided to use histogram equalization instead 
		equalizeHist(channels[0], channels[0]);
		//merge 3 channels together and convert to BGR again
		merge(channels, ycrcb);
		cvtColor(ycrcb, dst, CV_YCrCb2BGR);
	}
	else if (img.channels() == 4){
		cout << "Error it is BGRA image \n";
	//	cvtColor(src, src, CV_BGRA2HSV);
	}
	
	return dst;
}
void skinDetection(Mat img, Mat& binImg)
{
	double y, cg, cr;
	vector<Mat>channels;
	split(img, channels);
	//here i will segment the picture to skin regions and non skin regions
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			//1-transform to Cg-Cr plane
			YCgCr(img.at<Vec3b>(i, j)[0], img.at<Vec3b>(i, j)[1], img.at<Vec3b>(i, j)[2], y, cg, cr);
			//2-check if it is a skkin rigion or not
			if ((y > 80) && (cr < 175) && (cr>135))
			{
				//skin region
				binImg.at<Vec3b>(i, j)[0] =255;
				binImg.at<Vec3b>(i, j)[1] =255;
				binImg.at<Vec3b>(i, j)[2] =255;
			}
			else
			{   
				//non skin region
				binImg.at<Vec3b>(i, j)[0] = 0;
				binImg.at<Vec3b>(i, j)[1] = 0;
				binImg.at<Vec3b>(i, j)[2] = 0;
			}

		}

	}
	


}

void AdaBoost(Mat & img)
{

}
void faceDetection(Mat img, Mat & binImg)
{
	Mat temp;
	//1- lightening
	binImg=lightening(img);
	//get the lightening img to the rignal one to deal with it 
	img = binImg;
	img.copyTo(temp);
	//2-skin detection
	skinDetection(img, binImg);
    //3-morphological operation
	Dilation(binImg,0,2);
	//4-AdaBoost algo
}
int main(int argc, char** argv)
{
	//define matrices and variables 
	Mat img,binImg;

	// load image 
	img = imread("test.jpg", -1);
	//resize the original image to suitable size 
	resize(img, img, Size(500,500));

	//apply face detection
	faceDetection(img,binImg);

	//dispaly images 
	namedWindow("Original Image", WINDOW_AUTOSIZE);// Create a window for display.
	imshow("Original Image", img);

	namedWindow("Binary Image", WINDOW_AUTOSIZE);// Create a window for display.
	imshow("Binary Image", binImg);
	waitKey(0);
	return 0;
}