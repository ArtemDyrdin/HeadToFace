#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/objdetect.hpp>
#include<iostream>
#include "wheel-driver/protocol/ModbusMaster.hpp"
#include <thread>
#include <chrono>

using namespace std;
using namespace cv;

// ôóíêöèÿ ïîâîðîòà èçîáðàæåíèÿ
Mat rotate(Mat src, double angle)
{
    Mat dst;
    Point2f pt(src.cols / 2., src.rows / 2.);  
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    warpAffine(src, dst, r, Size(src.cols, src.rows));
    return dst;
}

//Íàõîæäåíèå öåíòðà ïðÿìîóãîëüíèêà
Point2f getcenter_rect(Point2f tl, Point2f br)
{
    Point2f center((tl.x + br.x) / 2, (tl.y + br.y) / 2);
    return center;
}

// ôóíêöèÿ íàõîæäåíèÿ èíäåêñà ñàìîãî ïåðåäíåãî ëèöà
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

// ôóíêöèÿ îïðåäåëåíèÿ êîîðäèíàò íàêëîíåííîãî ëèöà
void determ_true_face_coord(vector<Rect>& faces, const int front_face_index, int angle, const int video_width, int const video_height) {
    // íàõîæäåíèå êîîðäèíàò öåíòðà ëèöà (èç êîîðäèíàò âåðõíåãî ëåâîãî óãëà ëèöà) â ñ/ê ñ öåíòðîì â ñåðåäèíå èçîáðàæåíèÿ
    int face_X = faces[front_face_index].x + faces[front_face_index].width / 2 - video_width / 2;
    int face_Y = -faces[front_face_index].y - faces[front_face_index].height / 2 + video_height / 2;

    // íàõîæäåíèå êîîðäèíàò öåíòðà ëèöà â ñ/ê, ïîâåðíóòîé îáðàòíî
    angle = -angle;
    int old_x = face_X * cos(angle) - face_Y * sin(angle);
    int old_y = face_Y * cos(angle) + face_X * sin(angle);
    if (abs(angle) == 35) {
        old_x = -old_x;
        old_y = -old_y;
    }

    // îïðåäåëåíèå ëåâîãî âåðõíåãî óãëà ëèöà (èç ýòîãî ïàðàìåòðà ìû íàõîäèëè öåíòð ëèöà)
    faces[front_face_index].x = old_x - faces[front_face_index].width / 2 + video_width / 2;
    faces[front_face_index].y = -old_y - faces[front_face_index].height / 2 + video_height / 2;
}

int main()
{
    /* robot::protocol::ModbusMaster mb("COM3", 115200);
     mb.Setup();
     std::this_thread::sleep_for(std::chrono::milliseconds(1000));*/

    VideoCapture video(0);
    CascadeClassifier facedetect;
    Mat img;
    facedetect.load("res/haarcascades/haarcascade_frontalface_default.xml");

    const int video_width = video.get(cv::CAP_PROP_FRAME_WIDTH); // øèðèíà âèäåîïîòîêà
    const int video_height = video.get(cv::CAP_PROP_FRAME_HEIGHT); // âûñîòà âèäåîïîòîêà

    int k = 126; // êîíñòàíòà, ñâÿçûâàþùàÿ ðàññòîÿíèå äî ëèöà è ñòîðîíó êâàäðàòà: c = k/a, ãäå c - ðàññòîÿíèå, a - ñòîðîíà êâàäðàòà
    int face_x = 0, face_y = 0; // àáñîëþòíûé óãîë ïîâîðîòà êàìåðû
    double leg_x, leg_y, dist, side;
    int angle_x = 0, angle_y = 0;

    while (true) {
        video >> img; // ïîëó÷åíèå òåêóùåãî èçîáðàæåíèÿ

        Point center((img.cols - 1) / 2.0, (img.rows - 1) / 2.0); // öåíòð èçîáðàæåíèÿ
       
        vector<Rect> faces; // ñîçäàíèå ìàññèâà ñ ëèöàìè

        int angle = 0; // óãîë íàêëîíà ëèöà

        // ïîâîðà÷èâàåì íà 35 ãðàäóñîâ âëåâî/âïðàâî ïîêà íå íàéäåì ëèöî èëè äîéäåì äî îñòàíîâû è ñîõðàíÿåì ïàðàìåòðû ëèöà ïîä íàéäåííûì óãëîì
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

            int front_face_index = get_front_face_index(faces); // èíäåêñ ïåðåäíåãî ëèöà

            // îïðåäåëåíèå êîîðäèíàò ëèöà èç ïåðåâåðíóòîãî èçîáðàæåíèÿ
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


            rectangle(img, faces[front_face_index].tl(), faces[front_face_index].br(), Scalar(0, 255, 0), 5); // ïîñòðîåíèå ðàìêè ëèöà
            putText(img, to_string(angle_x) + " " + to_string(angle_y), Point(20, 40), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 0), 1); // âûâîä óãëîâ äî ëèöà
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


        //âûâîäèì èçîáðàæåíèå
        imshow("Eye", img);
        waitKey(1);
    }
}

