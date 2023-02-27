#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace cv;
using namespace std;

int main(int, char**)
{
    VideoCapture vid_capture(0);
    

    string ascii_density = ".'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";//"$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`\'.";
    
    while (true) {
        Mat frame;
        
        bool working = vid_capture.read(frame);

        if (working == true) {

            
            
            Mat jo;

            resize(frame, jo, Size(200, 200), INTER_LINEAR );

            namedWindow("frames",  WINDOW_NORMAL);

            Mat gray;

            cvtColor(jo, gray, COLOR_BGR2GRAY);

            imshow("frames", gray);

            ofstream currentFrame("afilename.txt");
            system("CLS");
            for (int i = 0; i < gray.rows; i++) {
                cout << "\n";
                for (int j = 0; j < gray.cols; j++) {
                    Scalar intensity = gray.at<uchar>(i,j);
                    cout << ascii_density[68*(intensity[0])/255];
                    cout << " ";
                }
            }
        }

        

        int key = waitKey(25);
        if (key == 'q') {
            cout << "quit";
            break;
        }
    }

    return 0;
}