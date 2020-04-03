//
//  LaneDetector.cpp
//  project_app
//
//  Created by Никита Борисов on 30.03.2020.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//
#include "MatchingAlgorithms.hpp"

using namespace cv;
using namespace std;

vector<pair<KeyPoint, Mat>> best_keypoints;

int MatchingAlgorithms::matching(Mat img_1, Mat img_2, vector<KeyPoint> keypoints_1, Mat descriptors_1, map<pair<float, float>, pair<int, Mat>> &points) {
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

        //считаем, сколько раз выбирали каждую точку
        map<pair<float, float>, pair<int, Mat>>::iterator it;
        for (int i = 0; i < descriptors_1.rows; i++) {
            if (matches[i].distance <= max(2.8 * min_dist, 0.05)) {
                Mat d = descriptors_1.row(i);
                it = points.find({ keypoints_1[i].pt.x, keypoints_1[i].pt.y });
                if (it != points.end()) {
                    (it->second).first += 1;
                } else {
                    points.insert({ { keypoints_1[i].pt.x, keypoints_1[i].pt.y }, { 1, d } });
                }
            }
        }
    }

    return 0;
}

vector<pair<float, float>> MatchingAlgorithms::best_points(Mat input_color) {
    Mat input;
    cvtColor(input_color, input, 0);
    map < std::pair<float, float>, std::pair<int, cv::Mat>>points;

    //keypoints исходной картинки
    vector<KeyPoint> keypoints_1;
    Mat descriptors_1;
    Ptr<ORB> orb = ORB::create();
    orb->detect(input, keypoints_1);
    orb->compute(input, keypoints_1, descriptors_1);
    //перспектива
    Point2f inputQuad[4];
    Point2f outputQuad[4];
    for (int j = 10; j < 40; j += 6) {
        vector<vector<int>> array = {{-4*j, -4*j, 4*j, -4*j, -4*j, -4*j, 4*j, -4*j}, { 4*j, 4*j, -4*j, 4*j, 4*j, 4*j, -4*j, 4*j},
        {-j, 4*j, 8*j, 0, 10*j, -8*j, -2*j, 2*j}, {-j, -4*j, 8*j, 0, 10*j, 8*j, -2*j, -2*j} };
        for (int i = 0; i < array.size(); ++i) {
            inputQuad[0] = Point2f(array[i][0], array[i][1]);
            inputQuad[1] = Point2f(input.cols + array[i][2], array[i][3]);
            inputQuad[2] = Point2f(input.cols + array[i][4], input.rows + array[i][5]);
            inputQuad[3] = Point2f(array[i][6], input.rows + array[i][7]);
            outputQuad[0] = Point2f(0, 0);
            outputQuad[1] = Point2f(input.cols - 1, 0);
            outputQuad[2] = Point2f(input.cols - 1, input.rows - 1);
            outputQuad[3] = Point2f(0, input.rows - 1);
            Mat lambda = getPerspectiveTransform(inputQuad, outputQuad);
            Mat output;
            warpPerspective(input, output, lambda, output.size());
            matching(input, output, keypoints_1, descriptors_1, points);
        }
    }

    //поворот
    for (int i = -2; i <= 2; ++i) {
        Mat output;
        Mat lambda = getRotationMatrix2D(Point2f(input.cols / 2, input.rows / 2), 30*i, 1);
        warpAffine(input, output, lambda, output.size());       
        matching(input, output, keypoints_1, descriptors_1, points);
    }

    //выбираем самые частые точки
    multimap<int, tuple<float, float, Mat>> reverse_points;
    for (auto elem : points) {
        reverse_points.insert({ elem.second.first, {elem.first.first, elem.first.second, elem.second.second} });
    }
    int i = 0;
    vector<pair<float, float>> result;
    multimap<int, tuple<float, float, Mat>>::iterator it = reverse_points.end();
    while (i < 20) {
        --it;
        Point2f p(get<0>(it->second), get<1>(it->second));
        KeyPoint new_point = KeyPoint(p, 5, -1, 0, 0, -1);
        best_keypoints.push_back({ new_point, get<2>(it->second) });
        result.push_back({ get<0>(it->second), get<1>(it->second) });
        ++i;
    }

    return result;
}

pair<int, int> MatchingAlgorithms::find_point(Mat img_color, Mat input_color, int point_num) {
    //преобразуем в чб
    Mat img;
    cvtColor(input_color, img, 0);
    Mat input;
    cvtColor(input_color, input, 0);

    //keypoints на кадре видеопотока
    vector<KeyPoint> keypoints;
    Mat descriptors;
    Ptr<ORB> orb = ORB::create();
    orb->detect(input, keypoints);
    orb->compute(input, keypoints, descriptors);

    //сопоставляем нашу точку с keypoints
    BFMatcher matcher;
    vector<DMatch> matches;
    Mat img_matches;
    vector<KeyPoint> keypoint_1 = { best_keypoints[point_num].first };
    if (!descriptors.empty()) {
        matcher.match(best_keypoints[point_num].second, descriptors, matches);
    }

    return { keypoints[matches[0].trainIdx].pt.x, keypoints[matches[0].trainIdx].pt.y };
}
