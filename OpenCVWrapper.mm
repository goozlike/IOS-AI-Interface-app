//
//  OpenCVWrapper.m
//  project_app
//
//  Created by Никита Борисов on 26.03.2020.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//
#import <opencv2/opencv.hpp>

#import "OpenCVWrapper.h"

@implementation OpenCVWrapper

+ (NSString *)openCVVersionString {
return [NSString stringWithFormat:@"OpenCV Version %s",  CV_VERSION];
}

@end


