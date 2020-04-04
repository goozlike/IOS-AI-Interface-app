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

- (UIImage *) pls: (UIImage*) image : (int) num{
    
    // convert uiimage to mat
    cv::Mat opencvImage;
    UIImageToMat(image, opencvImage, true);
    

    
    // Run lane detection
    MatchingAlgorithms match;
    cv::Mat imageWithMatches = match.find_point(opencvImage, num, "aaaa");
    
    // convert mat to uiimage and return it to the caller
    return MatToUIImage(imageWithMatches);
}

- (float *) findBest: (UIImage *) image {
    
    // convert uiimage to mat
    cv::Mat opencvImage;
    UIImageToMat(image, opencvImage, true);
    
    MatchingAlgorithms best;
    float * points = best.best_points(opencvImage);

    return points;
    
}

@end
