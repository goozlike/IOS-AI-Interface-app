//
//  MatchingAlgorithmsBridge.h
//  project_app
//
//  Created by Никита Борисов on 30.03.2020.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//

#ifndef MatchingAlgorithmsBridge_h
#define MatchingAlgorithmsBridge_h


#endif /* MatchingAlgorithmsBridge_h */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface MatchingAlgorithmsBridge : NSObject

- (float *) findBest: (UIImage *) image;

- (UIImage *) match: (UIImage *) image : (int) num : (NSString *) text;

-(void)work: (NSURL *) path : (int) size;

-(void)writeGroups: (NSURL *) saveURL;

-(void)readGroups: (NSURL*) saveURL;

-(void)setText: (NSURL*) saveURL;

-(void)writeKeypoints: (NSURL*) saveURL;

-(void)readKeypoints: (NSURL*) saveURL;

-(void)confirmProject: (int) r;

- (float *) getCoords: (int) r;

-(void) applPhoto: (UIImage *) image;

@end
