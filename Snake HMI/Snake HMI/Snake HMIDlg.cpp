
// Snake HMIDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "Snake HMI.h"
#include "Snake HMIDlg.h"
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "MultiArray.h"				// 多維陣列工具類別

using namespace std;


LRESULT CALLBACK capVideoStreamCallback(HWND hWnd,LPVIDEOHDR lpVHdr);
LPVIDEOHDR lpVHdrTemp;
//--*
#define USE_COMMON

#if defined(WIN32)
#include <roboard_dll.h>  // use the DLL version of RoBoIO lib
//#include <roboard.h>   // use the static version of RoBoIO lib
#else
#include <roboard.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//*--

HWND hMyWnd, hWndC;
LPBITMAPINFO lpbi;   // for video format
//--*


//Image
#define IMAGE_HEIGHT 240
#define IMAGE_WIDTH 320

//other
#define M_PI 3.14159265358979323846
#define UP_LIMIT 10
#define LEFT_LIMIT 10
#define PRETIME 10
//Senser
const unsigned char acceler_addr = 0x30>>1;
const unsigned char magnetic_addr = 0x1e;
const unsigned char gyro_addr = 0xD0>>1;
//motor
unsigned char regWrite[11]={0};
unsigned char syncWrite[(MOTOR_NUM*5)+8]={0};
unsigned char position[MOTOR_NUM*2]={0};
unsigned char speed[MOTOR_NUM*2]={0};
unsigned char oldPosition[MOTOR_NUM*2]={0};
unsigned char nextPosition[MOTOR_NUM*2]={0};
int motionPageF=0;
int motionPageL=0;
int motionPageR=0;
bool stateChangeFlag=1;
unsigned char sendData[20]={0xFF,0xFF,0x01,0x07,0x03,0x1E,0x00,0x02,0x00,0x02,0x00};
unsigned char pos[2]={0x00,0x02};//low, hight
unsigned char pos2[2]={0x00,0x02};
unsigned char read[6]={0x00,0x00,0x00,0x00,0x00,0x00};
//other
double doubleTemp=0;
bool imgProcessFlag;
int gdwFrameNum=0;
char *gachBuffer=new char[100];
int state=0;//0=initial;1=F;2=L;3=R;
CString strTemp;
ImageProcessFunction g_imageTool(IMAGE_HEIGHT, IMAGE_WIDTH);// 影像工具類別

frames motionInitial[MOTIOM_MAX];
frames motionF[MOTIOM_MAX];
frames motionR[MOTIOM_MAX];
frames motionL[MOTIOM_MAX];

double g_pOldHSLImage[3][IMAGE_WIDTH][IMAGE_HEIGHT]={0};



// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 對話方塊資料
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSnakeHMIDlg 對話方塊




CSnakeHMIDlg::CSnakeHMIDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSnakeHMIDlg::IDD, pParent)
, m_strAzimuth(_T("Azimuth"))
, m_InfoText(_T("InfoText"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSnakeHMIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_AZIMUTH, m_strAzimuth);
	DDX_Text(pDX, IDC_STATIC_AZIMUTH3, m_InfoText);
}

BEGIN_MESSAGE_MAP(CSnakeHMIDlg, CDialog)
	ON_WM_TIMER()// 計時器
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CSnakeHMIDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SetCAM, &CSnakeHMIDlg::OnBnClickedButtonSetcam)
	ON_BN_CLICKED(IDCANCEL, &CSnakeHMIDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON7, &CSnakeHMIDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON1, &CSnakeHMIDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CSnakeHMIDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CSnakeHMIDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CSnakeHMIDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CSnakeHMIDlg 訊息處理常式

BOOL CSnakeHMIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	ImageWidth=0;
	ImageHeight=0;
	Img = new CImage;
	HslImg = new CImage;
	RGBImg = new CImage;



	ImgFlag=false;
	imgProcessFlag=false;
	AllocConsole();
	Set();

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CSnakeHMIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CSnakeHMIDlg::OnPaint()
{
	CClientDC thisDC(this);
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
	/*if(ImgFlag)// 重繪圓影像
	{
		Img->Draw(thisDC, 1*(ImageWidth+10)+LEFT_LIMIT, 0*(ImageHeight+10)+UP_LIMIT);
		//CopyImg->Draw(thisDC, 1*(ImageWidth+10)+10, 0*(ImageHeight+10)+10);
		HslImg->Draw(thisDC, 2*(ImageWidth+10)+LEFT_LIMIT, 0*(ImageHeight+10)+UP_LIMIT);
		RGBImg->Draw(thisDC, 3*(ImageWidth+10)+LEFT_LIMIT, 0*(ImageHeight+10)+UP_LIMIT);
		//ProcessImg->Draw(thisDC, 1*(ImageWidth+10)+10, 0*(ImageHeight+10)+10);
	}*/
}

void CSnakeHMIDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateData();
	switch(nIDEvent)
	{
	case 1:
		{
			RunSensor();
			switch(state)
			{
			case 0:
				Action(1,motionInitial);
				break;
			case 1:
				Action(motionPageF,motionF);
				break;
			case 2:
				Action(motionPageL,motionL);
				break;
			case 3:
				Action(motionPageR,motionR);
				break;
			}
			if(imgProcessFlag==true)
					ImageProcess();
			break;
		}
		

	}
	CDialog::OnTimer(nIDEvent);
}





// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CSnakeHMIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSnakeHMIDlg::Set()
{
	UpdateData();
	//roboard initial
	roboio_SetRBVer(RB_110); // use RB-110 , if use RB-100, replace roboio_SetRBVer(RB_100);
	if (i2c_Initialize(I2CIRQ_DISABLE) == false)
	{
		m_strAzimuth.Format(_T("I2C initial false!!  %s\n"),roboio_GetErrMsg());
		EndProgram();
	}
	i2c0_SetSpeed(I2CMODE_AUTO, 400000L); // set i2c speed:400k
	// set acceler
	if(i2c0master_StartN(acceler_addr, I2C_WRITE,2) == false)
	{
		m_strAzimuth.Format(_T("g-sensor error:%s !!\n"),roboio_GetErrMsg());
		EndProgram();
	}
	i2c0master_WriteN(0x20);
	i2c0master_WriteN(0x37);

	if(i2c0master_StartN(magnetic_addr, I2C_WRITE,4) == false)
	{
		m_strAzimuth.Format(_T("mangnetic sensor error:%s !!\n"),roboio_GetErrMsg());
		EndProgram();
	}
	i2c0master_WriteN(0x00); //set CRA_REG_M
	i2c0master_WriteN(0x18); //DO2 = 1, DO1 = 1, DO0 = 0
	i2c0master_WriteN(0x01); //set CRB_REG_M
	i2c0master_WriteN(0x60); //GN0 = 1, GN1 = 1, GN2 = 0(-2.5g ~ +2.5g)
	if(i2c0master_StartN(gyro_addr, I2C_WRITE,2) == false)
	{
		m_strAzimuth.Format(_T("mangnetic sensor error:%s !!\n"),roboio_GetErrMsg());
		EndProgram();
	}
	i2c0master_WriteN(0x16); //set DLPF, full scale
	i2c0master_WriteN(0x18); // bit3 = bit4 = 1
	//	printf("RM-G146 sensor module start...\n\n");



	if (com3_Init(COM_HDUPLEX)==false)
	{
		m_InfoText.Format(_T("Init COM3 fail!!  %s\n"),roboio_GetErrMsg());
		EndProgram();
	}
	com3_SetFormat(COM_BYTESIZE8,COM_STOPBIT1,COM_NOPARITY);
	com3_SetBaud(COMBAUD_115200BPS);

	//---motor initial---
	//RegWrite->one motor
	//SyncWrite-> motor
	//unsigned char sendData[20]={0xFF,0xFF,0x01,0x07,0x03,0x1E,0x00,0x02,0x00,0x02,0x00};
	//RegWrite:Start,Start,ID,DataLenght,Instruction,Address,PositionLow,PositionHigh,SpeedLow,SpeedHigh,CheckSum
	/*unsigned char sendData[28]={0XFF,0XFF,0XFE,0X18,0X83,0X1E,0X04,
	0X00,0X10,0X00,0X50,0X01,
	0X01,0X20,0X02,0X60,0X03,
	0X02,0X30,0X00,0X70,0X01,
	0X03,0X20,0X02,0X80,0X03,
	0X12};*/
	/*SyncWrite:Start,Start,ID,DataLenght,Instruction,Address,MotorNum
	PositionLow,PositionHigh,SpeedLow,SpeedHigh,
	...,
	...,
	...,
	CheckSum*/

	//initial regWrite
	regWrite[0]=0xFF;
	regWrite[1]=0xFF;
	regWrite[3]=0x07;
	regWrite[4]=0x03;
	regWrite[5]=0x1E;
	//initial sycnWrite
	syncWrite[0]=0xFF;
	syncWrite[1]=0xFF;
	syncWrite[2]=0xFE;
	syncWrite[4]=0x83;
	syncWrite[5]=0x1E;
	syncWrite[5]=MOTOR_NUM;
	//initial position
	for(int i=0;i<MOTOR_NUM*2;i++)
	{
		if(i%2!=0)
			position[i]=0x02;
	}
	//initial speed
	for(int i=0;i<MOTOR_NUM*2;i++)
	{
		if(i%2!=0)
			speed[i]=0x02;
	}
	for(int i=0;i<MOTOR_NUM*2;i++)
	{
		if(i%2!=0)//i==0?
			oldPosition[i]=0x02;//hight byte
	}
	UpdateData(FALSE);
	for(int i=0;i<MOTOR_NUM;i++)
		motionInitial[0].motor[i]=511;
	motionPageF=ReadServoFile("M_Position.txt",motionF);
	motionPageL=ReadServoFile("L_Position.txt",motionL);
	motionPageR=ReadServoFile("R_Position.txt",motionR);
}

void CSnakeHMIDlg:: EndProgram()
{
	i2c_Close();
}




void CSnakeHMIDlg::ErrorMsg(unsigned char addr)
{
	UpdateData();
	if(addr==acceler_addr)
		m_strAzimuth.Format(_T("g-sensor error:%s !!\n"), roboio_GetErrMsg());
	else if(addr==magnetic_addr)
		m_strAzimuth.Format(_T("mangnetic sensor error:%s !!\n"), roboio_GetErrMsg());
	else if(addr==gyro_addr)
		m_strAzimuth.Format(_T("gyro sensor error:%s !!\n"), roboio_GetErrMsg());
	UpdateData(FALSE);
	/*
	if(addr==acceler_addr)
	printf("g-sensor error:%s !!\n",roboio_GetErrMsg());
	else if(addr==magnetic_addr)
	printf("mangnetic sensor error:%s !!\n",roboio_GetErrMsg());
	else if(addr==gyro_addr)
	printf("gyro sensor error:%s !!\n",roboio_GetErrMsg());
	*/
}
//-sensor-//
unsigned int CSnakeHMIDlg::ReadI2CCmd(unsigned char addr, unsigned char rwbit, int count)
{	
	unsigned int reg=0;

	if(i2c0master_StartN(addr, rwbit,count) == false)
	{
		ErrorMsg(addr);
		EndProgram();
	}
	if((reg = i2c0master_ReadN()) == 0xffff)
	{
		ErrorMsg(addr);
		EndProgram();
	}
	return reg;
}

void CSnakeHMIDlg::SendI2CCmd(unsigned char addr, unsigned char rwbit, int count,unsigned char cmd)
{
	if(i2c0master_StartN(addr, rwbit,count) == false)
	{
		ErrorMsg(addr);
		EndProgram();
	}
	if(i2c0master_WriteN(cmd) == false)
	{
		ErrorMsg(addr);
		EndProgram();
	}
}

unsigned int CSnakeHMIDlg::GetSenserVal(unsigned char senserAddr,unsigned char cmd)
{
	SendI2CCmd(senserAddr, I2C_WRITE,1,cmd);
	return ReadI2CCmd(senserAddr, I2C_READ,1);	
}
unsigned int CSnakeHMIDlg::GetSenserVal2Byte(unsigned char senserAddr,unsigned char highCmd,unsigned char lowCmd)
{
	unsigned int hByte=0,lByte=0;
	hByte=GetSenserVal(senserAddr,highCmd);
	hByte = hByte << 8;
	lByte=GetSenserVal(senserAddr,lowCmd);
	return hByte + lByte;

}

int CSnakeHMIDlg::UnsignedIntToInt(unsigned int val)
{
	if((val&0x8000)!=0)
		return val|0xffff0000;
	else
		return val;
}

double CSnakeHMIDlg::GetPitch(double *aVal)  
{   
	if (aVal[1] == 0 && aVal[2] == 0)
		return 0.0;                      
	return atan2(aVal[1],aVal[2]);
}

double	CSnakeHMIDlg::GetRoll(double *aVal)
{
	// q 為 G-sensor 的 Yg 軸與 Zg 軸的平方和
	double q;
	q = aVal[1]*aVal[1] + aVal[2]*aVal[2];
	if(aVal[2] > 0.0)
		return atan2(aVal[0],sqrt(q));
	else
		return atan2(aVal[0],-sqrt(q));

}


