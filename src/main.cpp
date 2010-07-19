#include <string>
#include <iostream>
// OpenCV:
//#include <cvaux.h>
#include <stdio.h>
#include <stdlib.h>

#include <cv.h>
#include <highgui.h>
//#include <cxcore.h>
//#include <cxcore.h>

int main(int argc, char *argv[]) {
    bool verbose = true;
    std::string input_image_path = "hello.jpg";
    std::string output_image_path = "output.jpg";

    if (verbose)
        std::cout << "Loading image " << input_image_path << std::endl;
    IplImage* input_image = cvLoadImage(input_image_path.c_str());
    if (!input_image) {
        std::cout << "Could not load image file " << input_image_path << std::endl;
        exit(1);
    }
    int output_width = 120; //input_image->width / 2;
    int output_height = 120; //input_image->height / 2;
    CvSize size = cvSize(output_width, output_height);
    if (verbose)
        std::cout << "Resizing image to " << output_width << "x" << output_height << std::endl;

    //IplImage* image_of_desired_size = cvCreateImage(size, 8, 0);
    IplImage* image_of_desired_size = cvCreateImage(size, input_image->depth, input_image->nChannels); // IPL_DEPTH_32S
    
    //if (verbose)
    //    std::cout << "Image types: input is " << input_image.type() << " and target size image is " << image_of_desired_size.type() << std::endl;
    try {
        cvResize(input_image, image_of_desired_size, CV_INTER_LINEAR);
    } catch (const std::exception &e) { // cv::Exception &e) {
        //const char* error_message = e.what();
        std::cout << e.what() << std::endl;
        std::cout << "Will exit with error." << std::endl;
        exit(1);
    }
    
    if (verbose)
        std::cout << "Saving image as " << output_image_path << std::endl;
    cvSaveImage(output_image_path.c_str(), &image_of_desired_size);
    
    if (verbose)
        std::cout << "Freeing image data." << std::endl;
    cvReleaseImage(&image_of_desired_size);
    cvReleaseImage(&input_image);

    return 0;
}

