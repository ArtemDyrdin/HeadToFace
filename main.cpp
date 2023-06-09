#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/objdetect.hpp>
#include "wheel-driver/protocol/ModbusMaster.hpp"
#include <thread>
#include <chrono>

using namespace std;
using namespace cv;


//Нахождение центра прямоугольника
Point2f getcenter_rect(Point2f tl, Point2f br)
{
    Point2f center((tl.x + br.x) / 2, (tl.y + br.y) / 2);
    return center;
}


int main()
{
    /* robot::protocol::ModbusMaster mb("COM3", 115200);
     mb.Setup();
     std::this_thread::sleep_for(std::chrono::milliseconds(1000));*/

    VideoCapture video(0);
    Mat img;

    //Обнаружение лица
    CascadeClassifier facedetect;
    facedetect.load("res/haarcascades/haarcascade_frontalface_default.xml");
    vector<Rect> faces;
    //facedetect.load("res/lbpcascades/lbpcascade_frontalface_improved.xml");

    int k = 126; // константа, связывающая расстояние до лица и сторону квадрата: c = k/a, где c - расстояние, a - сторона квадрата
    int face_x = 0, face_y = 0; // абсолютный угол поворота камеры
    double leg_x, leg_y, dist, side;
    int angle_x = 0, angle_y = 0;
    int f_index = 0;

    while (true)
    {
        video >> img;

        facedetect.detectMultiScale(img, faces, 1.3, 5);

        //центр изображения
        Point center((img.cols - 1) / 2.0, (img.rows - 1) / 2.0);


        for (int i = 0; i < faces.size(); i++)
        {
            int side_ = 0;
            if (faces[i].height > side_)
            {
                side_ = faces[i].height;
                f_index = i;

            }
        }

        side = faces[f_index].height;
        leg_y = (getcenter_rect(faces[f_index].tl(), faces[f_index].br()).y - center.y) * 0.000264;
        leg_x = (getcenter_rect(faces[f_index].tl(), faces[f_index].br()).x - center.x) * 0.000264;
        dist = k / side;
        angle_y = -atan(leg_y / dist) * 180 / acos(-1);
        angle_x = -atan(leg_x / dist) * 180 / acos(-1);

        face_x += angle_x;
        face_y += angle_y;

        for (int i = 0; i < faces.size(); i++)
        {
            rectangle(img, faces[i].tl(), faces[i].br(), Scalar(50, 255, 255), 5);
            putText(img, to_string(angle_x) + " " + to_string(angle_y), Point(20, 40), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 0), 1);
        }

        /*if (face_x >= -45 && face_x <= 45)
        {
            mb.WriteMultiAnalogOutput(0x01, 0x0006, {
                    static_cast<uint16_t>(face_x)
                });
        }
        if (face_y >= -20 && face_y <= 20)
        {
            mb.WriteMultiAnalogOutput(0x01, 0x0005, {
                    static_cast<uint16_t>(face_y)
                });
        }*/

        cv::imshow("Frame", img);
        cv::waitKey(1);
    }
}
