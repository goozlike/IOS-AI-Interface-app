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
#include <vector>

@implementation MatchingAlgorithmsBridge

Mat img;
vector<vector<unsigned long long>> v(10);
vector<pair<cv::KeyPoint, cv::Mat>> keypoints;


- (UIImage *) match: (UIImage*) image : (int) num : (NSString *) text{
    
    // convert uiimage to mat
    cv::Mat opencvImage;
    UIImageToMat(image, opencvImage, true);
    
    // Run lane detection
    MatchingAlgorithms match;
    const char *cfilename=[text UTF8String];
    cv::Mat imageWithMatches = match.find_point(opencvImage, num, cfilename);
    
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

-(void)writeGroups: (NSURL*) saveURL
{
    MatchingAlgorithms alg;
    vector<unsigned long> sizes(10);
    
    for (int i = 0; i < 10; ++i) {
        sizes[i] = alg.send_size(i);
    }
    
    NSMutableData *myData = [NSMutableData dataWithBytes: sizes.data() length: 10 * sizeof(unsigned  long)];
    
    
    for (int i = 0; i < 10; ++i) {
        vector<unsigned long long> *ptr = alg.send_group(i);

        NSMutableData *myDataVec = [NSMutableData dataWithBytes: ptr->data() length: sizes[i] * sizeof(unsigned long long)];
        [myData appendData:myDataVec];
    }

    [myData writeToURL:saveURL atomically:YES];
    
    unsigned long s = [myData length];
}

-(void)readGroups: (NSURL*) saveURL
{
    NSData *readedData = [NSData dataWithContentsOfURL:saveURL];
    
    unsigned long s = [readedData length];

    vector<unsigned long> sizes;
    unsigned long * sizePtr = (unsigned long *)[readedData bytes];
    
    for (int i = 0; i < 10; ++i) {
        sizes.push_back(*sizePtr);
        ++sizePtr;
    }
    
  
    unsigned long long *ptr = (unsigned long long*)sizePtr;
    
    for (int j = 0; j < 10; ++j) {
        v[j] = {};
        for (int i = 0; i < sizes[j]; ++i) {
            v[j].push_back(*ptr);
            ++ptr;
        }
    }
}


-(void)writeKeypoints: (NSURL*) saveURL
{
    MatchingAlgorithms alg;
    vector<pair<cv::KeyPoint, cv::Mat>> *ptr = alg.send_kpoints();
    vector<pair<cv::KeyPoint, cv::Mat>> v = *ptr;
        
    NSMutableArray<NSData*> *datas = [NSMutableArray new];
    
    unsigned long size = ptr->size();
    
    for (int j = 0; j < size; ++j) {
        cv::KeyPoint kp = v[j].first;
        NSData *myDataKp = [NSData dataWithBytes: &kp length: sizeof(kp)];
        [datas addObject:myDataKp];
        
        NSData *myDataMat = [NSData dataWithBytes: v[j].second.data length: 32 * sizeof(uchar)];
        [datas addObject:myDataMat];
    }

    [datas writeToURL:saveURL atomically:YES];
    
    


}

-(void)readKeypoints: (NSURL*) saveURL
{
    NSMutableArray<NSData*> *readedData = [NSMutableArray<NSData*> arrayWithContentsOfURL:saveURL];

    unsigned long c = [readedData count];
    
    keypoints.resize(c/ 2);
        
    for (int j = 0; j < c / 2; ++j) {
        KeyPoint* kptr = (cv::KeyPoint*)[readedData[2 * j] bytes];
        keypoints[j].first = *kptr;
        
        
        
        uchar *uptr = (uchar*)[readedData[2 * j + 1] bytes];
    
        keypoints[j].second = cv::Mat(1, 32, CV_8UC1, uptr);
    }
}

-(void)confirmProject: (int) r
{
    MatchingAlgorithms alg;
    alg.confirm(&keypoints, &v);
}

- (float *) getCoords: (int) r {
    MatchingAlgorithms alg;
    
    float * ptr = alg.sendKPcoordinates();
    return ptr;
}

-(void) applPhoto: (UIImage *) image
{
    cv::Mat opencvImage;
    UIImageToMat(image, opencvImage, true);
    MatchingAlgorithms alg;
    
    alg.appMat(opencvImage);
}


@end
