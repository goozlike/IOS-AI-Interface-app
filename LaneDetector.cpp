//
//  LaneDetector.cpp
//  project_app
//
//  Created by Никита Борисов on 30.03.2020.
//  Copyright © 2020 Arina Goloubitskaya. All rights reserved.
//
#include "LaneDetector.hpp"

using namespace cv;
using namespace std;

double getAverage(vector<double> vector, int nElements) {
    
    double sum = 0;
    int initialIndex = 0;
    int last30Lines = int(vector.size()) - nElements;
    if (last30Lines > 0) {
        initialIndex = last30Lines;
    }
    
    for (int i=(int)initialIndex; i<vector.size(); i++) {
        sum += vector[i];
    }
    
    int size;
    if (vector.size() < nElements) {
        size = (int)vector.size();
    } else {
        size = nElements;
    }
    return (double)sum/size;
}

Mat LaneDetector::detect_lane(Mat image) {
    
    Mat colorFilteredImage = filter_only_yellow_white(image);
    Mat regionOfInterest = crop_region_of_interest(colorFilteredImage);
    Mat edgesOnly = detect_edges(regionOfInterest);
    
    vector<Vec4i> lines;
    HoughLinesP(edgesOnly, lines, 1, CV_PI/180, 10, 20, 100);
    
    return draw_lines(image, lines);
}

Mat LaneDetector::filter_only_yellow_white(Mat image) {
    
    Mat hlsColorspacedImage;
    cvtColor(image, hlsColorspacedImage, CV_RGB2HLS);
    
    Mat yellowMask;
    Scalar yellowLower = Scalar(10, 0, 90);
    Scalar yellowUpper = Scalar(50, 255, 255);
    inRange(hlsColorspacedImage, yellowLower, yellowUpper, yellowMask);
    
    Mat whiteMask;
    Scalar whiteLower = Scalar(0, 190, 0);
    Scalar whiteUpper = Scalar(255, 255, 255);
    inRange(hlsColorspacedImage, whiteLower, whiteUpper, whiteMask);
    
    Mat mask;
    bitwise_or(yellowMask, whiteMask, mask);
    
    Mat maskedImage;
    bitwise_and(image, image, maskedImage, mask);
    
    return maskedImage;
}

Mat LaneDetector::crop_region_of_interest(Mat image) {
    
    /*
     The code below draws the region of interest into a new image of the same dimensions as the original image.
     The region of interest is filled with the color we want to filter for in the image.
     Lastly it combines the two images.
     The result is only the color within the region of interest.
     */
    
    int maxX = image.rows;
    int maxY = image.cols;
    
    Point shape[1][5];
    shape[0][0] = Point(0, maxX);
    shape[0][1] = Point(maxY, maxX);
    shape[0][2] = Point((int)(0.55 * maxY), (int)(0.6 * maxX));
    shape[0][3] = Point((int)(0.45 * maxY), (int)(0.6 * maxX));
    shape[0][4] = Point(0, maxX);
    
    Scalar color_to_filter(255, 255, 255);
    
    Mat filledPolygon = Mat::zeros(image.rows, image.cols, CV_8UC3); // empty image with same dimensions as original
    const Point* polygonPoints[1] = { shape[0] };
    int numberOfPoints[] = { 5 };
    int numberOfPolygons = 1;
    fillPoly(filledPolygon, polygonPoints, numberOfPoints, numberOfPolygons, color_to_filter);
    
    // Cobine images into one
    Mat maskedImage;
    bitwise_and(image, filledPolygon, maskedImage);
    
    return maskedImage;
}

Mat LaneDetector::draw_lines(Mat image, vector<Vec4i> lines) {
    
    vector<double> rightSlope, leftSlope, rightIntercept, leftIntercept;
    
    for (int i=0; i<lines.size(); i++) {
        Vec4i line = lines[i];
        double x1 = line[0];
        double y1 = line[1];
        double x2 = line[2];
        double y2 = line[3];
        
        double yDiff = y1-y2;
        double xDiff = x1-x2;
        double slope = yDiff/xDiff;
        double yIntecept = y2 - (slope*x2);
        
        if ((slope > 0.3) && (x1 > 500)) {
            rightSlope.push_back(slope);
            rightIntercept.push_back(yIntecept);
        } else if ((slope < -0.3) && (x1 < 600)) {
            leftSlope.push_back(slope);
            leftIntercept.push_back(yIntecept);
        }
    }
    
    double leftAvgSlope = getAverage(leftSlope, 30);
    double leftAvgIntercept = getAverage(leftIntercept, 30);
    double rightAvgSlope = getAverage(rightSlope, 30);
    double rightAvgIntercept = getAverage(rightIntercept, 30);
    
    int leftLineX1 = int(((0.65*image.rows) - leftAvgIntercept)/leftAvgSlope);
    int leftLineX2 = int((image.rows - leftAvgIntercept)/leftAvgSlope);
    int rightLineX1 = int(((0.65*image.rows) - rightAvgIntercept)/rightAvgSlope);
    int rightLineX2 = int((image.rows - rightAvgIntercept)/rightAvgSlope);
    
    Point shape[1][4];
    shape[0][0] = Point(leftLineX1, int(0.65*image.rows));
    shape[0][1] = Point(leftLineX2, int(image.rows));
    shape[0][2] = Point(rightLineX2, int(image.rows));
    shape[0][3] = Point(rightLineX1, int(0.65*image.rows));
    
    const Point* polygonPoints[1] = { shape[0] };
    int numberOfPoints[] = { 4 };
    int numberOfPolygons = 1;
    Scalar fillColor(0, 0, 255);
    fillPoly(image, polygonPoints, numberOfPoints, numberOfPolygons, fillColor);
    
    Scalar rightColor(0,255,0);
    Scalar leftColor(255,0,0);
    line(image, shape[0][0], shape[0][1], leftColor, 10);
    line(image, shape[0][3], shape[0][2], rightColor, 10);
    
    return image;
}

Mat LaneDetector::detect_edges(Mat image) {
    
    Mat greyScaledImage;
    cvtColor(image, greyScaledImage, CV_RGB2GRAY);
    
    Mat edgedOnlyImage;
    Canny(greyScaledImage, edgedOnlyImage, 50, 120);
    
    return edgedOnlyImage;
}

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
