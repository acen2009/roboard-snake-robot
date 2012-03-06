// Snake HMIDlg.h : 標頭檔
//
#include <atlimage.h>// CImage
#include "ImageProcessFunction.h"
#include "Vfw.h"
#include <windowsx.h>                 // for GlobalAllocPtr
#pragma comment( lib, "Vfw32.lib" )   // 使用 Video For Window 32 library
//motor
#define MOTOR_NUM 11
#define MOTIOM_MAX 1000
typedef struct frames
{
	unsigned long motor[MOTOR_NUM];
};
// CSnakeHMIDlg 對話方塊
class CSnakeHMIDlg : public CDialog
{
// 建構
public:
	CSnakeHMIDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
	enum { IDD = IDD_SNAKEHMI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSetcam();

	void Set();
	void Run();
	void EndProgram();

	void ErrorMsg(unsigned char addr);
	unsigned int ReadI2CCmd(unsigned char addr, unsigned char rwbit, int count);
	void SendI2CCmd(unsigned char addr, unsigned char rwbit, int count,unsigned char cmd);
	unsigned int GetSenserVal(unsigned char senserAddr,unsigned char cmd);
	unsigned int GetSenserVal2Byte(unsigned char senserAddr,unsigned char highCmd,unsigned char lowCmd);
	int UnsignedIntToInt(unsigned int val);
	double GetPitch(double *aVal);
	double GetRoll(double *aVal);
	double GetAzimuth(double *mVal, double pitch, double roll);

	void ImgProcess();
	void RunSensor();
	void Action(int motionPage,frames *motion);
	void PreMove(unsigned char *oldPosition,unsigned char *nextPosition);
	void Move(int motionPageIn,frames *motion_r);
	int ReadServoFile(char *filename,frames *motion_r);
	void ReadImage();
	void RGBtoHSL(unsigned char ***ImgData,double ***HslData, UINT ImageWidth, UINT ImageHeight);
	double GetColorC(double p,double q,double tC);
	void HSLtoRGB(unsigned char ***ImgData,double ***HslData, UINT ImageWidth, UINT ImageHeight);
	void ImageProcess();
	int ReadBMP(UCHAR ***ucImageOut, int nImageWidth, int nImageHeight, char *fileName);
	void SignArea(int nTopY, int nBottomY, int nLeftX, int nRightX, int nShiftX, int nShiftY, UCHAR r, UCHAR g, UCHAR b);
	BOOL CSnakeHMIDlg::PreTranslateMessage(MSG* pMsg) ;

	CImage *Img,*HslImg,*RGBImg;
	CString ImgPathName;
	UINT ImageHeight, ImageWidth;
	bool ImgFlag;
	CString m_strAzimuth;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	CString m_InfoText;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
};
