#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/objdetect.hpp>
#include<iostream>
#include<math.h>


using namespace std;
using namespace cv;

// ������� �������� �����������
Mat rotate(Mat src, double angle)
{
    Mat dst;
    Point2f pt(src.cols / 2., src.rows / 2.);  
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    warpAffine(src, dst, r, Size(src.cols, src.rows));
    return dst;
}

// �������� ���������� ������
Point2f getcenter_rect(Point2f tl, Point2f br)
{
    Point2f center((tl.x + br.x) / 2, (tl.y + br.y) / 2);
    return center;
}

// ������� ���������� ������� ������ ��������� ����
int get_front_face_index(vector<Rect>& faces) {
    int front_face_index = 0;
    for (int i = 0; i < faces.size(); i++)
    {
        int max_side = 0;
        if (faces[i].height > max_side)
        {
            max_side = faces[i].height;
            front_face_index = i;
        }
    }
    return front_face_index;
}

// ������� ����������� ��������� ������������ ����
void determ_true_face_coord(vector<Rect>& faces, const int front_face_index, int angle, const int video_width, int const video_height) {
    // ���������� ��������� ������ ���� (�� ��������� �������� ������ ���� ����) � �/� � ������� � �������� �����������
    int face_X = faces[front_face_index].x + faces[front_face_index].width / 2 - video_width / 2;
    int face_Y = -faces[front_face_index].y - faces[front_face_index].height / 2 + video_height / 2;

    // ���������� ��������� ������ ���� � �/�, ���������� �������
    angle = -angle;
    int old_x = face_X * cos(angle) - face_Y * sin(angle);
    int old_y = face_Y * cos(angle) + face_X * sin(angle);
    if (abs(angle) == 35) {
        old_x = -old_x;
        old_y = -old_y;
    }

    // ����������� ������ �������� ���� ���� (�� ����� ��������� �� �������� ����� ����)
    faces[front_face_index].x = old_x - faces[front_face_index].width / 2 + video_width / 2;
    faces[front_face_index].y = -old_y - faces[front_face_index].height / 2 + video_height / 2;
}

int main()
{
    VideoCapture video(0);
    CascadeClassifier facedetect;
    Mat img;
    facedetect.load("res/haarcascades/haarcascade_frontalface_default.xml");

    const int video_width = video.get(cv::CAP_PROP_FRAME_WIDTH); // ������ �����������
    const int video_height = video.get(cv::CAP_PROP_FRAME_HEIGHT); // ������ �����������

    int k = 126; // ���������, ����������� ���������� �� ���� � ������� ��������: c = k/a, ��� c - ����������, a - ������� ��������
    int face_x = 0, face_y = 0; // ���������� ���� �������� ������
    double leg_x, leg_y, dist, side;
    int angle_x = 0, angle_y = 0;

    while (true) {
        video >> img; // ��������� �������� �����������

        Point center((img.cols - 1) / 2.0, (img.rows - 1) / 2.0); // ����� �����������
       
        vector<Rect> faces; // �������� ������� � ������

        int angle = 0; // ���� ������� ����

        // ������������ �� 35 �������� �����/������ ���� �� ������ ���� ��� ������ �� �������� � ��������� ��������� ���� ��� ��������� �����
        facedetect.detectMultiScale(rotate(img, angle), faces, 1.3, 5);
        if (faces.size() == 0) {
            angle = 35;
            facedetect.detectMultiScale(rotate(img, angle), faces, 1.3, 5);
            if (faces.size() == 0) {
                angle = -35;
                facedetect.detectMultiScale(rotate(img, angle), faces, 1.3, 5);
                if (faces.size() == 0) {
                    angle = 70;
                    facedetect.detectMultiScale(rotate(img, angle), faces, 1.3, 5);
                    if (faces.size() == 0) {
                        angle = -70;
                        facedetect.detectMultiScale(rotate(img, angle), faces, 1.3, 5);
                    }
                }
            }
        }

        if (faces.size() != 0) {

            int front_face_index = get_front_face_index(faces); // ������ ��������� ����

            // ����������� ��������� ���� �� ������������� �����������
            if (angle != 0)
                determ_true_face_coord(faces, front_face_index, angle, video_width, video_height);

            side = faces[front_face_index].height;
            leg_y = (getcenter_rect(faces[front_face_index].tl(), faces[front_face_index].br()).y - center.y) * 0.000264;
            leg_x = (getcenter_rect(faces[front_face_index].tl(), faces[front_face_index].br()).x - center.x) * 0.000264;
            dist = k / side;
            angle_y = -atan(leg_y / dist) * 180 / acos(-1);
            angle_x = -atan(leg_x / dist) * 180 / acos(-1);

            face_x += angle_x;
            face_y += angle_y;


            rectangle(img, faces[front_face_index].tl(), faces[front_face_index].br(), Scalar(0, 255, 0), 5); // ���������� ����� ����
            putText(img, to_string(angle_x) + " " + to_string(angle_y), Point(20, 40), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 0), 1); // ����� ����� �� ����
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


        //������� �����������
        imshow("Eye", img);
        waitKey(1);
    }
    
}
