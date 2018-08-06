#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#define SCALE_FACTOR 1

cv::Mat src;
cv::Mat gray;

cv::Mat dest;

int lowThreshold;
const int maxLowThreshold = 100;

std::ofstream output;

void CannyThreshold(int _a, void *_b) {
    cv::Mat detectedEdges;
    cv::blur(gray, detectedEdges, cv::Size(3, 3));
//    cv::Canny(detectedEdges, detectedEdges, lowThreshold, lowThreshold * 3, 3);
    cv::threshold(detectedEdges, detectedEdges, 127, 255, cv::THRESH_BINARY_INV);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(detectedEdges, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    for(auto con : contours) {
        if(cv::contourArea(con) < 100) continue;
        cv::Point2f center;
        float radius;

        cv::minEnclosingCircle(con, center, radius);

        cv::circle(src, center, radius, cv::Scalar(255, 0, 0));

        // now find the inscribed circle
        int dist = 0;
        int maxDist = 0;
        cv::Point nCenter;

        #pragma omp parallel for
        for(int i = 0; i < detectedEdges.cols; i++) {
            for(int j = 0; j < detectedEdges.rows; j++) {
                dist = cv::pointPolygonTest(con, cv::Point(i, j), true);
                if(dist > maxDist) {
                    maxDist = dist;
                    nCenter = cv::Point(i, j);
                }
            }
        }

//        std::cout << nCenter << std::endl;
        cv::circle(src, nCenter, maxDist, cv::Scalar(255, 0, 0));
        std::cout << "inscribed radius: " << maxDist << " circunscribed radius: "
           << radius << std::endl;

        output << maxDist << "," << radius << std::endl;
    }

    dest = cv::Scalar::all(0);

    src.copyTo(dest, detectedEdges);
//    imshow("circles", src);
}

int main(int argc, char* argv[]) {
    if(argc < 3) {
        return -1;
    }

    output.open(argv[2], std::ios_base::trunc);
    output << "Inscribed Radius,Circumscribed Radius" << std::endl;

//    cv::namedWindow("circles", cv::WINDOW_AUTOSIZE);

    cv::Mat bigsrc = cv::imread(argv[1], cv::IMREAD_COLOR);

    if(bigsrc.empty()) {
        std::cerr << "Fon" << std::endl;
        return -1;
    }

    cv::resize(bigsrc, src, cv::Size(), SCALE_FACTOR, SCALE_FACTOR);

    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    cv::createTrackbar("Threshold: ", "circles", &lowThreshold, maxLowThreshold, CannyThreshold);

    CannyThreshold(0, 0);

//    cv::waitKey();

    output.close();

    return 0;
}
