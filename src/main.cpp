/*
 * Toonloop
 *
 * Copyright 2010 Alexandre Quessy
 * <alexandre@quessy.net>
 * http://www.toonloop.com
 *
 * Toonloop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Toonloop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the gnu general public license
 * along with Toonloop.  If not, see <http://www.gnu.org/licenses/>.
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
    
    if (verbose)
        std::cout << "Saving image as " << output_image_path << std::endl;
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
    
    // TODO:
    if (graphical) {
        const char *window_name = PACKAGE;
        std::cout << "Creating graphical window." << std::endl;
        std::cout << "Press Escape to close the window." << std::endl;
        cvNamedWindow(window_name, 1);
        cvShowImage(window_name, output_image);
        while (1) {
            if(cvWaitKey(15) == 27)
                break;
            usleep(1000); // 1 ms
        }
    }

    if (verbose)
        std::cout << "Freeing image data." << std::endl;
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
        verbose = true;
    if (options.count("graphical"))
        graphical = true;
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

    // Now, let's do it:
    return crop_and_resize(input_image_path, output_image_path, output_width, output_height, verbose, graphical);
}