double CSnakeHMIDlg::GetAzimuth(double *mVal, double pitch, double roll)
{
	double xh, yh, azi;
	double cp, sp, cr, sr;

	cp = cos(pitch);
	sp = sin(pitch);
	cr = cos(roll);
	sr = sin(roll);
	// 做線性變換，x 即是 Xh，y即是 Yh
	xh = mVal[0]*cr - mVal[1]*sr*sp - mVal[2]*cp*sr;
	yh = mVal[1]*cp - mVal[2]*sp;

	if(pitch > M_PI/2 || pitch < -M_PI/2)  // 此時整個 sensor module 顛倒，x 跟 y 都要變號
		azi = atan2(xh,-yh);
	else
		azi = atan2(-xh,yh);    

	if (azi < 0)
	{
		azi = 2*M_PI + azi;    // M_PI = 3.1415967
	}
	return  azi;
}
void CSnakeHMIDlg::RunSensor()
{
	unsigned int acceler_X=0, acceler_Y=0, acceler_Z=0, magnetic_X=0, magnetic_Y=0, magnetic_Z=0, gyro_X=0, gyro_Y=0, gyro_Z=0, gyro_temp=0;
	double aVal[3]={0},mVal[3]={0},gVal[3]={0};
	double pitch=0,roll=0,azimuth=0;
	// read acceler (LSM303DLH.pdf p.27)
	acceler_X=GetSenserVal2Byte(acceler_addr,0x29,0x28);
	acceler_Y=GetSenserVal2Byte(acceler_addr,0x2b,0x2a);
	acceler_Z=GetSenserVal2Byte(acceler_addr,0x2d,0x2c);

	// read magnetic (LSM303DLH.pdf p.27 p.28)
	magnetic_X=GetSenserVal2Byte(magnetic_addr,0x03,0x04);
	magnetic_Y=GetSenserVal2Byte(magnetic_addr,0x05,0x06);
	magnetic_Z=GetSenserVal2Byte(magnetic_addr,0x07,0x08);

	// read gyro (PS-ITG-3200-00-01.4 P.22)
	gyro_temp=GetSenserVal2Byte(gyro_addr,0x1b,0x1c);
	gyro_X=GetSenserVal2Byte(gyro_addr,0x1d,0x1e);
	gyro_Y=GetSenserVal2Byte(gyro_addr,0x1f,0x20);
	gyro_Z=GetSenserVal2Byte(gyro_addr,0x21,0x22);

	aVal[0]=(double) UnsignedIntToInt(acceler_X);
	aVal[1]=(double) UnsignedIntToInt(acceler_Y);
	aVal[2]=(double) UnsignedIntToInt(acceler_Z);
	mVal[0]=(double) UnsignedIntToInt(magnetic_X);
	mVal[1]=(double) UnsignedIntToInt(magnetic_Y);
	mVal[2]=(double) UnsignedIntToInt(magnetic_Z);
	gVal[0]=(double) UnsignedIntToInt(gyro_X);
	gVal[1]=(double) UnsignedIntToInt(gyro_Y);
	gVal[2]=(double) UnsignedIntToInt(gyro_Z);

	CString stest;
	pitch=GetPitch(aVal);
	roll=GetRoll(aVal);
	azimuth=GetAzimuth(mVal,pitch,roll)*180.0/M_PI;
	//doubleTemp=azimuth;
	/*
	FILE*fp=freopen("CONOUT$","wt",stdout);								//這四行加到你要的地方
	freopen("CONIN$","rt",stdin); 
	std::cout <<"azimuth="<<azimuth<<std::endl;//顯示方位
	*/
	UpdateData();
	m_strAzimuth.Format(_T("%f"), azimuth);
	UpdateData(false);


}
//-Servo-//
void CSnakeHMIDlg::Action(int motionPage,frames *motion)
{
	//目前位置---指定動作起始位置---指定動作結束位置
	int count=0;
	for(int i=0;i<MOTOR_NUM;i++)//儲存起始位置
	{
		nextPosition[i*2]=motion[0].motor[i]%256;
		nextPosition[i*2+1]=motion[0].motor[i]/256;
	}
	for(int i=0;i<MOTOR_NUM;i++)//儲存最後位置
	{
		oldPosition[i*2]=motion[motionPage-1].motor[i]%256;
		oldPosition[i*2+1]=motion[motionPage-1].motor[i]/256;
	}
	for(int i=0;i<MOTOR_NUM;i++)//計數器，計算上次結束位置與此次起點位置是否相同
	{
		if(oldPosition[i*2]==nextPosition[i*2]&&oldPosition[i*2+1]==nextPosition[i*2+1])
		{
			count++;
		}
	}
	if(state!=99){
	if(count==MOTOR_NUM&&stateChangeFlag==1)//如果上次結束位置與此次起點位置不同，則用內插法先慢慢移到起點位置
	{
		PreMove(oldPosition,nextPosition);
		stateChangeFlag=0;
	}
	//執行指定動作

	Move(motionPage,motion);
	}
	//unsigned char sendData[20]={0xFF,0xFF,0x01,0x07,0x03,0x1E,0x00,0x02,0x00,0x02,0x00};
}



void CSnakeHMIDlg::PreMove(unsigned char *oldPosition,unsigned char *nextPosition)//內差法
{
	unsigned int position=0;
	unsigned int speed=1023;
	rcservo_EnterPlayMode();
	for(int i=1; i<=PRETIME;i++)
	{
		for(int j=0;j<MOTOR_NUM;j+=2)
		{
			position=(oldPosition[j]+oldPosition[j+1]*256)+(i*((nextPosition[j]+nextPosition[j+1]*256)-(oldPosition[j]+oldPosition[j+1]*256))/PRETIME);
			sendData[7]=position/256;
			sendData[6]=position%256;

			sendData[9]=speed/256;
			sendData[8]=speed%256;


			sendData[2]=j+1;//ID
			sendData[10]=~(sendData[2]+sendData[3]+sendData[4]+sendData[5]+sendData[6]+sendData[7]+sendData[8]+sendData[9]);

			if(com3_Send(sendData,11)==false)
			{
				UpdateData();
				m_InfoText = _T(" COM3 ErrMsg is %s\n", roboio_GetErrMsg());
				UpdateData(false);
			}	

		}
	}
}

void CSnakeHMIDlg::Move(int motionPageIn,frames *motion_r)
{
	int motionPageTemp=0;
	unsigned int position=0;
	unsigned int speed=1023;
	int motionPage=motionPageIn;
	rcservo_EnterPlayMode();
	if(motionPageIn==0)
	{
		UpdateData();
		m_InfoText = _T(" Error!No data can use! ");
		UpdateData(false);
	}
	else
	{

		do
		{	
			for(int i=0;i<MOTOR_NUM;i++)
			{
				position=motion_r[motionPageTemp].motor[i];
				sendData[7]=position/256;
				sendData[6]=position%256;

				sendData[9]=speed/256;
				sendData[8]=speed%256;


				sendData[2]=i+1;//ID
				sendData[10]=~(sendData[2]+sendData[3]+sendData[4]+sendData[5]+sendData[6]+sendData[7]+sendData[8]+sendData[9]);

				if(com3_Send(sendData,11)==false)
				{
					UpdateData();
					m_InfoText = _T(" COM3 ErrMsg is ");
					UpdateData(false);
				}
				if(imgProcessFlag==true)
					ImageProcess();
			}
			motionPageTemp++;
			motionPage--;
		}while(motionPage>0);
	}
}
int CSnakeHMIDlg::ReadServoFile(char *filename,frames *motion_r)
{
	int motionPage=0;
	
	FILE *fp;
	
	if((fp=fopen(filename,"r"))==NULL)
	{
		MessageBox(_T(" Open file error! "),_T(""),MB_ICONWARNING | MB_OK);
		UpdateData();
		m_InfoText = _T(" Open file error! ");
		UpdateData(false);
	}
	else
	{		
		UpdateData();
		m_InfoText = _T(" Open file! ");
		UpdateData(false);
		int motorValCount=0;
		int strCount=0;
		unsigned long int c;
		char str[100]={0};
		while((c=getc(fp))!=EOF)
		{
			
			if(c==' ')	
			{
				motion_r[motionPage].motor[motorValCount]=atol(str);
				strCount=0;
				for(int i=0;i<100;i++)
					str[i]=0;
				motorValCount++;
			}
			else
			{
				if(c=='\n')
				{
					motionPage++;
					motorValCount=0;
				}
				else
				{
					str[strCount]=c;
					strCount++;
				}
			}
		}
		fclose(fp);

	}
	return motionPage;
}

