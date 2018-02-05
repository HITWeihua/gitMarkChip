#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>  
#include <vector>  
#include <map>
#include <sys/stat.h> 　
#include <sys/types.h> 
#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <exception> 
#include<unistd.h>


#define RED 1
#define GREEN 2
#define BLUE 3
#define YELLOW 4
#define ERASER 999

#define WINDOW_NAME "Label images"

using namespace std;
using namespace cv;



 
Point point;
bool g_bDrawingBox = false;//是否进行绘制
bool g_button = false;
bool next = true;
bool curSaveFlag = false;

int CurColor;
int colValue = 0;
int curKey; 
int colorR = 0;
int ImgtotalNum = 2147483647;
int curImgNum = -1; //默认从第一图片开始;

Mat resImage;
Mat srcImage;
Mat tempImage;

void on_MouseHandle(int event, int x, int y, int flags, void* param);
void DrawPoint( cv::Mat& img, cv::Point point,int color );
void SetGreyValue(cv::Mat& img, cv::Point point,int color);



cv::String Option;
cv::String curImgName; //默认Next
cv::String Imagefolder;
cv::String curImgstr;
cv::String savedirstr;

vector<cv::String> ImgNames;
vector<cv::String> ImgFloders;
map<cv::String,int> ImgMap;


typedef int (*CmdProcFunc)();
typedef struct{
     char         *pszCmd;
     CmdProcFunc  fpCmd;
}CMD_PROC;

#define CMD_ENTRY(cmdStr, func)     {cmdStr, func}
#define CMD_ENTRY_END               {NULL,   NULL}

int Start()
{	
	curImgNum = ImgMap[Option];

	cout << "**Current Image Number is:" << curImgNum << endl;
	curImgName = ImgNames.at(curImgNum);
	cout << "**Start process Image:" << curImgName << endl;
	curImgstr = Imagefolder+ curImgName;
	srcImage = imread(curImgstr);
	srcImage.copyTo(tempImage);
   	resImage = Mat::zeros(srcImage.size(),CV_8UC1);
	
	namedWindow(WINDOW_NAME);
	
	imshow(WINDOW_NAME,tempImage);
	
	setMouseCallback(WINDOW_NAME,on_MouseHandle,(void*)&tempImage);
	next = false;
	while(1)
        {
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
				CurColor = YELLOW;
				colValue = 4;
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
				cout << "**The current pencil radius:"<< colorR<< endl;			
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
					cout<< "<WARNNING>Can't save picture "<< curImgNum << ex.what() << endl;
			
				}	
				curSaveFlag = true;
				cout<< "**Save picture "<< curImgNum << ":"<< curImgName <<" successfully!" << endl;		
			}
			break;
			case 27:
			{
				if(!curSaveFlag)
				{
					cout << "<WARNNING!>The current image hasn't been saved!"<< endl;
					break;
				}else
				{
					resImage.release();
					srcImage.release();
					tempImage.release();
					destroyWindow(WINDOW_NAME);

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
        if( curKey == 27 && curSaveFlag ) 
		{	
			curSaveFlag = false;				
			break;//按下ESC键，程序退出
		}
    }	
	return 0;
}

int  Next()
{
	curImgNum = curImgNum+1;
	if(curImgNum > ImgtotalNum - 1)
	{
		cout<< "<WARNNING!>There isn't more images! "<<endl;
		return 0;			
	}
	Option = ImgNames.at(curImgNum);
	Start();
	return 0;
}
//命令表
static CMD_PROC gCmdMap[] = {
     CMD_ENTRY("start", Start),
     CMD_ENTRY("next", Next),
     CMD_ENTRY_END
 };
#define CMD_MAP_NUM     (sizeof(gCmdMap)/sizeof(CMD_PROC)) - 1/*End*/

//返回gCmdMap中的CmdStr列(必须为只读字符串)，以供CmdGenerator使用
static char *GetCmdByIndex(unsigned int dwCmdIndex)
{
     if(dwCmdIndex >=  CMD_MAP_NUM)
		return NULL;
     return gCmdMap[dwCmdIndex].pszCmd;
}

