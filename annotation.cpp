#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <fstream>
#include <string>
#include <thread>
#include "argparse.hpp"

using namespace cv;
using namespace std;

vector<string> labels;
Rect tRect, fRect;

struct Annotation {
    Rect rect;
    int label;
};


vector<Annotation> rects;

Mat frame, text;

bool inProgress = false;
bool doneProgress = false;

int last_slider_val = 0;
int alpha_slider = 0;

int GLOBALINDEX ;

Point pointer = Point(0,0);
Rect tRec = Rect(0,0,0,0);

void readLabels(string lab) // Read Label file and plot list in zenity
{
    ifstream myReadFile;
    myReadFile.open(lab.c_str());
    string output;
    
    string labUI = "zenity --list  --height 500 --title=\"Labels\" --column=\"Index\" --column=\"Label\" ";
    
    if (myReadFile.is_open()) 
    {
        cout<<"Labels are:\n";
        while (!myReadFile.eof()) 
        {
            getline(myReadFile, output);
            cout<<output<<" ";
            if(output.length()>0)
            {   
                labels.push_back(output);
                labUI = labUI+"\""+to_string(labels.size()-1)+"\" "+"\""+output+"\" ";
                cout<<labels.size()-1<<endl;
            }
        }
    }
    system(labUI.c_str());
    myReadFile.close();

}


void onMouse(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        if(!inProgress)
        {
            inProgress = true;
            tRect.x = x;
            tRect.y = y;
        }
    }

    if  ( event == EVENT_LBUTTONUP )
    {
        
        if(inProgress)
        {
            inProgress = false;
            tRect.width = x - tRect.x;
            tRect.height = y - tRect.y;
            
            doneProgress = true;
        }
    }
    
    if(inProgress && !doneProgress)
    {
        Mat tFrame = frame.clone();
        rectangle(tFrame, Point(tRect.x, tRect.y), Point(x,y), Scalar(255,0,255), 1);       
        tRec = Rect(tRect.x, tRect.y, x-tRect.x, y-tRect.y);
        imshow("frame", tFrame);
    }
    
    if(!inProgress && doneProgress && tRect.area() > 10)
    {
        doneProgress = false;
        rects.push_back({tRect, alpha_slider});
    }

    pointer = Point(x,y);
}


int GETLASTINDEX(string folder)
{
	ostringstream ss;
	ss<<folder.c_str()<<"/index.txt";
	string ssc = ss.str();
	ifstream ifile(ssc.c_str());

	int val;
	ifile >> val;
	ifile.close();
	return val;
}

int UPDATELASTINDEX(string folder, int val)
{
	ostringstream ss;
	ss<<folder.c_str()<<"/index.txt";
	string ssc = ss.str();
	ofstream ofile(ssc.c_str(), ios_base::out | ios_base::trunc);

	ofile << val;
	ofile.close();
	return 0;
}

int  main(int argc, const char** argv)
{
    // make a new ArgumentParser
    ArgumentParser parser;
    // add some arguments to search for
    parser.addArgument("-v", "--video", 1);
    parser.addArgument("-l", "--labels", 1);
    parser.addArgument("-o", "--output", 1);
    parser.parse(argc, argv);
    
    string vid = parser.retrieve<string>("video"); 
    string lab = parser.retrieve<string>("labels");
    string out = parser.retrieve<string>("output");
    
    GLOBALINDEX = GETLASTINDEX(out);
    
    /* Read label from the file */
    std::thread lableThread(readLabels, lab);
    lableThread.detach();

    // Open Video for processing
    VideoCapture cap(vid.c_str());
    
    Mat framecopy;
    namedWindow("frame", 0);
    setMouseCallback( "frame", onMouse, 0 );
    
    cap >> frame;
    
    while(cap.isOpened())
    {
        cap >> frame;
        framecopy = frame.clone();
        
        createTrackbar( "Objects", "frame", &alpha_slider, labels.size()-1 );
        
    keyPress:
        
        text = Mat::zeros(50, 350, CV_8UC3);
        putText(text, labels[alpha_slider], Point(5,30),  FONT_HERSHEY_PLAIN, 2.0, Scalar(0,0,255), 3);
        
        addWeighted(text, 0.6, frame(Rect(0,0,text.cols, text.rows)), 0.4, 1.0, frame(Rect(0,0,text.cols, text.rows)));
        
        if(!inProgress)
        {   
            for(int i = 0; i<rects.size(); i++)
            {
                rectangle(frame, rects[i].rect, Scalar(255,0,0), 1);
            }        
            imshow("frame", frame);
        }

	    Mat tFrame = frame.clone();
	    line(tFrame, Point(pointer.x, 0), Point(pointer.x,frame.rows), Scalar(0,255,255), 1);
	    line(tFrame, Point(0, pointer.y), Point(frame.cols, pointer.y), Scalar(0,255,255), 1);
	    rectangle(tFrame, tRec, Scalar(255,0,255), 1);
	    imshow("frame", tFrame);    
        
        char c = waitKey(10);
        
        last_slider_val = alpha_slider;
        
        
        if(c == 27) // esc key
        {
            imshow("frame", frame);
            cout<<"Key Pressed!"<<endl;
            break;
        }
        else if(c == 'd') // 'd' key - move to next frame
        {
            imshow("frame", frame);
            continue;
        }
        else if(c == 'w') // 'w' key - skip one second
        {
            imshow("frame", frame);
        
            cout<<"A key pressed!"<<endl;
            for(int i=0; i<30; i++)
            {
                cap >> frame;
            }
        }
        else if(c == 's')
        {
            ostringstream si, st;
            si<<out.c_str()<<"/"<<GLOBALINDEX<<".jpg";
            st<<out.c_str()<<"/"<<GLOBALINDEX<<".txt";

            imwrite(si.str(), framecopy);

            string ssc = st.str();
            ofstream ofile(ssc.c_str(), ios_base::out | ios_base::trunc);

            for(int i = 0; i<rects.size(); i++)
            {
                float xr = 0.0, yr = 0.0, wr= 0.0, hr = 0.0;

                xr = (rects[i].rect.x+rects[i].rect.width/2)*1.0/frame.cols;
                yr = (rects[i].rect.y+rects[i].rect.height/2)*1.0/frame.rows;
                wr = (rects[i].rect.width)*1.0/frame.cols;
                hr = (rects[i].rect.height)*1.0/frame.rows;

                ofile << rects[i].label <<" "<<xr<< " "<< yr<<" "<<wr<<" "<<hr<<endl;
            }

            ofile.close();
            rects.clear();

            UPDATELASTINDEX(out,++GLOBALINDEX);
            cout<<"Images Processed : "<<GLOBALINDEX<<endl;
            
        }
        else if(c == 'x')
        {
            cap >> frame;
            rects.clear();
            imshow("frame", frame);
        }
        else
        {
            goto keyPress;
        }
    }
    
    return 0;
}

