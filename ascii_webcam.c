#include <ncurses.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#define WIDTH 640
#define HEIGHT 480

struct buffer {
    void *start;
    size_t length;
};

int xioctl(int fd, int request, void *arg) {
    int r;
    do {
        r = ioctl(fd, request, arg);
    } while (r == -1 && errno == CINTR);
    return r;
}

int main() {
    const char ascii_density[] = ".'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
    
    int fd = open("/dev/video0", O_RDWR);
    if (fd == -1) {
        perror("Error opening video device");
        return 1;
    }
    
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    
    if (xioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("Error setting format");
        close(fd);
        return 1;
    }
    
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    
    if (xioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("Error requesting buffers");
        close(fd);
        return 1;
    }
    
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    
    if (xioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
        perror("Error querying buffer");
        close(fd);
        return 1;
    }
    
    void *buffer_start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    if (buffer_start == MAP_FAILED) {
        perror("Error mapping buffer");
        close(fd);
        return 1;
    }
    
    if (xioctl(fd, VIDIOC_STREAMON, &buf.type) == -1) {
        perror("Error starting stream");
        munmap(buffer_start, buf.length);
        close(fd);
        return 1;
    }
    
    // Initialize ncurses
    initscr();
    curs_set(0);
    nodelay(stdscr, TRUE);
    noecho();
    keypad(stdscr, TRUE);
    
    int maxCols, maxRows;
    getmaxyx(stdscr, maxRows, maxCols);
    
    // We'll resize the image to 200x200 in software
    int target_width = 200;
    int target_height = 200;
    
    // Allocate memory for resized image (grayscale)
    unsigned char *resized_gray = malloc(target_width * target_height);
    if (!resized_gray) {
        fprintf(stderr, "Error allocating memory\n");
        endwin();
        close(fd);
        return 1;
    }
    
    while (1) {
        // Queue the buffer
        if (xioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            perror("Error queueing buffer");
            break;
        }
        
        // Dequeue the buffer (wait for image)
        if (xioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
            perror("Error dequeueing buffer");
            break;
        }
        
        // Now buffer_start contains the image in YUYV format
        // We'll convert to grayscale and resize
        unsigned char *yuyv = (unsigned char *)buffer_start;
        
        // Simple resize and convert to grayscale (just take the Y component and skip every other pixel)
        for (int i = 0; i < target_height; i++) {
            int src_y = i * HEIGHT / target_height;
            for (int j = 0; j < target_width; j++) {
                int src_x = j * WIDTH / target_width;
                // YUYV format: Y0 U0 Y1 V0 Y2 U2 Y3 V2 ...
                // So the Y (luminance) is at every other byte
                int src_index = (src_y * WIDTH + src_x) * 2;
                resized_gray[i * target_width + j] = yuyv[src_index];
            }
        }
        
        int rowsToPrint = (target_height < maxRows) ? target_height : maxRows;
        int colsToPrint = (target_width < maxCols/2) ? target_width : maxCols/2;
        int verticalPadding = (maxRows - rowsToPrint) / 2;
        
        clear();
        
        for (int v = 0; v < verticalPadding; v++) {
            printw("\n");
        }
        
        for (int i = 0; i < rowsToPrint; i++) {
            int horizontalPadding = (maxCols - colsToPrint * 2) / 2;
            
            for (int p = 0; p < horizontalPadding; p++) {
                printw(" ");
            }
            
            for (int j = 0; j < colsToPrint; j++) {
                unsigned char intensity = resized_gray[i * target_width + j];
                int index = (68 * intensity) / 255;
                if (index >= (int)strlen(ascii_density)) index = strlen(ascii_density) - 1;
                printw("%c ", ascii_density[index]);
            }
            printw("\n");
        }
        
        refresh();
        
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }
        
        usleep(80000);
    }
    
    free(resized_gray);
    munmap(buffer_start, buf.length);
    close(fd);
    endwin();
    
    return 0;
}
