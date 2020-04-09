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

- (UIImage *) findAndDraw: (UIImage *) image : (int) num;
@end
