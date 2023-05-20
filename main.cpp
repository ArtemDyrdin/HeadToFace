#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/objdetect.hpp>
#include<iostream>


using namespace std;
using namespace cv;

//��������� ������, �� ������� ����� ������������ �����������
double rotate_angle(Point2f leye_c, Point2f reye_c)
{
    double angle = atan((reye_c.y - leye_c.y) / (reye_c.x - leye_c.x));//��� ������? ���
    return angle;
}

//���������� ������ ��������������
Point2f getcenter_rect(Point2f tl, Point2f br)
{
    Point2f center( ((br.x - tl.x) / 2.0) + tl.x, ((tl.y - br.y) / 2.0) + tl.y);
    return center;
}


int main()
{
    VideoCapture video(0);
    CascadeClassifier facedetect;
    CascadeClassifier eyedetect;
    Mat img;
    Mat rot_img;
    facedetect.load("res/haarcascades/haarcascade_frontalface_default.xml");
    eyedetect.load("res/haarcascades/haarcascade_eye_tree_eyeglasses.xml");
    //���-�� �����, ��� ����� ����� ����� ����� ������, �� ��� �� ����� 
    //facedetect.load("res/lbpcascades/lbpcascade_frontalface_improved.xml");

    while (true) {
        //��������� ����� � �����������
        //video.read(img);
        video >> img;

        //����� �����������
        Point center((img.cols - 1) / 2.0, (img.rows - 1) / 2.0);
       
        //������� ����������
        vector<Rect> faces;
        vector<Rect> eyes;
        Rect leye;
        Rect reye;
        Mat RotMat; // ������� ��������
        double angle = 0;

        //������ ���� � ����, �� ��� � ��������������� �������
        facedetect.detectMultiScale(img, faces, 1.3, 5); 
        eyedetect.detectMultiScale(img, eyes, 1.3, 5);  
        

       // ���������� ����� � ����, ���� ����������� 2 �����
        if (eyes.size() == 2)
        {
            //��������� ����� � ������ ������
            if (eyes[0].tl().x < eyes[1].tl().x)
            {
                leye = eyes[0];
                reye = eyes[1];
            }
            else
            {
                leye = eyes[1];
                reye = eyes[0];
            }
            //���� 
            angle = rotate_angle(getcenter_rect(reye.tl(), reye.br()), getcenter_rect(leye.tl(), leye.br()));
        }

        int angle_ = angle * 180 / acos(-1);

        putText(img, to_string(angle*180/acos(-1)) + " - Angle", Point(10, 40), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 0), 1);
        //����� ����������������(���� � ����)
        for (int i = 0; i < faces.size(); i++) 
        {
            rectangle(img, faces[i].tl(), faces[i].br(), Scalar(50, 255, 255), 5);
        }
        for (int i = 0; i < eyes.size(); ++i)
        {
            rectangle(img, eyes[i].tl(), eyes[i].br(), Scalar(50, 50, 255), 5);

        }
        //������� �����������
        imshow("Frame", img);
        waitKey(1);
    }
    
}
