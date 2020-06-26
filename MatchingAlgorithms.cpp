//
//  LaneDetector.cpp
//  project_app
//
//  Created by Никита Борисов on 30.03.2020.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//
#include "MatchingAlgorithms.hpp"
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
 
using namespace cv;
using namespace std;
 
vector<pair<KeyPoint, Mat>> best_keypoints;
Mat img_color;
vector<vector<unsigned long long>> groups;
Point fine_working_point = Point(0, 0);
int counter = 0;
 
int MatchingAlgorithms::matching(Mat img_1, Mat img_2, vector<KeyPoint> keypoints_1, Mat descriptors_1, map<pair<float, float>, pair<int, Mat>> &points, Mat lambda) {
    vector<KeyPoint> keypoints_2;
    Mat descriptors_2;
    Ptr<ORB> orb = ORB::create();
 
    //keypoints преобразованной картинки
    orb->detect(img_2, keypoints_2);
    orb->compute(img_2, keypoints_2, descriptors_2);
 
    //-- matching descriptor vectors using FLANN matcher
    BFMatcher matcher;
    vector<DMatch> matches;
    Mat img_matches, keypoints;
    if (!descriptors_1.empty() && !descriptors_2.empty()) {
        matcher.match(descriptors_1, descriptors_2, matches);
        double max_dist = 0; double min_dist = 100;
 
        // calculation of max and min idstance between keypoints
        for (int i = 0; i < descriptors_1.rows; i++)
        {
            double dist = matches[i].distance;
            if (dist < min_dist) min_dist = dist;
            if (dist > max_dist) max_dist = dist;
        }
 
        //Запоминаем хорошие совпадения
        map<pair<float, float>, pair<int, Mat>>::iterator it;
        for (int i = 0; i < descriptors_1.rows; i++) {
            if (matches[i].distance <= max(2.8 * min_dist, 0.05)) {
                float x = keypoints_1[i].pt.x;
                float y = keypoints_1[i].pt.y;
                float px = (lambda.data[0] * x + lambda.data[1] * y + lambda.data[2]) / (lambda.data[2 * 3] * x + lambda.data[2 * 3 + 1] * y + lambda.data[2 * 3 + 2]);
                float py = (lambda.data[1 * 3] * x + lambda.data[1 * 3 + 1] * y + lambda.data[1 * 3 + 2]) / (lambda.data[2 * 3] * x + lambda.data[2 * 3 + 1] * y + lambda.data[2 * 3 + 2]);
                float dist1 = keypoints_2[matches[i].trainIdx].pt.x - x;
                float dist2 = keypoints_2[matches[i].trainIdx].pt.y - y;
                if (dist1 >= -10 & dist1 <= 10 & dist2 >= -10 & dist2 <= 10) {
                    Mat d = descriptors_1.row(i);
                    it = points.find({ x, y });
                    if (it != points.end()) {
                        (it->second).first += 1;
                    }
                    else {
                        points.insert({ { x, y }, { 1, d } });
                    }
                }
            }
        }
    }
 
    return 0;
}
 
