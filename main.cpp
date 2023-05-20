#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/objdetect.hpp>
#include<iostream>


using namespace std;
using namespace cv;

//вычисл€ем уголок, на который будем поворачивать изображение
double rotate_angle(Point2f leye_c, Point2f reye_c)
{
    double angle = atan((reye_c.y - leye_c.y) / (reye_c.x - leye_c.x));//так просто? чзх
    return angle;
}

//нахождение центра пр€моугольника
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
    //где-то читал, что штука снизу лучше штуки сверху, но это не точно 
    //facedetect.load("res/lbpcascades/lbpcascade_frontalface_improved.xml");

    while (true) {
        //считываем кадры в изображение
        //video.read(img);
        video >> img;

        //центр изображени€
        Point center((img.cols - 1) / 2.0, (img.rows - 1) / 2.0);
       
        //немного объ€влений
        vector<Rect> faces;
        vector<Rect> eyes;
        Rect leye;
        Rect reye;
        Mat RotMat; // матрица поворота
        double angle = 0;

        //детект лица и глаз, всЄ идЄт в соответствующие векторы
        facedetect.detectMultiScale(img, faces, 1.3, 5); 
        eyedetect.detectMultiScale(img, eyes, 1.3, 5);  
        

       // определ€ем глаза и угол, если задетекчено 2 глаза
        if (eyes.size() == 2)
        {
            //определим левый и правый глазки
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
            //угол 
            angle = rotate_angle(getcenter_rect(reye.tl(), reye.br()), getcenter_rect(leye.tl(), leye.br()));
        }

        int angle_ = angle * 180 / acos(-1);

        putText(img, to_string(angle*180/acos(-1)) + " - Angle", Point(10, 40), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 0), 1);
        //вывод пр€моугольничков(лица и глаз)
        for (int i = 0; i < faces.size(); i++) 
        {
            rectangle(img, faces[i].tl(), faces[i].br(), Scalar(50, 255, 255), 5);
        }
        for (int i = 0; i < eyes.size(); ++i)
        {
            rectangle(img, eyes[i].tl(), eyes[i].br(), Scalar(50, 50, 255), 5);

        }
        //выводим изображение
        imshow("Frame", img);
        waitKey(1);
    }
    
}