static int ExecCmd(char *pszCmdLine)
{
    if(NULL == pszCmdLine)
        return -1;
	if(0 == strcmp(pszCmdLine, gCmdMap[1].pszCmd))
	{
		Next();
		return 0;
	} 
	else if(0 == strcmp(pszCmdLine, gCmdMap[0].pszCmd))
	{
			Next();
			return 0;
	}
	else
	{
	   string pszLineHead = pszCmdLine; 
  	   vector<string> arr;
	   istringstream ss(pszLineHead);
	   string word;
 	   while(ss >> word)
	   {
    		arr.push_back(word);
 	   }
	   if(arr.size() == 1)
		{
			cout<<"<WARNNING!>Invalid commad!" << endl;
			return 0;
		}
		else
		{ 
		    try
		    {
	    	    pszCmdLine = (char*)arr[0].data();
	  	 	    Option = arr[1];
		    }catch(exception& e)
		    {
				cout <<"<WARNNING!>Start must have a file name!" << endl;
				return 0;
		    }				
			if(arr[0] == "start")
			{
				string temp = Imagefolder+ Option;
				const char* temp1 = temp.c_str();
				if(0 == access(temp1,4))
				{
					Start();
					return 0;
				}
				else
				{	
					cout<<"<WARNNING!>The image doesn't exist!"<<endl;
					return 0;
				}
			}
			else
			{
				cout << "<WARNNING!>Invalid commad!!"<<endl;
			}
		}
	} 
    return 0;
 }



//退出交互式调测器的命令(不区分大小写)
static const char *pszQuitCmd[] = {"Quit", "Exit", "End", "Bye", "Q", "E", "B"};
static const unsigned char ucQuitCmdNum = sizeof(pszQuitCmd) / sizeof(pszQuitCmd[0]);
static int IsUserQuitCmd(char *pszCmd)
{
    unsigned char ucQuitCmdIdx = 0;
    for(; ucQuitCmdIdx < ucQuitCmdNum; ucQuitCmdIdx++)
    {
        if(!strcasecmp(pszCmd, pszQuitCmd[ucQuitCmdIdx]))
            return 1;
    }

    return 0;
}

//剔除字符串首尾的空白字符(含空格)
static char *StripWhite(char *pszOrig)
{
    if(NULL == pszOrig)
        return "NUL";

    char *pszStripHead = pszOrig;
    while(isspace(*pszStripHead))
        pszStripHead++;

    if('\0' == *pszStripHead)
        return pszStripHead;

    char *pszStripTail = pszStripHead + strlen(pszStripHead) - 1;
    while(pszStripTail > pszStripHead && isspace(*pszStripTail))
        pszStripTail--;
    *(++pszStripTail) = '\0';

    return pszStripHead;
}

static char *pszLineRead = NULL;  //终端输入字符串
static char *pszStripLine = NULL; //剔除前端空格的输入字符串

char *ReadCmdLine()
{
     //若已分配命令行缓冲区，则将其释放
    if(pszLineRead)
    {
        free(pszLineRead);
        pszLineRead = NULL;
    }
    //读取用户输入的命令行
    pszLineRead = readline(">>Please input 'start XXX' or 'next' to label image:");

    //剔除命令行首尾的空白字符。若剔除后的命令不为空，则存入历史列表
    pszStripLine = StripWhite(pszLineRead);	
    if(pszStripLine && *pszStripLine)
        add_history(pszStripLine);
     	

	
    return pszStripLine;
}

static char *CmdGenerator(const char *pszText, int dwState)
{
    static int dwListIdx = 0, dwTextLen = 0;
    if(!dwState)
    {
        dwListIdx = 0;
        dwTextLen = strlen(pszText);
    }

    //当输入字符串与命令列表中某命令部分匹配时，返回该命令字符串
    const char *pszName = NULL;
    while((pszName = GetCmdByIndex(dwListIdx)))
    {
        dwListIdx++;

        if(!strncmp (pszName, pszText, dwTextLen))
            return strdup(pszName);
    }

    return NULL;
}

