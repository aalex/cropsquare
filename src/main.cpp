#include <string>
#include <iostream>
// OpenCV:
#include <cvaux.h>
#include <highgui.h>
#include <stdio.h>
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
    IplImage* ipl = cvLoadImage(input_image_path.c_str());
    if (verbose)
        std::cout << "Resizing image to " << output_width << "x" << output_height << std::endl;
    CvSize size = cvSize(output_width, output_height);

    IplImage* tmpsize = cvCreateImage(size, 8, 0);
    cvResize(ipl, tmpsize, CV_INTER_LINEAR);
    if (verbose)
        std::cout << "Saving image as " << output_image_path << std::endl;
    cvReleaseImage(&ipl);

    return 0;
}