float* MatchingAlgorithms::best_points(Mat input) {
    img_color = input;
    map < std::pair<float, float>, std::pair<int, cv::Mat>>points;
 
    //keypoints исходной картинки
    vector<KeyPoint> keypoints_1;
    Mat descriptors_1;
    Ptr<ORB> orb = ORB::create();
    orb->detect(input, keypoints_1);
    orb->compute(input, keypoints_1, descriptors_1);
    Point2f inputQuad[4];
    Point2f outputQuad[4];
    vector<vector<double>> arr = { {0, 0.1}, {0, 0.08}, {0, 0.05}, {0.05, 0}, {0.08, 0}, {0.1, 0} };
    // vertical perspective
    for (int i = 0; i < arr.size(); ++i) {
        inputQuad[0] = Point2f(0, 0);
        inputQuad[1] = Point2f(input.cols, 0);
        inputQuad[2] = Point2f(input.cols, input.rows);
        inputQuad[3] = Point2f(0, input.rows);
        outputQuad[0] = Point2f(0 + arr[i][0] * input.cols, 0 + arr[i][0] * input.rows);
        outputQuad[1] = Point2f(input.cols * (1 - arr[i][0]), 0 + arr[i][0] * input.rows);
        outputQuad[2] = Point2f(input.cols * (1 - arr[i][1]), input.rows * (1 - arr[i][1]));
        outputQuad[3] = Point2f(0 + arr[i][1] * input.cols, input.rows * (1 - arr[i][1]));
        Mat lambda = getPerspectiveTransform(inputQuad, outputQuad);
        Mat output;
        warpPerspective(input, output, lambda, output.size());
        matching(input, output, keypoints_1, descriptors_1, points, lambda);
    }
    // horizontal perspective
    for (int i = 0; i < arr.size(); ++i) {
        inputQuad[0] = Point2f(0, 0);
        inputQuad[1] = Point2f(input.cols, 0);
        inputQuad[2] = Point2f(input.cols, input.rows);
        inputQuad[3] = Point2f(0, input.rows);
        outputQuad[0] = Point2f(0 + arr[i][0] * input.cols, 0 + arr[i][0] * input.rows);
        outputQuad[1] = Point2f(input.cols * (1 - arr[i][1]), 0 + arr[i][1] * input.rows);
        outputQuad[2] = Point2f(input.cols * (1 - arr[i][1]), input.rows * (1 - arr[i][1]));
        outputQuad[3] = Point2f(0 + arr[i][0] * input.cols, input.rows * (1 - arr[i][0]));
        Mat lambda = getPerspectiveTransform(inputQuad, outputQuad);
        Mat output;
        warpPerspective(input, output, lambda, output.size());
        matching(input, output, keypoints_1, descriptors_1, points, lambda);
    }
 
    //поворот
    for (int i = -2; i <= 2; ++i) {
        Mat output;
        Mat lambda = getRotationMatrix2D(Point2f(input.cols / 2, input.rows / 2), 30 * i, 1);
        warpAffine(input, output, lambda, output.size());
        matching(input, output, keypoints_1, descriptors_1, points, lambda);
    }
 
    //выбираем самые частые точки
    multimap<int, tuple<float, float, Mat>> reverse_points;
    for (auto elem : points) {
        reverse_points.insert({ elem.second.first, {elem.first.first, elem.first.second, elem.second.second} });
    }
    unsigned long long i = 0;
    float * result = new float[20];
    multimap<int, tuple<float, float, Mat>>::iterator it = reverse_points.end();
    while (groups.size() != 10) {
        --it;
        Point2f p(get<0>(it->second), get<1>(it->second));
        KeyPoint new_point = KeyPoint(p, 20, -1, 0, 0, -1);
        best_keypoints.push_back({ new_point, get<2>(it->second) });
        int flag = 0;
        for (int j = 0; j < groups.size(); ++j) {
            float dist = pow(pow((new_point.pt.x - best_keypoints[groups[j][0]].first.pt.x), 2) + pow((new_point.pt.y - best_keypoints[groups[j][0]].first.pt.y), 2), 0.5);
            if (dist < pow(pow(input.cols, 2) + pow(input.rows, 2), 0.5) / 50) {
                groups[j].push_back(i);
                flag = 1;
                break;
            }
        }
        if (flag == 0) {
            groups.push_back({ i });
        }
        ++i;
    }
    for (int j = 0; j < 10; ++j) {
        result[2 * j] = best_keypoints[groups[j][0]].first.pt.x;
        result[2 * j + 1] = best_keypoints[groups[j][0]].first.pt.y;
    }
    Point o(result[2 * 6], result[2 * 6 + 1]);
    putText(input, "point", o, 1, 4, (120, 20, 250), 4, 8, false);
    namedWindow("i", WINDOW_NORMAL);
    imshow("i", input);
    waitKey();
    return result;
}
 
float * MatchingAlgorithms::sendKPcoordinates() {
    float * result = new float[20];
    for (int j = 0; j < 10; ++j) {
        result[2 * j] = best_keypoints[groups[j][0]].first.pt.x;
        result[2 * j + 1] = best_keypoints[groups[j][0]].first.pt.y;
    }
    return result;
}
 