//-SetCam-//
void CSnakeHMIDlg::OnBnClickedButtonSetcam()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	//*--
	hMyWnd=this->m_hWnd;
	CAPSTATUS CapStatus;

	// Step 1. 建立 Capture Window 並且把這個 Window 貼在 Dialog 上
	hWndC = capCreateCaptureWindow (
		_T("My Capture Window"),// window name if pop-up 
		WS_CHILD | WS_VISIBLE,      // window style 
		LEFT_LIMIT,UP_LIMIT,160,120,             // window position and dimensions
		this->m_hWnd,               // 不能給 NULL, 否則會失敗
		0);			// child ID

	// Step 2. 與硬體驅動程式連線
	LRESULT  fOK = ::SendMessage (hWndC, WM_CAP_DRIVER_CONNECT, 0, 0L); 


	// Step 3. 列舉出目前系統所擁有的 Capture Driver//(此項暫不使用)


	// Step 4. 取得目前 Capture 的功能 
	// (我們藉此判斷使用者是否可以設定 Video Source 與 Video Format) 
	CAPDRIVERCAPS CapDriverCaps;
	::SendMessage (hWndC, WM_CAP_DRIVER_GET_CAPS, \
		sizeof (CAPDRIVERCAPS), (LONG) (LPVOID) &CapDriverCaps); 

	// Step 5: 讓使用者使用者選定的 Video Source
	if (CapDriverCaps.fHasDlgVideoSource)
		capDlgVideoSource(hWndC); 

	// Step 6: 讓使用者調整的 Video Format
	if (CapDriverCaps.fHasDlgVideoFormat){
		capDlgVideoFormat(hWndC); 

		// Are there new image dimensions?
		capGetStatus(hWndC, &CapStatus, sizeof (CAPSTATUS));

		// If so, notify the parent of a size change.
	}

	// Step 7: 讓使用者調整影像的輸出屬性 (亮度,對比,彩度)
	if (CapDriverCaps.fHasDlgVideoDisplay)
		capDlgVideoDisplay(hWndC);

	// Step 8: 得到最後的硬體狀態
	BOOL bOK=capGetStatus(hWndC, &CapStatus, sizeof (CAPSTATUS)); 

	// Step 9: 調整 Capture視窗的大小
	::SetWindowPos(hWndC, NULL, 0, 0, CapStatus.uiImageWidth, \
		CapStatus.uiImageHeight, SWP_NOZORDER | SWP_NOMOVE);

	// Step 10. Obtaining and Setting the Video Format
	DWORD dwSize;
	dwSize = capGetVideoFormatSize(hWndC);
	lpbi = (LPBITMAPINFO) GlobalAllocPtr(GHND, dwSize);
	capGetVideoFormat(hWndC, lpbi, dwSize);
	capGetVideoFormat(hWndC, lpbi, dwSize);

	// Step 11. Previewing Video
	capPreviewRate(hWndC, 1000/1000);     // rate, in milliseconds
	capPreview(hWndC, TRUE);       // starts preview 
	//capOverlay(hWndC, TRUE);       // starts overlay 

	// Step 12. 當一張 frame 填滿後,呼叫登記的 function 處理
	BOOL bOk= capSetCallbackOnFrame(hWndC, capVideoStreamCallback);  // end of OnBnClickedOk
}

void CSnakeHMIDlg::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	SetTimer(1, 10, NULL);
}



//-Img-//

void CSnakeHMIDlg::ReadImage()
{

	CFileDialog dlg(TRUE,L"bmp",L"*.bmp",OFN_HIDEREADONLY, L"Bmp Files(*.bmp)|*.bmp||",AfxGetMainWnd());//建立讀取圖片對話視窗
	Img->Destroy();//Destroy原Image
	Invalidate();
	if (dlg.DoModal()==IDOK)//判斷是否按下讀取圖片對話框的確定鍵
	{
		ImgPathName=dlg.GetPathName();//取得路徑名稱
		if (FAILED(Img->Load(ImgPathName)))//讀取圖片至Img並判斷是否錯誤
		{
			MessageBox(_T("讀取錯誤"),_T(""),MB_ICONWARNING | MB_OK);
			UpdateData();
			m_InfoText = _T("讀取錯誤");
			UpdateData(false);
		}
		else
		{
			ImageWidth=Img->GetWidth();
			ImageHeight=Img->GetHeight();
		}
	}	
}


void CSnakeHMIDlg::RGBtoHSL(unsigned char ***ImgData,double ***HslData, UINT ImageWidth, UINT ImageHeight)
{
	double r,g,b,max,min,limit=255.0;
	for (int Y=0;Y<ImageHeight;Y++)
	{
		for (int X=0;X<ImageWidth;X++)
		{
			max=0;
			min=1;
			r=ImgData[0][X][Y]/limit;
			g=ImgData[1][X][Y]/limit;
			b=ImgData[2][X][Y]/limit;

			if (max<r)
				max=r;
			if (max<g)
				max=g;
			if (max<b)
				max=b;
			if (min>r)
				min=r;
			if (min>g)
				min=g;
			if (min>b)
				min=b;
			//H
			if (max==min)
				HslData[0][X][Y]=0;
			else if (max==r && g>=b)
				HslData[0][X][Y]=60*((g-b)/(max-min))+0;
			else if (max==r && g< b)
				HslData[0][X][Y]=60*((g-b)/(max-min))+360;
			else if (max==g)
				HslData[0][X][Y]=60*((b-r)/(max-min))+120;
			else if (max==b)
				HslData[0][X][Y]=60*((r-g)/(max-min))+240;

			//L
			HslData[2][X][Y]=0.5*(max+min);
			//S
			if (HslData[2][X][Y]==0 || max==min)
				HslData[1][X][Y]=0;
			else if (0<HslData[2][X][Y] && HslData[2][X][Y]<=0.5)
				HslData[1][X][Y]=(max-min)/(2.0*HslData[2][X][Y]);
			else if (HslData[2][X][Y]>0.5)
				HslData[1][X][Y]=(max-min)/(2.0-2.0*HslData[2][X][Y]);

		}
	}
}
double CSnakeHMIDlg::GetColorC(double p,double q,double tC)
{
	double ColorC=0;
	while(tC<0||tC>1.0)
	{
		if(tC<0)
			tC=tC+1.0;
		if(tC>1)
			tC=tC-1.0;
	}
	if(tC<1.0/6.0)
		ColorC=p+((q-p)*6.0*tC);
	else if(1.0/6.0<=tC&&tC<1.0/2.0)
		ColorC=q;
	else if(1.0/2.0<=tC&&tC<2.0/3.0)
		ColorC=p+((q-p)*6.0*(2.0/3.0-tC));
	else
		ColorC=p;
	return ColorC;

}
void CSnakeHMIDlg::HSLtoRGB(unsigned char ***ImgData,double ***HslData, UINT ImageWidth, UINT ImageHeight)
{
	FILE*fp=freopen("CONOUT$","wt",stdout);								//這四行加到你要的地方
	freopen("CONIN$","rt",stdin); 
	double q=0,p=0,hk=0,tR=0,tG=0,tB=0,limit=255;
	for (int Y=0;Y<ImageHeight;Y++)
	{
		for (int X=0;X<ImageWidth;X++)
		{
			if(HslData[1][X][Y]==0)
			{
				ImgData[0][X][Y]=HslData[2][X][Y]*limit;
				ImgData[1][X][Y]=HslData[2][X][Y]*limit;
				ImgData[2][X][Y]=HslData[2][X][Y]*limit;
			}
			else
			{
				if(HslData[2][X][Y]<0.5)
					q=HslData[2][X][Y]*(1+HslData[1][X][Y]);
				else if(HslData[2][X][Y]>=0.5)
					q=HslData[2][X][Y]+HslData[1][X][Y]-(HslData[2][X][Y]*HslData[1][X][Y]);
				p=2*HslData[2][X][Y]-q;
				if(HslData[0][X][Y]==0)
					hk=0;
				else
					hk=HslData[0][X][Y]/360.0;
				tR=hk+(1.0/3.0);
				tG=hk;
				tB=hk-(1.0/3.0);
				ImgData[0][X][Y]=GetColorC(p,q,tR)*limit;
				ImgData[1][X][Y]=GetColorC(p,q,tG)*limit;
				ImgData[2][X][Y]=GetColorC(p,q,tB)*limit;
				if(ImgData[0][X][Y]>255)
					ImgData[0][X][Y]=255;
				else if(ImgData[0][X][Y]<0)
					ImgData[0][X][Y]=0;
				if(ImgData[1][X][Y]>255)
					ImgData[1][X][Y]=255;
				else if(ImgData[1][X][Y]<0)
					ImgData[1][X][Y]=0;
				if(ImgData[2][X][Y]>255)
					ImgData[2][X][Y]=255;
				else if(ImgData[2][X][Y]<0)
					ImgData[2][X][Y]=0;
			}
		}
	}

}