static char **CmdCompletion (const char *pszText, int dwStart, int dwEnd)
{
    //rl_attempted_completion_over = 1;
    char **pMatches = NULL;
    if(0 == dwStart)
        pMatches = rl_completion_matches(pszText, CmdGenerator);

    return pMatches;
}

//
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
//绘制label图
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
		case YELLOW:
		{
			cv::circle(img,point,colorR,cv::Scalar(0,255,255),-1);
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
	Mat IMG;
	addWeighted(srcImage,0.8,img,0.2,0,IMG);
    imshow(WINDOW_NAME, IMG );
}




int main(void)
{ 
	bool successInput = false;
	vector<cv::String>::iterator inputfind;
	vector<cv::String> ImgFormat;
	ImgFormat.push_back(".bmp");
	ImgFormat.push_back(".jpg");
	ImgFormat.push_back(".jpeg");	
	ImgFormat.push_back(".png");
	ImgFormat.push_back(".tiff");
	ImgFormat.push_back(".jp2");
	ImgFormat.push_back(".tif");
	ImgFormat.push_back(".pbm");	
	
	rl_attempted_completion_function = CmdCompletion;
    printf("		>> Welcome to Label Images!<<		\n");
    cv::String imagepattern;
	char *input; 
	char *inputfolder;
	do
	{
		input = readline(">>Please input your image folder(END by /):");	
		inputfolder = input;
	    Imagefolder = input;
		int res;
		//res==0 代表有读取权限
		res = access(input,4);
		if(res == 0)
		{
			successInput = true;	
		}
		else
		{
			cout <<"<WARNNING!>  Your input folder doesn't exist or the folder can't be read! " << endl;
		}
	}while(!successInput);
	successInput = false;

    int lenth = Imagefolder.size();
	cv::String imgformat;
	do
	{
		input = readline(">>Please input your image format(Satrt by .):");

    	imgformat = input;
		inputfind = find(ImgFormat.begin(),ImgFormat.end(),imgformat);
		if(inputfind != ImgFormat.end()){
				successInput = true;	
		}
		else
		{
			cout <<"<WARNNING!>  The image format can't be understand!" <<endl;
		}
	}while(!successInput);
	successInput = false;
	
	imagepattern = Imagefolder +"*"+ imgformat;
  	glob(imagepattern, ImgFloders, false);
  	size_t count = ImgFloders.size(); 
    ImgtotalNum	 = int(count);
        cout<<"Find "<< count << " images" << endl;
	//对字符串进行排序
	sort(ImgFloders.begin(),ImgFloders.end());
	int i = 0;
	vector<cv::String>::iterator iter;
	for(iter = ImgFloders.begin() ;iter != ImgFloders.end();++iter)
	{
		cv::String curName = iter->substr(lenth,100000);		
		ImgNames.push_back(curName);
		ImgMap[curName] = i;
		i = i + 1;
	}	
	do
	{
		input = readline(">>Please input your target image saved folder(END by /,if it doesn't exist,it will be created.):");
		savedirstr = input;
		int res;
		//创建目标目录
		res = access(input,2);
		if(res == 0)
		{
			if(0 == strcmp(input,inputfolder))
			{
				cout << "<WARNNING!> The target folder can't be same with the input folder!"<<endl;
			}
			else
			{
				cout <<"**The target image has existed！" <<endl;
				successInput = true;
			}
		}
		else
		{
			if(0 == mkdir( savedirstr.c_str(),S_IRWXU))
			{
				cout <<"**Create target image saved folder successfully!" <<endl;
				successInput = true;
			}
			else
			{
				cout <<"<WARNNING!> Can't create target image saved folder！" <<endl;
			}
		}
	}while(!successInput);	
	successInput = false;;	
    while(1)
    {
        char *pszCmdLine = ReadCmdLine();
        if(IsUserQuitCmd(pszCmdLine))
        {
            free(pszLineRead);
            break;
        }
        ExecCmd(pszCmdLine);
    }

    return 0;
}
// /home/lwh/Project/MarkChip/build/chipimglabel/
// /home/lwh/Project/MarkChip/build/chipimg/
// start Fly_PartIMG_5_5.bmp



