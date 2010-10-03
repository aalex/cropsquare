/*
 * cropsquare
 *
 * Copyright 2010 Alexandre Quessy <alexandre@quessy.net>
 *
 * cropsquare is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cropsquare is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the gnu general public license
 * along with cropsquare.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "config.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cv.h>
#include <highgui.h>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <algorithm> // std::min 
#include <unistd.h> //usleep

#define MAX_CONTOUR_LEVELS 10 //This will be used when displaying contours

/**
 * Reads an image, crops it, resize the result and save it as a new image. 
 */
int crop_and_resize(std::string input_image_path, std::string output_image_path, int output_width, int output_height, bool verbose, bool graphical) {
    if (verbose)
        std::cout << "Loading image " << input_image_path << std::endl;
    IplImage* input_image = cvLoadImage(input_image_path.c_str());
    if (!input_image) {
        std::cout << "Could not load image file " << input_image_path << std::endl;
        exit(1);
    }
    CvSize input_size = cvGetSize(input_image);

    // Many computer vision algorithms do not use colour information. Here, we convert from RGB to greyscale before processing further.
    IplImage *grey_image = cvCreateImage(input_size, input_image->depth, 1); // One channel, since it's greyscale
    cvCvtColor(input_image, grey_image, CV_RGB2GRAY);
    
    // We then detect edges in the image using the Canny algorithm. 
    // This will return a binary image, one where the pixel values will be 255 for 
    // pixels that are edges and 0 otherwise. This is unlike other edge detection 
    // algorithms like Sobel, which compute greyscale levels.
    double thresh1 = 63; // [0,255]
    double thresh2 = 191; // [0,255]
    
    IplImage *edge_image = cvCreateImage(input_size, input_image->depth, 1); // One channel, since it's greyscale
    cvCanny(grey_image, edge_image, thresh1, thresh2, 3); //We set the threshold values and set the window size to 3
    
    // The edges returned by the Canny algorithm might have small holes in them, which will cause some problems during contour detection.
    // The simplest way to solve this problem is to "dilate" the image. This is a morphological operator that will set any pixel in a binary image to 255 (on) 
    // if it has one neighbour that is not 0. The result of this is that edges grow fatter and small holes are filled in.
    // We re-use small_grey_image to store the results, as we won't need it anymore.
    cvDilate(edge_image, grey_image, 0, 1);
    
    // CvMemStorage and CvSeq are structures used for dynamic data collection. CvMemStorage contains pointers to the actual
    // allocated memory, but CvSeq is used to access this data. Here, it will hold the list of image contours.
    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *contours = 0;
    // Once we have a binary image, we can look for contours in it. cvFindContours will scan through the image and store connected contours in "storage".
    // "contours" will point to the first contour detected. CV_RETR_TREE means that the function will create a contour hierarchy. Each contour will contain 
    // a pointer to contours that are contained inside it (holes). CV_CHAIN_APPROX_NONE means that all the contours points will be stored. Finally, an offset
    // value can be specified, but we set it to (0,0).
    cvFindContours(grey_image, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));
    
    //This function will display contours on top of an image. We can specify different colours depending on whether the contour in a hole or not.
    // TODO: remove this:
    cvDrawContours(input_image, contours, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), MAX_CONTOUR_LEVELS, 1, CV_AA, cvPoint(0, 0));

    if (verbose)
        std::cout << "Input image size is " << input_size.width << "x" << input_size.height << std::endl;
    int final_width = std::min(input_size.width, input_size.height);
    int final_height = std::min(input_size.width, input_size.height);
    if (verbose)
        std::cout << "ROI will be from (0,0) to (" << final_width << "," << final_height << ")" << std::endl;

    // TODO:
    // // sets the Region of Interest
    // // Note that the rectangle area has to be __INSIDE__ the image */
    cvSetImageROI(input_image, cvRect(0, 0, final_width, final_height));
    // TODO:
    // // create intermediary image
    // // Note that cvGetSize will return the width and the height of ROI */
    IplImage *intermediary_image = cvCreateImage(cvGetSize(input_image), input_image->depth, input_image->nChannels);
    /* copy subimage */
    cvCopy(input_image, intermediary_image, NULL);
     /* always reset the Region of Interest to prevent issues in case we use the image later */
    cvResetImageROI(input_image);

    CvSize output_size = cvSize(output_width, output_height);
    if (verbose)
        std::cout << "Resizing image to " << output_width << "x" << output_height << std::endl;

    IplImage* output_image = cvCreateImage(output_size, input_image->depth, input_image->nChannels);
    
    //if (verbose)
    //    std::cout << "Image types: input is " << input_image.type() << " and target size image is " << output_image.type() << std::endl;
    try {
        cvResize(intermediary_image, output_image, CV_INTER_LINEAR);
    } catch (const cv::Exception &e) {
        std::cout << e.err << std::endl;
        std::cout << "Will exit with error." << std::endl;
        exit(1);
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        std::cout << "Will exit with error." << std::endl;
        exit(1);
    }
    
    std::cout << "Saving the output image of " << output_width << "x" << output_height << " as " << output_image_path << std::endl;
    try {
        cvSaveImage(output_image_path.c_str(), output_image);
    } catch (const cv::Exception &e) {
        std::cout << e.err << std::endl;
        std::cout << "Will exit with error." << std::endl;
        exit(1);
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        std::cout << "Will exit with error." << std::endl;
        exit(1);
    }
    
    if (graphical) {
        std::string window1_name = "input";
        std::string window2_name = "output";
        std::cout << "Creating graphical window." << std::endl;
        std::cout << "Press Escape to close the window." << std::endl;
        int window1 = cvNamedWindow(window1_name.c_str(), CV_WINDOW_AUTOSIZE);
        int window2 = cvNamedWindow(window2_name.c_str(), CV_WINDOW_AUTOSIZE);
        cvShowImage(window1_name.c_str(), input_image);
        cvShowImage(window2_name.c_str(), output_image);
        while (1) {
            usleep(1000); // 1 ms
            if (cvWaitKey(10) == 27) { // Escape
                break;
            }
        }
    }

    if (verbose)
        std::cout << "Freeing image data." << std::endl;
    cvDestroyAllWindows(); //releases all the windows created so far

    cvReleaseImage(&output_image); // Do I need the ampersands?
    cvReleaseImage(&intermediary_image);
    cvReleaseImage(&input_image);
    if (verbose)
        std::cout << "Success!" << std::endl;
    return 0;
}