void CSnakeHMIDlg::ImageProcess()
{
	UpdateData();
	COLORREF color;
	CClientDC thisDC(this);			// 用及時繪圖速度比用 CImage 的 Draw() 還快！

	FILE*fp=freopen("CONOUT$","wt",stdout);								//這四行加到你要的地方
	freopen("CONIN$","rt",stdin); 

	ImageWidth=lpbi->bmiHeader.biWidth;
	ImageHeight=lpbi->bmiHeader.biHeight;
	Array3DforUCHAR ImgData(3,ImageWidth,ImageHeight);//得到要處理圖片的長和寬之後，定義一個符合格式的三維陣列
	Array3DforDouble HslData(3,ImageWidth,ImageHeight);
	Array3DforUCHAR RGBData(3,ImageWidth,ImageHeight);
	ImageProcessFunction g_imageTool(ImageWidth, ImageHeight);// 影像工具類別
	COLORREF Color;
	////////////////////// 進行處理 //////////////////////
	double dY, dCb, dCr;// YCbCr
	int nCbCrAngle=0;
	int nDrawRange=0;
	int error1=-10;
	int error2=25;//color range,bin range is small
	int error3=20;//color range,bin range is small
	double dR=0,dG=0,dB=0;
	double dY2=0,dCb2=0,dCr2=0;
	Array3DforUCHAR g_pSourceImage(3, ImageWidth, ImageHeight);
	Array3DforDouble g_pHSLImage(3, ImageWidth, ImageHeight);

	Array3DforUCHAR g_pRGBImage(3, ImageWidth, ImageHeight);
	Array2DforUCHAR g_pGrayImage(ImageWidth, ImageHeight);
	Array2DforUCHAR g_pBinarizationImage(ImageWidth, ImageHeight);
	Array2DforUCHAR g_pBufImage(ImageWidth, ImageHeight);
	Array2DforUCHAR g_pBufImage2(ImageWidth, ImageHeight);
	Array2DforUCHAR g_pBufImage3(ImageWidth, ImageHeight);
	Array2DforUCHAR g_pBufImageTemp(ImageWidth, ImageHeight);
	Array2DforUCHAR g_pOpenImage(ImageWidth, ImageHeight);
	Array2DforUCHAR g_pEdgeImage(ImageWidth, ImageHeight);
	double dataByte=lpVHdrTemp->dwBufferLength/(lpbi->bmiHeader.biHeight*lpbi->bmiHeader.biWidth);
	if(dataByte==2)
	{
		//YUY2轉RGB
		//B = 1.164(Y - 16)         + 2.018(U - 128)
		//G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
		//R = 1.164(Y - 16) + 1.596(V - 128)
		int a=0,b=0;
		unsigned char *pData = (unsigned char *)lpVHdrTemp->lpData;
		for(DWORD count=0;count<lpVHdrTemp->dwBytesUsed	;count+=4)
		{
			//Y0 U0 Y1 V0
			float R,G,B;
			float Y0 = *pData++;
			float U  = *pData++;
			float Y1 = *pData++;
			float V  = *pData++;
			R=(Y0 + (1.370705 * (V-128)));
			G=(Y0 - (0.698001 * (V-128)) - (0.337633 * (U-128)));
			B=(Y0 + (1.732446 * (U-128)));
			if(R<0) R =0;
			if(R>255) R=255;
			if(G<0) G =0;
			if(G>255) G=255;
			if(B<0) B =0;
			if(B>255) B=255;
			g_pSourceImage.Data[2][a][b]= (BYTE)B;
			g_pSourceImage.Data[1][a][b]= (BYTE)G;
			g_pSourceImage.Data[0][a][b]= (BYTE)R;
			R=(Y1 + (1.370705 * (V-128)));
			G=(Y1 - (0.698001 * (V-128)) - (0.337633 * (U-128)));
			B=(Y1 + (1.732446 * (U-128)));
			if(R<0) R =0;
			if(R>255) R=255;
			if(G<0) G =0;
			if(G>255) G=255;
			if(B<0) B =0;
			if(B>255) B=255;
			g_pSourceImage.Data[2][a+1][b]= (UCHAR)B;
			g_pSourceImage.Data[1][a+1][b]= (UCHAR)G;
			g_pSourceImage.Data[0][a+1][b]= (UCHAR)R;
			a=a+2;
			if(a==lpbi->bmiHeader.biWidth)
			{
				a=0;
				b++;
			}
		}
	}else if(dataByte==3)
	{
		int i=0;
		for (int Y=ImageHeight-1; Y>=0; Y--)
		{
			for (int X=0; X<ImageWidth; X++)
			{
				//RGB24順序為BGR
				g_pSourceImage.Data[2][X][Y]=(double)*(lpVHdrTemp->lpData+i*3);//B
				g_pSourceImage.Data[1][X][Y]=(double)*(lpVHdrTemp->lpData+1+i*3);//G
				g_pSourceImage.Data[0][X][Y]=(double)*(lpVHdrTemp->lpData+2+i*3);//R

				i++;
			}
		}
	}
	else
	{
		int i=0;
		for (int Y=ImageHeight-1; Y>=0; Y--)
		{
			for (int X=0; X<ImageWidth; X++)
			{
				
				g_pSourceImage.Data[2][X][Y]=255;//B
				g_pSourceImage.Data[1][X][Y]=255;//G
				g_pSourceImage.Data[0][X][Y]=255;//R

				i++;
			}
		}
	}



	//顯示RGB
	for (int Y=0; Y<ImageHeight; Y++)
	{
		for (int X=0; X<ImageWidth; X++)
		{
			color=RGB(g_pSourceImage.Data[0][X][Y],g_pSourceImage.Data[1][X][Y],g_pSourceImage.Data[2][X][Y]);
			thisDC.SetPixel(1*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,0*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
		}
	}

	//*/

	RGBtoHSL(g_pSourceImage.Data, g_pHSLImage.Data, ImageWidth, ImageHeight);
	//*L濾波
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pBufImage2.Data[X][Y]=(UCHAR)((double)g_pHSLImage.Data[2][X][Y]*255);
	
		}
	}
	g_imageTool.Mediam(g_pBufImage2.Data,g_pBufImageTemp.Data,ImageWidth, ImageHeight);
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pHSLImage.Data[2][X][Y]=(double)g_pBufImageTemp.Data[X][Y]/255;	
		}
	}//*/

	//*S濾波
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pBufImage.Data[X][Y]=(UCHAR)((double)g_pHSLImage.Data[1][X][Y]*255);
	
		}
	}
	g_imageTool.Mediam(g_pBufImage.Data,g_pBufImageTemp.Data,ImageWidth, ImageHeight);
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pHSLImage.Data[1][X][Y]=(double)g_pBufImageTemp.Data[X][Y]/255;	
		}
	}//*/


	/*H濾波
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pBufImage3.Data[X][Y]=(UCHAR)((double)g_pHSLImage.Data[0][X][Y]/360*255);
	
		}
	}
	g_imageTool.Mediam(g_pBufImage3.Data,g_pBufImageTemp.Data,ImageWidth, ImageHeight);
	//g_imageTool.Laplace(g_pBufImageTemp.Data,g_pBufImage2.Data,ImageWidth, ImageHeight);
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pHSLImage.Data[0][X][Y]=(double)g_pBufImageTemp.Data[X][Y]/255*360;	
		}
	}//*/
	//顯示HSL
	for (int Y=0; Y<ImageHeight; Y++)
	{
		for (int X=0; X<ImageWidth; X++)
		{
			color=RGB(g_pHSLImage.Data[0][X][Y]/360*255,g_pHSLImage.Data[1][X][Y]*255,g_pHSLImage.Data[2][X][Y]*255);
			thisDC.SetPixel(2*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,0*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
		}
	}
	//顯示H
	for (int Y=0; Y<ImageHeight; Y++)
	{
		for (int X=0; X<ImageWidth; X++)
		{
			color=RGB(g_pHSLImage.Data[0][X][Y]/360*255,g_pHSLImage.Data[0][X][Y]/360*255,g_pHSLImage.Data[0][X][Y]/360*255);
			thisDC.SetPixel(0*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,1*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
		}
	}
	//顯示S
	for (int Y=0; Y<ImageHeight; Y++)
	{
		for (int X=0; X<ImageWidth; X++)
		{
			color=RGB(g_pHSLImage.Data[1][X][Y]*255,g_pHSLImage.Data[1][X][Y]*255,g_pHSLImage.Data[1][X][Y]*255);
			thisDC.SetPixel(1*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,1*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
		}
	}
	//顯示L
	for (int Y=0; Y<ImageHeight; Y++)
	{
		for (int X=0; X<ImageWidth; X++)
		{
			color=RGB(g_pHSLImage.Data[2][X][Y]*255,g_pHSLImage.Data[2][X][Y]*255,g_pHSLImage.Data[2][X][Y]*255);
			thisDC.SetPixel(2*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,1*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
		}
	}



//*
	if(gdwFrameNum>1)
	{
		//顯示Old HSL
		for (int Y=0; Y<ImageHeight; Y++)
		{
			for (int X=0; X<ImageWidth; X++)
			{
				color=RGB(g_pOldHSLImage[0][X][Y]/360*255,g_pOldHSLImage[1][X][Y]*255,g_pOldHSLImage[2][X][Y]*255);
				thisDC.SetPixel(3*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,1*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
			}
		}
	}
	//儲存Old HSL
	for (int Y=0; Y<ImageHeight; Y++)
	{
		for (int X=0; X<ImageWidth; X++)
		{
			g_pOldHSLImage[0][X][Y]=g_pHSLImage.Data[0][X][Y];
			g_pOldHSLImage[1][X][Y]=g_pHSLImage.Data[1][X][Y];
			g_pOldHSLImage[2][X][Y]=g_pHSLImage.Data[2][X][Y];
		}
	}
	//*/



		//------------------------------------------------------------------------------------------------------
	UCHAR HistogramArray[256]={0};// 直方圖暫存陣列(陣列元素初始值如果只給其中幾個,剩下的值自動為零)
	double Probability[256]={0};// 機率暫存陣列;
	double max = 0;
//*
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pBufImage2.Data[X][Y]=(UCHAR)(g_pHSLImage.Data[2][X][Y]*255);
	
		}
	}
	g_imageTool.Histogram(ImageWidth,ImageHeight,g_pBufImage2.Data,HistogramArray);//直方圖統計
	

	//正規化
	
	g_imageTool.Normalization(ImageWidth,ImageHeight,g_pBufImage2.Data,HistogramArray,Probability);
	
	for (int i=0;i<256;i++)
	{
		if(Probability[i]>max)
		{
			max=Probability[i];
		}
	}
	//將Image影像進行直方圖等化
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pHSLImage.Data[2][X][Y]=Probability[g_pBufImage2.Data[X][Y]]/max;//max計算對應灰階值(0~1)
		}
	}
	//*/

	//*若過暗
	double GraySum=0,AvgLight=0;
	for (int i=0; i<256; i++)
	{	
		GraySum+=HistogramArray[i];

	}
	AvgLight=GraySum/((double)(ImageWidth*ImageHeight));
	if(AvgLight<100)
	{
		//開燈
	}
	//*/

	//------------------------------------------------------------------------------------------------------
	//*
	for (int i=0;i<256;i++)
	{
		HistogramArray[i]=0;
	}
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pBufImage.Data[X][Y]=(UCHAR)(g_pHSLImage.Data[1][X][Y]*255);
	
		}
	}
	g_imageTool.Histogram(ImageWidth,ImageHeight,g_pBufImage.Data,HistogramArray);//直方圖統計
		//正規化
	for (int i=0;i<256;i++)
	{
		Probability[i]=0;
	}
	g_imageTool.Normalization(ImageWidth,ImageHeight,g_pBufImage.Data,HistogramArray,Probability);
	max = 0;
	for (int i=0;i<256;i++)
	{
		if(Probability[i]>max)
		{
			max=Probability[i];
		}
	}
	//將Image影像進行直方圖等化
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pHSLImage.Data[1][X][Y]=Probability[g_pBufImage.Data[X][Y]]/max;//計算對應灰階值(0~1)
		}
	}//*/
	//------------------------------------------------------------------------------------------------------
	/*
	for (int i=0;i<256;i++)
	{
		HistogramArray[i]=0;
	}
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pBufImage.Data[X][Y]=(UCHAR)(g_pHSLImage.Data[0][X][Y]*255);
	
		}
	}
	g_imageTool.Histogram(ImageWidth,ImageHeight,g_pBufImage.Data,HistogramArray);//直方圖統計
	/*
	//顯示直方圖
	for (int Y=0;Y<ImageWidth;Y++)// 直方圖
	{
		color = RGB(255, 255, 255);
		for (int X=0; X<ImageWidth; X++)
		{
			thisDC.SetPixel(0*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,2*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
		}
	}
	for (int X=0; X<ImageWidth; X++)
	{
		for (int Y=ImageWidth-1-HistogramArray[X]; Y<ImageWidth; Y++)//?
		{
			if (fmod((float)X, (float)20)==0)//浮點數相除取餘數(浮點數的餘數)//?
			{
				color = RGB(255, 0, 0);
			}
			else
			{
				color = RGB(0, 0, 0);
			}
			if (Y<0)
			{
				thisDC.SetPixel(0*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,2*(ImageHeight+UP_LIMIT)+UP_LIMIT+0,color);
			}
			else
			{
				thisDC.SetPixel(0*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,2*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
			}
		}
	}//*/
	/*for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pBufImage.Data[X][Y]=g_pHSLImage.Data[1][X][Y];
	
		}
	}*/
	/*
	//正規化
	for (int i=0;i<256;i++)
	{
		Probability[i]=0;
	}
	g_imageTool.Normalization(ImageWidth,ImageHeight,g_pBufImage.Data,HistogramArray,Probability);
	max = 0;
	for (int i=0;i<256;i++)
	{
		if(Probability[i]>max)
		{
			max=Probability[i];
		}
	}
	//將Image影像進行直方圖等化
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pHSLImage.Data[0][X][Y]=Probability[g_pBufImage.Data[X][Y]]/max;//計算對應灰階值(0~1)
		}
	}//*/
	/*
	for (int Y=0;Y<ImageWidth;Y++)//
	{
		for (int X=0; X<ImageWidth; X++)
		{
			color=RGB(g_pBufImage.Data[X][Y],g_pBufImage.Data[X][Y],g_pBufImage.Data[X][Y]);
			thisDC.SetPixel(1*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,2*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
		}
	}//*/
	//------------------------------------------------------------------------------------------------------



	int Threshold=20;
	//*H
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pBufImage.Data[X][Y]=(UCHAR)((double)g_pHSLImage.Data[0][X][Y]/360*255);
	
		}
	}
	g_imageTool.Histogram(ImageWidth,ImageHeight,g_pBufImage.Data,HistogramArray);//直方圖統計
	g_imageTool.Normalization(ImageWidth,ImageHeight,g_pBufImage.Data,HistogramArray,Probability);
	Threshold=g_imageTool.Otsu(ImageWidth, ImageHeight,g_pBufImage.Data,Probability);
	g_imageTool.Observation(Threshold,ImageWidth,ImageHeight,g_pBufImage.Data,g_pBinarizationImage.Data);

	for (int Y=0; Y<ImageHeight; Y++)
	{
		for (int X=0; X<ImageWidth; X++)
		{
			color=RGB(g_pBinarizationImage.Data[X][Y],g_pBinarizationImage.Data[X][Y],g_pBinarizationImage.Data[X][Y]);
			thisDC.SetPixel(0*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,2*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
		}
	}
//
	//*S
	Threshold=20;
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pBufImage2.Data[X][Y]=(UCHAR)((double)g_pHSLImage.Data[1][X][Y]*255);
	
		}
	}
	g_imageTool.Histogram(ImageWidth,ImageHeight,g_pBufImage2.Data,HistogramArray);//直方圖統計
	g_imageTool.Normalization(ImageWidth,ImageHeight,g_pBufImage2.Data,HistogramArray,Probability);
	Threshold=g_imageTool.Otsu(ImageWidth, ImageHeight,g_pBufImage2.Data,Probability);
	g_imageTool.Observation(Threshold,ImageWidth,ImageHeight,g_pBufImage2.Data,g_pBinarizationImage.Data);
	
	
	for (int Y=0; Y<ImageHeight; Y++)
	{
		for (int X=0; X<ImageWidth; X++)
		{
			color=RGB(g_pBinarizationImage.Data[X][Y],g_pBinarizationImage.Data[X][Y],g_pBinarizationImage.Data[X][Y]);
			thisDC.SetPixel(1*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,2*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
		}
	}
	


	//*L
	Threshold=20;
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pBufImage3.Data[X][Y]=(UCHAR)((double)g_pHSLImage.Data[2][X][Y]*255);
	
		}
	}
	g_imageTool.Histogram(ImageWidth,ImageHeight,g_pBufImage3.Data,HistogramArray);//直方圖統計
	g_imageTool.Normalization(ImageWidth,ImageHeight,g_pBufImage3.Data,HistogramArray,Probability);
	g_imageTool.Observation(Threshold,ImageWidth,ImageHeight,g_pBufImage3.Data,g_pBufImageTemp.Data);
	g_imageTool.Opening(g_pBufImageTemp.Data,g_pBufImage3.Data,ImageWidth,ImageHeight);
	g_imageTool.Closing(g_pBufImage3.Data,g_pBufImageTemp.Data,ImageWidth,ImageHeight);
	
	
	for (int Y=0; Y<ImageHeight; Y++)
	{
		for (int X=0; X<ImageWidth; X++)
		{
			color=RGB(g_pBufImageTemp.Data[X][Y],g_pBufImageTemp.Data[X][Y],g_pBufImageTemp.Data[X][Y]);
			thisDC.SetPixel(2*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,2*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
		}
	}
	//HS
	//*H
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pBufImage.Data[X][Y]=(UCHAR)((double)g_pHSLImage.Data[0][X][Y]/360*255);
	
		}
	}
	//*S
	Threshold=20;
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			g_pBufImage2.Data[X][Y]=(UCHAR)((double)g_pHSLImage.Data[1][X][Y]*255);
	
		}
	}
	for(int Y=0;Y<ImageHeight;Y++)
	{
		for(int X=0;X<ImageWidth;X++)
		{
			if(g_pBufImage.Data[X][Y]>50 && g_pBufImage.Data[X][Y]<120 && g_pBufImage2.Data[X][Y]>50 && g_pBufImage2.Data[X][Y]<120)
				g_pBufImageTemp.Data[X][Y]=0;
			else
				g_pBufImageTemp.Data[X][Y]=255;	
		}
	}
	for (int Y=0; Y<ImageHeight; Y++)
	{
		for (int X=0; X<ImageWidth; X++)
		{
			color=RGB(g_pBufImageTemp.Data[X][Y],g_pBufImageTemp.Data[X][Y],g_pBufImageTemp.Data[X][Y]);
			thisDC.SetPixel(3*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,2*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
		}
	}





	int countX=0,countY=0,countBlack=0,r=3,maxR=ImageHeight+ImageWidth;
	for (int Y=0; Y<ImageHeight; Y++)
	{
		for (int X=0; X<ImageWidth; X++)
		{

			if(g_pHSLImage.Data[2][X][Y]*255<20)
			{
				countX+=X;
				countY+=Y;
				countBlack++;
			}
			
		}
	}
	countX/=countBlack;
	countY/=countBlack;

	HSLtoRGB(g_pRGBImage.Data, g_pHSLImage.Data, ImageWidth, ImageHeight);



	for (int Y=0; Y<ImageHeight; Y++)
	{
		for (int X=0; X<ImageWidth; X++)
		{
			if(g_pHSLImage.Data[2][X][Y]*255<20)
				color=RGB(0,0,255);
			else
				color=RGB(g_pRGBImage.Data[0][X][Y],g_pRGBImage.Data[1][X][Y],g_pRGBImage.Data[2][X][Y]);
			thisDC.SetPixel(3*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT+X,0*(ImageHeight+UP_LIMIT)+UP_LIMIT+Y,color);
		}
	}

	SignArea(countY-r, countY+r, countX-r, countX+r, 3*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT, 0*(ImageHeight+UP_LIMIT)+UP_LIMIT, 255, 0, 0);//紅色圈代表重心位置


	for (int Y=0; Y<ImageHeight; Y++)
	{
		for (int X=0; X<ImageWidth; X++)
		{
			if(g_pHSLImage.Data[2][X][Y]*255<20)
			{
				if(sqrt(pow((double)abs(X-countX),2)+pow((double)abs(Y-countY),2))<maxR)
				{
					countBlack=X;
					r=Y;
					maxR=sqrt(pow((double)abs(X-countX),2)+pow((double)abs(Y-countY),2));
				}
			}
			
		}
	}
	countX=countBlack;
	countY=r;
	r=3;
	SignArea(countY-r, countY+r, countX-r, countX+r, 3*(ImageWidth+LEFT_LIMIT)+LEFT_LIMIT, 0*(ImageHeight+UP_LIMIT)+UP_LIMIT, 0, 255, 0);//綠色圈代表最靠近重心位置的點


	UpdateData(false);

}

