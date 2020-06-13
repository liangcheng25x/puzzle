#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    Mat img = imread(argv[1]);
    imshow("image", img);
    
    cout << "type: " << img.type() << endl;
    cout << "depth: " << img.depth() << endl;
    cout << "channel: " << img.channels() << endl;
    
    waitKey();
    
    return 0;
}
