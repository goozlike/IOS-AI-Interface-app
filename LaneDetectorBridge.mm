//
//  MatchingAlgorithmsBridge.mm
//  project_app
//
//  Created by Никита Борисов on 30.03.2020.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//

#import <opencv2/opencv.hpp>
#import <opencv2/imgcodecs/ios.h>
#import <Foundation/Foundation.h>
#import "MatchingAlgorithmsBridge.h"
#include "MatchingAlgorithms.hpp"

@implementation MatchingAlgorithmsBridge

- (UIImage *) Match: (UIImage *) image {
    
    // convert uiimage to mat
    cv::Mat opencvImage;
    UIImageToMat(image, opencvImage, true);
    
    // convert colorspace to the one expected by the lane detector algorithm (RGB)
    cv::Mat convertedColorSpaceImage;
    cv::cvtColor(opencvImage, convertedColorSpaceImage, CV_RGBA2RGB);
    
    // Run lane detection
    MatchingAlgorithms match;
    cv::Mat imageWithMatches = match.matching(convertedColorSpaceImage);
    
    // convert mat to uiimage and return it to the caller
    return MatToUIImage(imageWithLaneDetected);
}

@end
