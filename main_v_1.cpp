#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>  
#include <vector>  
#include <sys/stat.h> 　
#include <sys/types.h> 


#define WINDOW_NAME "PICTURE"        //为窗口标题定义的宏 

#define RED 1
#define GREEN 2
#define BLUE 3
#define ERASER 999

using namespace std;
using namespace cv;




Point point;
bool g_bDrawingBox = false;//是否进行绘制
bool g_button = false;
bool next = true;

int CurColor;
int colValue = 0;
int curKey; 
int colorR = 0;
int imgNum = 0;

Mat resImage;
Mat srcImage;
Mat tempImage;

void on_MouseHandle(int event, int x, int y, int flags, void* param);
void DrawPoint( cv::Mat& img, cv::Point point,int color );
void SetGreyValue(cv::Mat& img, cv::Point point,int color );
vector<Mat> read_images_in_folder(cv::String pattern);

int main()
{
	bool curSaveFlag = false;
   //遍历文件夹搜寻图片
	cv::String pattern = "/home/lwh/Project/MarkChip/build/chipimg/*.bmp";
	cv::String savedirstr = "/home/lwh/Project/MarkChip/build/chipimg_label/";
		

	if(0 == mkdir( savedirstr.c_str(),S_IRWXU))
	{
		cout <<"Create dir successfully!" <<endl;
	}
	else
	{
		cout <<"Can't create dir!" <<endl;
	}
	

	/*
	函数名：glob
	参数：String pattern 字符串，由文件夹路径和所要读取的文件名的格式构成的一个正则模板
	参数： std::vector<String>& result 字符串容器，存放所有符合模板的路径
	参数：bool recursive = false 标志位，是否使用递归的方式（没发现使用true和false的结果有什么区别）
	功能描述：遍历指定的文件夹，读取符合搜索模板的所有文件路径
	void glob(String pattern, std::vector<String>& result, bool recursive = false);*/
	
	cv::String curImgName;
	vector<cv::String> fn;
  	glob(pattern, fn, false);
  	size_t count = fn.size(); //number of png files in images folder
	
	
    //设置鼠标操作回调函数
    
			
   while(1)
   {
		
		if(next)
		{
			srcImage = imread(fn[imgNum]);
			srcImage.copyTo(tempImage);
    		resImage = Mat::zeros(srcImage.size(),CV_8UC1);
			size_t bmppos = fn[imgNum].find("chipimg");
			curImgName = fn[imgNum].substr(bmppos + 8,1000);
			cout << curImgName <<endl;	
			namedWindow( WINDOW_NAME );
			imshow(WINDOW_NAME,tempImage);
		    setMouseCallback(WINDOW_NAME,on_MouseHandle,(void*)&tempImage);
			next = false;
		}
		curKey  = waitKey(10);
		switch(curKey)
		{
			case 122://z
			case 90://Z
			{
				CurColor = RED;
				colValue = 1;
				g_button = true;
			}
			break;
			case 120://x
			case 88://X
			{
				CurColor = GREEN;
				colValue = 2;
				g_button = true;
			}
			break;
			case 99://c
			case 67://C
			{
				CurColor = BLUE;
				colValue = 3;
				g_button = true;
			}
			break;
			case 118://v
			case 86://V
			{
				CurColor = 3;
				colValue = 0;
				g_button = true;
			}
			break;
			case 81:
			case 113:
			{
				CurColor = ERASER;
				g_button = true;
			}
			break;
		//调整画笔大小
			case 48:
			case 49:
			case 50:
			case 51:
			case 52:
			case 53:			
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			{
				colorR = curKey-48;
				cout << "The current pencil radius:"<< colorR<< endl;			
			}
			break;
			//S键，save	
			
			case 115:
			{
				cv::String filename = savedirstr + curImgName;
				try
				{
					imwrite(filename,resImage);
				}catch(runtime_error& ex)
				{
					cout<< "Can't save picture "<< imgNum << ex.what() << endl;
			
				}	
				curSaveFlag = true;
				cout<< "Save picture "<< imgNum << ":"<< filename <<" successfully!" << endl;
					
			}
			break;
			//->和\|/键，下一张	
			//case 83:
			case 100:
			{
				if(!curSaveFlag)
				{
					cout << "WARNNING! The current image hasn't been saved!"<< endl;
				}else
				{
					imgNum++;
					cout << "The current image number is :" << imgNum << endl; 
					curSaveFlag = false;
					next = true;
					resImage.release();
					srcImage.release();
					tempImage.release();
					destroyWindow(WINDOW_NAME)	;
				}	
			}
			break;	
			default:
			{
				CurColor = 0;
				g_button = false;
				break;
			}
		}  
		
        if( curKey == 27 ) break;//按下ESC键，程序退出
    }
    return 0;
}

void on_MouseHandle(int event, int x, int y, int flags, void* param)
{
    Mat& image = *(cv::Mat*) param; 
    switch( event)
    {
        //鼠标移动消息
        case EVENT_MOUSEMOVE: 
        {      
   	       	if( g_button )
            {
				 g_bDrawingBox = true;				 
				 point.x = x;
				 point.y = y;
				 DrawPoint(image, point,CurColor);
				 SetGreyValue(resImage, point,CurColor );
				 //resImage.at<uchar>(y,x) = 250;
            }
        }
        break;
        //左键按下消息
        case EVENT_LBUTTONDOWN: 
        {
	    	if(g_button)
			{
				g_bDrawingBox = true;
				SetGreyValue(resImage, point,CurColor );
			}
		}
        break;
        //左键抬起消息
        case EVENT_LBUTTONUP: 
        {   
        	g_bDrawingBox = false;
			g_button = false;
        }
        break;
    }
}
void SetGreyValue(cv::Mat& img, cv::Point point,int color )
{
switch(color)
	{
		case RED:
		{
			img.at<uchar>(point) = 250;
		}
		break;
		case GREEN:
		{
			img.at<uchar>(point) = 128;
		}
		break;
		case BLUE:
		{
			img.at<uchar>(point) = 64;
		}
		break;
		//清除某一点
		case ERASER:
		{
			cv::circle(img,point,0,0,-1);	
		}
		break;
		default:
			cv::circle(img,point,colorR,cv::Scalar(0,0,255),-1);
		break;
	}
}
void DrawPoint( cv::Mat& img, cv::Point point,int color )
//cvScaler::BGR
{
    switch(color)
	{
		case RED:
		{
			cv::circle(img,point,colorR,cv::Scalar(0,0,255),-1);
		}
		break;
		case GREEN:
		{
			cv::circle(img,point,colorR,cv::Scalar(0,255,0),-1);
		}
		break;
		case BLUE:
		{
			cv::circle(img,point,colorR,cv::Scalar(255,0,0),-1);
		}
		break;
		//清除某一点
		case ERASER:
		{
			cv::circle(img,point,0,srcImage.at<Vec3b>(point),-1);
			for(int i =-colorR;i<colorR;i++)
			{
				for(int j =-colorR;j<colorR;j++)
				{
					Point tempoint= Point(point.x+i,point.y+j);
					cv::circle(img,tempoint,0,srcImage.at<Vec3b>(tempoint),-1);
				}
			
			}			
		}
		break;
		default:
			cv::circle(img,point,colorR,cv::Scalar(0,0,255),-1);
		break;
	}
	//图像融合显示
	//IplImage IMG;
//	addWeighted(IplImage(srcImage),0.5,IplImage(img),0.5,IMG);
	Mat IMG;
	addWeighted(srcImage,0.8,img,0.2,0,IMG);
    imshow( WINDOW_NAME, IMG );
}

