#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <ncurses.h>
#include <stdlib.h>
using namespace cv;
using namespace std;

int main(int, char**)
{
    VideoCapture vid_capture(0);

    string ascii_density = ".'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

    initscr();
    curs_set(0);

    while (true) {
        Mat frame;

        bool working = vid_capture.read(frame);

        if (working == true) {

            Mat jo;

            resize(frame, jo, Size(200, 200), INTER_LINEAR );

            Mat gray;

            cvtColor(jo, gray, COLOR_BGR2GRAY);

            int maxCols = getmaxx(stdscr);
            int maxRows = getmaxy(stdscr);

            int rowsToPrint = min(gray.rows, maxRows);
            int colsToPrint = min(gray.cols, maxCols);

            int verticalPadding = (maxRows - rowsToPrint) / 2;

            WINDOW *buffer = newwin(maxRows, maxCols, 0, 0); // Create an offscreen buffer

            for (int v = 0; v < verticalPadding; v++) {
                wprintw(buffer, "\n");
            }

            for (int i = 0; i < rowsToPrint; i++) {
                int horizontalPadding = (maxCols - colsToPrint * 2) / 2;

                for (int p = 0; p < horizontalPadding; p++) {
                    wprintw(buffer, " ");
                }

                for (int j = 0; j < colsToPrint; j++) {
                    Scalar intensity = gray.at<uchar>(i,j);
                    wprintw(buffer, "%c%c", ascii_density[68*(intensity[0])/255], ' ');
                }
                wprintw(buffer, "\n");
            }

            for (int v = 0; v < maxRows - rowsToPrint - verticalPadding; v++) {
                wprintw(buffer, "\n");
            }

            // Copy buffer to screen
            wnoutrefresh(buffer);
            doupdate();

            delwin(buffer); // Delete the offscreen buffer
        }

        int key = waitKey(80);
        if (key == 'q') {
            cout << "quit";
            break;
        }
    }

    endwin();

    return 0;
}