int CSnakeHMIDlg::ReadBMP(UCHAR ***ucImageOut, int nImageWidth, int nImageHeight, char *fileName)
{
	FILE *pFile;
	unsigned char *Image_buf;
	char fname_bmp[128];
	unsigned char header[54];

	Image_buf=(unsigned char *)malloc((size_t)nImageWidth*nImageHeight*3);

	if(Image_buf==NULL) return -1;
	sprintf(fname_bmp,"%s.bmp",fileName);
	if((pFile=fopen(fname_bmp,"rb"))==NULL) return -1;

	fread(header,sizeof(unsigned char),54,pFile);
	fread(Image_buf,sizeof(unsigned char),(size_t)(long)nImageWidth*nImageHeight*3, pFile);
	fclose(pFile);

	for(int i=0;i<nImageHeight;i++)
	{
		for(int j=0;j<nImageWidth;j++)
		{
			ucImageOut[0][j][nImageHeight-1-i] = *(Image_buf+3*(nImageWidth*i+j)+2);
			ucImageOut[1][j][nImageHeight-1-i] = *(Image_buf+3*(nImageWidth*i+j)+1);
			ucImageOut[2][j][nImageHeight-1-i] = *(Image_buf+3*(nImageWidth*i+j));

		}
	}
	free(Image_buf);
	return 0;
}
void CSnakeHMIDlg::OnBnClickedCancel()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	EndProgram();
	KillTimer(1);
	OnCancel();
}

