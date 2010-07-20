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

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char *argv[]) {
    bool verbose = false;
    std::string input_image_path = "input.jpg";
    std::string output_image_path = "output.jpg";
    //std::string mode = "default"
    int output_width = 120;
    int output_height = 120;

    po::options_description desc(std::string(PACKAGE) + std::string(" options"));
    // std::cout << "adding options" << std::endl;
    desc.add_options()
        ("help,h", "Show this help message and exit")
        //("mode,m", po::value<std::string>()->default_value(mode), "How the region of interest is detected")
        ("input,i", po::value<std::string>()->default_value(input_image_path), "Input image") /* TODO: use an arg */
        ("output,o", po::value<std::string>()->default_value(output_image_path), "Output image") /* TODO: use an arg */
        ("version", "Show program's version number and exit")
        ("verbose,v", po::bool_switch(), "Enables a verbose output")
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
    if (options.count("verbose")) {
        verbose = true;
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

    // Now, let's do it:
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

    IplImage* image_of_desired_size = cvCreateImage(size, input_image->depth, input_image->nChannels);
    
    //if (verbose)
    //    std::cout << "Image types: input is " << input_image.type() << " and target size image is " << image_of_desired_size.type() << std::endl;
    try {
        cvResize(input_image, image_of_desired_size, CV_INTER_LINEAR);
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
        cvSaveImage(output_image_path.c_str(), image_of_desired_size);
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
        std::cout << "Freeing image data." << std::endl;
    cvReleaseImage(&image_of_desired_size);
    cvReleaseImage(&input_image);
    if (verbose)
        std::cout << "Success!" << std::endl;

    return 0;
}