Mat MatchingAlgorithms::find_point(Mat input, int point_num, string text) {
    Mat img = img_color;
 
    //keypoints на кадре видеопотока
    vector<KeyPoint> keypoints_2;
    Mat descriptors_2;
    Ptr<ORB> orb = ORB::create();
    orb->detect(input, keypoints_2);
    orb->compute(input, keypoints_2, descriptors_2);
    //сопоставляем нашу точку с keypoints
    BFMatcher matcher;
    vector<DMatch> matches;
    Mat result;
    Mat descriptors_1;
    for (int i = 0; i < groups[point_num].size(); ++i) {
        descriptors_1.push_back(best_keypoints[groups[point_num][i]].second);
    }
    if (!descriptors_2.empty()) {
        matcher.match(descriptors_1, descriptors_2, matches);
    }
    float dist = matches[0].distance;
    int num = 0;
    for (int i = 1; i < groups[point_num].size(); ++i) {
        if (matches[i].distance < dist) {
            dist = matches[i].distance;
            num = i;
        }
    }
    result = input;
    Point o(best_keypoints[groups[point_num][num]].first.pt.x, best_keypoints[groups[point_num][num]].first.pt.y);
    circle(img_color, o, 10, CV_RGB(255, 0, 120), 5, 8, 0);
    //namedWindow("img", WINDOW_NORMAL);
    //imshow("img", img_color);
    Scalar color = cv::Scalar(50, 0, 255, 255);
    int baseline = 0;
    Size textSize = getTextSize(text, 1, 4, 4, &baseline);
    Point org(keypoints_2[matches[num].trainIdx].pt.x - (textSize.width/2), keypoints_2[matches[num].trainIdx].pt.y + (textSize.height/2));
 
    //проверка дескриптора
    int flag = 0;
    Mat desc_1;
    desc_1.push_back(descriptors_2.row(matches[num].trainIdx));
    int best_point_sz = 0;
    for (int i = 0; i != groups.size(); ++i) {
        best_point_sz += groups[i].size();
    }
    Mat desc_2;
    for (int i = 0; i != best_point_sz; ++i) {
        desc_2.push_back(best_keypoints[i].second);
    }
    BFMatcher matcher_;
    vector<DMatch> matches_;
    matcher_.match(desc_1, desc_2, matches_);
    if (matches[0].distance < dist)
        flag = 1;
 
    //проверка расстояния с точкой на предыдущем кадре
    if ((fine_working_point.x == 0 && fine_working_point.y == 0) || counter > 5) {
        fine_working_point = org;
        counter = 0;
    }
    if (pow(pow((org.x - fine_working_point.x), 2) + pow((org.y - fine_working_point.y), 2), 0.5) > pow(pow(input.cols, 2) + pow(input.rows, 2), 0.5)/7) {
        org = fine_working_point;
        counter += 1;
    }
    else {
        fine_working_point = org;
        counter = 0;
    }
 
    //вывод текста
    putText(result, text, org, 1, 6, color, 6, 8, false);
    namedWindow("result", WINDOW_NORMAL);
    imshow("result", result);
    waitKey();
    return result;
}

 
vector<vector<unsigned long long>> * MatchingAlgorithms::send_groups() {
    return &groups;
}

vector<unsigned long long> *MatchingAlgorithms::send_group(int i) {
    return &groups[i];
}

vector<pair<KeyPoint, Mat>> *MatchingAlgorithms::send_kpoints() {

    return &best_keypoints;
}

unsigned long MatchingAlgorithms::send_size(int i) {
    
    return groups[i].size();
}

void MatchingAlgorithms::confirm(vector<pair<KeyPoint, Mat>> * new_kp, vector<vector<unsigned long long>> * new_gr) {
    best_keypoints = *new_kp;
    groups = *new_gr;
    
}

float * MatchingAlgorithms::sendKPcoordinates() {
    float * result = new float[20];
    for (int j = 0; j < 10; ++j) {
        result[2 * j] = best_keypoints[groups[j][0]].first.pt.x;
        result[2 * j + 1] = best_keypoints[groups[j][0]].first.pt.y;
    }
    return result;
}


void MatchingAlgorithms::appMat(Mat img) {
    img_color = img;
}