/**
 * Main program entry
 */
int main(int argc, char *argv[]) {
    namespace po = boost::program_options;
    namespace fs = boost::filesystem;
    
    bool verbose = false;
    std::string input_image_path = "input.jpg";
    std::string output_image_path = "output.jpg";
    //std::string mode = "default"
    int output_width = 120;
    int output_height = 120;
    bool graphical = false;

    po::options_description desc(std::string(PACKAGE) + std::string(" options"));
    // std::cout << "adding options" << std::endl;
    desc.add_options()
        ("help,h", "Show this help message and exit")
        //("mode,m", po::value<std::string>()->default_value(mode), "How the region of interest is detected")
        ("input,i", po::value<std::string>()->default_value(input_image_path), "Input image file name") /* TODO: use an arg */
        ("output,o", po::value<std::string>()->default_value(output_image_path), "Output image file name") /* TODO: use an arg */
        ("version", "Show program's version number and exit")
        ("verbose,v", po::bool_switch(), "Enables a verbose output")
        ("graphical,g", po::bool_switch(), "Enables the graphical interface")
        ("width,w", po::value<int>()->default_value(output_width), "Width of the resized image.")
        ("height,H", po::value<int>()->default_value(output_height), "Height of the resized images.");
    po::variables_map options;
    po::store(po::parse_command_line(argc, argv, desc), options);
    po::notify(options);
    if (options.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }
    if (options.count("version")) {
        std::cout << PACKAGE << " " << PACKAGE_VERSION << std::endl;
        return 0; 
    }
    if (options.count("verbose"))
        verbose = options["verbose"].as<bool>();
    if (options.count("graphical")) {
        if (verbose)
            std::cout << "Enabling the graphical user interface." << std::endl;
        graphical = options["graphical"].as<bool>();
    }
    if (options.count("input")) {
        input_image_path = options["input"].as<std::string>();
        if (! fs::exists(input_image_path)) {
            std::cout << "Could not find input file " << input_image_path << "." << std::endl;
            exit(1); // exit with error
        }
        if (verbose)
            std::cout << "Input image is set to " << input_image_path << std::endl;
    }
    if (options.count("output")) {
        output_image_path = options["output"].as<std::string>();
        if (fs::exists(output_image_path)) {
            if (verbose)
                std::cout << "Will overwrite image file " << output_image_path << "." << std::endl;
        }
        if (verbose)
            std::cout << "Output image file name is set to " << output_image_path << std::endl;
    } // TODO: else use input file name with a suffix?

    if (options.count("width")) { 
        output_width = options["width"].as<int>();
        if (verbose)
            std::cout << "Output width is set to " << output_width << std::endl;
    }
    if (options.count("width")) { 
        output_height = options["height"].as<int>();
        if (verbose)
            std::cout << "Output height is set to " << output_height << std::endl;
    } // TODO: make it easy to create a square image.

    if (verbose)
        std::cout << "Done parsing the command-line options." << std::endl;
    // Now, let's do it:
    int retval = crop_and_resize(input_image_path, output_image_path, output_width, output_height, verbose, graphical);
    //std::cout << "Goodbye" << std::endl;
    return retval;
}

// class Config {
//     public:
//         bool verbose;
//         std::string input_image_path;
//         std::string output_image_path;
//         int output_width;
//         int output_height;
//         bool graphical;
//         Config();
// };
// 
// Config::Config()
//
//
# if 0
    float input_ratio = float(input_size.width) / float(input_size.height);
    float output_ratio = float(output_width) / float(output_height);
    int tmp_width = 0;
    int tmp_height = 0;
    if (input_ratio > output_ratio) { // input is wider than desired
        tmp_width = int(input_size.height * output_ratio);
        tmp_height = input_size.height;
    } else { // input is narrower than desired
        tmp_width = input_size.width;
        tmp_height = int(input_size.width * (1.0 / output_ratio)); // FIXME
    }
#endif 
