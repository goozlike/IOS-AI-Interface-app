//
//  MatchingAlgorithms.hpp
//  project_app
//
//  Created by Никита Борисов on 30.03.2020.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//

#ifndef MatchingAlgorithms_hpp
#define MatchingAlgorithms_hpp

#include <stdio.h>

#endif

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class MatchingAlgorithms {
    
public:
    
   
    /*сопоставляем keypoints исходной фотки и изменнной*/
    int matching(Mat img_1, Mat img_2, vector<KeyPoint> keypoints_1, Mat descriptors_1, map<pair<float, float>, pair<int, Mat>> &points);
    
    /*выбираем 10 лучших точек на исходном фото*/
    int * best_points(Mat input_color);
    
    /*ищем выбранную точку на кадре с видеопотока*/
    Mat find_point(Mat input_color, int point_num, string text);
};