void CSnakeHMIDlg::OnBnClickedButton7()
{
	// TODO: 在此加入控制項告知處理常式程式碼

}


LRESULT CALLBACK capVideoStreamCallback(HWND hWnd,LPVIDEOHDR lpVHdr){

	FILE*fp=freopen("CONOUT$","wt",stdout);								//這四行加到你要的地方
	freopen("CONIN$","rt",stdin); 
	if (!hWnd) 
		return FALSE;   
//*
	wsprintf((LPWSTR)gachBuffer, L"Preview frame# %ld 一個pixel=%d bits; 寬=%d; 高=%d (RGB)=(%d,%d,%d) 使用的長度=%d" ,\
		gdwFrameNum++, lpbi->bmiHeader.biBitCount,\
		lpbi->bmiHeader.biWidth, lpbi->bmiHeader.biHeight,\
		(int)*(lpVHdr->lpData),\
		(int)*(lpVHdr->lpData+1),\
		(int)*(lpVHdr->lpData+2),\
		lpVHdr->dwBytesUsed		);
	SetWindowText(hMyWnd, (LPCWSTR)gachBuffer); 
	std::cout <<"R="<<(int)*(lpVHdr->lpData)<<",G="<<(int)*(lpVHdr->lpData+1)<<",B="<<(int)*(lpVHdr->lpData+2)<<" "<<std::endl;
	lpVHdrTemp=lpVHdr;
	imgProcessFlag=true;

	return (LRESULT) TRUE ;

}


