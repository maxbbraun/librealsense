// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include "example.hpp"          // Include short list of convenience functions for rendering

// Capture Example demonstrates how to
// capture depth and color video streams and render them to the screen
int main(int argc, char * argv[]) try
{
    rs2::log_to_console(RS2_LOG_SEVERITY_ERROR);
    // Create a simple OpenGL window for rendering:
    window app(640, 480, "RealSense Capture");

    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;

    // Create post-processing filters.
    rs2::decimation_filter decimation_filter(2);
    rs2::disparity_transform depth_to_disparity(true);
    rs2::spatial_filter spatial_filter(0.5, 20, 2, 4);
    rs2::temporal_filter temporal_filter(0.4, 20, 3);
    rs2::hole_filling_filter hole_filling_filter(1);
    rs2::disparity_transform disparity_to_depth(false);

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;

    // Start streaming with a simple configuration
    rs2::config config;
    config.enable_stream(RS2_STREAM_DEPTH, -1, 640, 480, RS2_FORMAT_ANY, 15);
    pipe.start(config);

    while (app) // Application still alive?
    {
        rs2::frameset data = pipe.wait_for_frames().    // Wait for next set of frames from the camera
                             apply_filter(decimation_filter).
                             apply_filter(depth_to_disparity).
                             apply_filter(spatial_filter).
                             apply_filter(temporal_filter).
                             apply_filter(hole_filling_filter).
                             apply_filter(disparity_to_depth).
                             apply_filter(color_map);   // Find and colorize the depth data

        // The show method, when applied on frameset, break it to frames and upload each frame into a gl textures
        // Each texture is displayed on different viewport according to it's stream unique id
        app.show(data);
    }

    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}
