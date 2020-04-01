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

int matching(cv::Mat img_1, cv::Mat img_2, std::vector<cv::KeyPoint> keypoints_1, cv::Mat descriptors_1, std::map<std::pair<float, float>, int> &points) {
    std::vector<cv::KeyPoint> keypoints_2;
    cv::Mat descriptors_2;
    cv::Ptr<cv::ORB> orb = cv::ORB::create();

    //keypoints преобразованной картинки
    orb->detect(img_2, keypoints_2);
    orb->compute(img_2, keypoints_2, descriptors_2);

    //-- matching descriptor vectors using FLANN matcher
    cv::BFMatcher matcher;
    std::vector<cv::DMatch> matches;
    cv::Mat img_matches, keypoints;
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
        std::map<std::pair<float, float>, int>::iterator it;
        for (int i = 0; i < descriptors_1.rows; i++) {
            if (matches[i].distance <= std::max(2.8 * min_dist, 0.05)) {
                it = points.find({ keypoints_1[i].pt.x, keypoints_1[i].pt.y });
                if (it != points.end()) {
                    it->second += 1;
                } else {
                    points.insert({ { keypoints_1[i].pt.x, keypoints_1[i].pt.y }, 1 });
                }
            }
        }
    }
    return 0;
}

cv::Mat show_best_points(cv::Mat input_color) {
    cv::Mat input;
    cv::cvtColor(input_color, input, 0);
    std::map<std::pair<float, float>, int> points;

    //keypoints исходной картинки
    std::vector<cv::KeyPoint> keypoints_1;
    cv::Mat descriptors_1;
    cv::Ptr<cv::ORB> orb = cv::ORB::create();
    orb->detect(input, keypoints_1);
    orb->compute(input, keypoints_1, descriptors_1);
    cv::Point2f inputQuad[4];
    cv::Point2f outputQuad[4];

    //перспектива
    for (int j = 10; j < 30; j += 2) {
        std::vector<std::vector<int>> array = {{-2*j, 4 * j, -8*j, -4*j, -10 * j, -8 * j, 2 * j, -2 * j}, { 2*j, 4 * j, -8 * j, 4*j, 10 * j, 8 * j, -2 * j, 2 * j},
        {-j, 4 * j, 8 * j, 0, 10 * j, -8 * j, -2 * j, 2 * j}, {-j, -4 * j, 8 * j, 0, 10 * j, 8 * j, -2 * j, -2 * j} };
        for (int i = 0; i < array.size(); ++i) {
            inputQuad[0] = cv::Point2f(array[i][0], array[i][1]);
            inputQuad[1] = cv::Point2f(input.cols + array[i][2], array[i][3]);
            inputQuad[2] = cv::Point2f(input.cols + array[i][4], input.rows + array[i][5]);
            inputQuad[3] = cv::Point2f(array[i][6], input.rows + array[i][7]);
            outputQuad[0] = cv::Point2f(0, 0);
            outputQuad[1] = cv::Point2f(input.cols - 1, 0);
            outputQuad[2] = cv::Point2f(input.cols - 1, input.rows - 1);
            outputQuad[3] = cv::Point2f(0, input.rows - 1);
            cv::Mat lambda = getPerspectiveTransform(inputQuad, outputQuad);
            cv::Mat output;
            cv::warpPerspective(input, output, lambda, output.size());
            matching(input, output, keypoints_1, descriptors_1, points);
        }
    }

    //поворот
    for (int i = -3; i <= 3; ++i) {
        cv::Mat output;
        cv::Mat lambda = cv::getRotationMatrix2D(cv::Point2f(input.cols / 2, input.rows / 2), 30*i, 1);
        cv::warpAffine(input, output, lambda, output.size());       
        matching(input, output, keypoints_1, descriptors_1, points);
    }

    //выбираем самые частые точки
    std::multimap<int, std::pair<float, float>> reverse_points;
    for (auto elem : points) {
        reverse_points.insert({ elem.second, elem.first });
    }
    int i = 0;
    std::multimap<int, std::pair<float, float>>::iterator it = reverse_points.end();
    std::vector<cv::KeyPoint> best_keypoints;
    while (i < 20) {
        --it;
        cv::Point2f p(it->second.first, it->second.second);
        cv::KeyPoint new_point = cv::KeyPoint(p, 5, -1, 0, 0, -1);
        best_keypoints.push_back(new_point);
        ++i;
    }

    //Возвращаем исходное фото с отмеченными точками
    cv::Mat keypoints;
    drawKeypoints(input_color, best_keypoints, keypoints, (60, 20, 220), cv::DrawMatchesFlags::DEFAULT);
    return keypoints;
