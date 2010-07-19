#include <string>
#include <iostream>
// OpenCV:
//#include <cvaux.h>
#include <highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <cv.h>
//#include <cxcore.h>
//#include <cxcore.h>

int main(int argc, char *argv[]) {
    bool verbose = true;
    std::string input_image_path = "hello.jpg";
    std::string output_image_path = "output.jpg";

    int output_width = 128;
    int output_height = 128;
    if (verbose)
        std::cout << "Loading image " << input_image_path << std::endl;
    IplImage* input_image = cvLoadImage(input_image_path.c_str());
    if (!input_image) {
        std::cout << "Could not load image file " << input_image_path << std::endl;
        exit(1);
    }
    CvSize size = cvSize(output_width, output_height);
    if (verbose)
        std::cout << "Resizing image to " << output_width << "x" << output_height << std::endl;

    //IplImage* output_image = cvCreateImage(size, 8, 0);
    IplImage* output_image = cvCreateImage(size, IPL_DEPTH_32S, 3);
    try {
        cvResize(input_image, output_image, CV_INTER_LINEAR);
    } catch (std::exception &e) { // cv::Exception &e) {
        //const char* error_message = e.what();
        std::cout << e.what() << std::endl;
        exit(1);
    }
    if (verbose)
        std::cout << "Saving image as " << output_image_path << std::endl;
    cvReleaseImage(&input_image);

    return 0;
}