void CSnakeHMIDlg::SignArea(int nTopY, int nBottomY, int nLeftX, int nRightX, int nShiftX, int nShiftY, UCHAR r, UCHAR g, UCHAR b)
{
	CClientDC aDC(this);
	COLORREF color;
	double Circle_R=0,Circle_X=0,Circle_Y=0;
	int X=0,Y=0;
	Circle_R=((double)(nRightX-nLeftX))/2.0;
	Circle_X=nRightX-Circle_R;
	Circle_Y=nBottomY-Circle_R;
	for (int i = 0 ; i < 360 ; i++)
	{
			color=RGB(r, g, b);
			X=nShiftX+Circle_X+Circle_R*cos((double)i*M_PI/180.0);
			Y=nShiftY+Circle_Y-Circle_R*sin((double)i*M_PI/180.0);
			aDC.SetPixel(X, Y, color);
	}
}

void CSnakeHMIDlg::OnBnClickedButton1()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	Action(motionPageF,motionF);//執行前進
	state=1;
	stateChangeFlag=1;
}

void CSnakeHMIDlg::OnBnClickedButton3()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	Action(motionPageL,motionL);//執行前進
	state=2;
	stateChangeFlag=1;
}

void CSnakeHMIDlg::OnBnClickedButton2()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	RunSensor();
}
BOOL CSnakeHMIDlg::PreTranslateMessage(MSG* pMsg) //重載函示，按下ESC取消動作
{ 
	if   (pMsg-> message==WM_KEYDOWN) 
	{ 
		if   (pMsg-> wParam==VK_ESCAPE) 
		{
			KillTimer(1);
			return   TRUE; 
		}
	} 
	return   CDialog::PreTranslateMessage(pMsg); 
} 
void CSnakeHMIDlg::OnBnClickedButton4()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	Action(motionPageR,motionR);//執行前進
	state=3;
	stateChangeFlag=1;
}
