#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <math.h>

#define M_PI		3.14159265358979323846
#define M_SQRT2		1.41421356237309504880

using namespace std;

class CImageTool
{
protected:
	UCHAR **ucTemp;

public:
	CImageTool(int nImageHeight, int nImageWidth)
	{
		ucTemp = (UCHAR**)malloc2d(nImageWidth, nImageHeight, sizeof(UCHAR));
	}

	~CImageTool()
	{
		free(ucTemp);
	}

public:
	//動態矩陣變數宣告
	void* malloc2d( int w, int h, int size )
	{
		void **a = (void**) malloc( w*sizeof(void*) + w*h*size );
		for(int j=0; j<w; j++ )
			a[j] = ((char *)(a+w)) + j*h*size;
		return a;
	}

	// 讀圖
	int ReadBMP(UCHAR *ucImageOut, int nImageWidth, int nImageHeight, char *fileName)
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
				*(ucImageOut+nImageWidth*(nImageHeight-i-1)+j) = *(Image_buf+3*(nImageWidth*i+j)+2);
				*(ucImageOut+nImageWidth*(nImageHeight-i-1)+j+nImageWidth*nImageHeight) = *(Image_buf+3*(nImageWidth*i+j)+1);
				*(ucImageOut+nImageWidth*(nImageHeight-i-1)+j+nImageWidth*nImageHeight*2) = *(Image_buf+3*(nImageWidth*i+j));
			}
		}
		free(Image_buf);
		return 0;
	}

	int ReadBMP(UCHAR ***ucImageOut, int nImageWidth, int nImageHeight, char *fileName)
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
				ucImageOut[0][nImageHeight-1-i][j] = *(Image_buf+3*(nImageWidth*i+j)+2);
				ucImageOut[1][nImageHeight-1-i][j] = *(Image_buf+3*(nImageWidth*i+j)+1);
				ucImageOut[2][nImageHeight-1-i][j] = *(Image_buf+3*(nImageWidth*i+j));
			}
		}
		free(Image_buf);
		return 0;
	}

	// 由RGB取得YCbCr的Y值
	double GetValueY(UCHAR R, UCHAR G, UCHAR B)
	{
		double ValueY = (0.299)*R + (0.587)*G + (0.114)*B;
		return ValueY;
	}

	// 由RGB取得YCbCr的Cb值
	double GetValueCb(UCHAR R, UCHAR G, UCHAR B)
	{
		double ValueCb = (-0.169)*R - (0.331)*G + (0.499)*B;
		return ValueCb;
	}

	// 由RGB取得YCbCr的Cr值
	double GetValueCr(UCHAR R, UCHAR G, UCHAR B)
	{
		double ValueCr = (0.499)*R + (-0.418)*G + (-0.0813)*B;
		return ValueCr;
	}

	// 由RGB取得YCbCr的Cr值
	double GetVectorCbCr(double Cb, double Cr)
	{
		double VectorCbCr = sqrt( pow(Cb, 2) + pow(Cr, 2) );
		return VectorCbCr;
	}

	// 取得CbCr平面角度
	double GetCbCrAngle(double Cb, double Cr)
	{
		double fThida;

		if (Cb>0 && Cr>0)// 第一象限
		{
			fThida = 180*atan(Cr/Cb)/M_PI;
		}
		else if (Cb<0 && Cr>0)// 第二象限
		{
			fThida = 180 + 180*atan(Cr/Cb)/M_PI;
		}
		else if (Cb<0 && Cr<0)// 第三象限
		{
			fThida = 180 + 180*atan(Cr/Cb)/M_PI;
		}
		else if (Cb>0 && Cr<0)// 第四象限
		{
			fThida = 2*180 + 180*atan(Cr/Cb)/M_PI;
		}
		else if (Cb>0 && Cr==0)// 水平零度角
		{
			fThida = 0;
		}
		else if (Cb<0 && Cr==0)// 水平180度角
		{
			fThida = 180;
		}
		else if (Cb==0 && Cr>0)// 垂直90度
		{
			fThida = 90;
		}
		else if (Cb==0 && Cr<0)// 垂直270度
		{
			fThida = 270;
		}
		else if (Cb==0 && Cr==0)// 原點
		{
			return -1;
		}

		return fThida;
	}

	// 膨脹
	int Dilation(UCHAR **ucImageIn, UCHAR **ucImageOut, int nImageWidth, int nImageHeight)
	{
		int nScanLeftX=0, nScanRightX, nScanTopY, nScanBottomY;

		for (int Y=0; Y<nImageHeight; Y++)
		{
			for (int X=0; X<nImageWidth; X++)
			{
				nScanLeftX=X-1; nScanRightX=X+1; nScanTopY=Y-1; nScanBottomY=Y+1;

				if (nScanLeftX<0){ nScanLeftX=0; }
				if (nScanRightX>nImageWidth-1){ nScanRightX=nImageWidth-1; }
				if (nScanTopY<0){ nScanTopY=0; }
				if (nScanBottomY>nImageHeight-1){ nScanBottomY=nImageHeight-1; }

				if (ucImageIn[nScanTopY][nScanLeftX]==0 || ucImageIn[Y][nScanLeftX]==0 || ucImageIn[nScanBottomY][nScanLeftX]==0 || 
					ucImageIn[nScanTopY][X]==0 || ucImageIn[Y][X]==0 || ucImageIn[nScanBottomY][X]==0 || 
					ucImageIn[nScanTopY][nScanRightX]==0 || ucImageIn[Y][nScanRightX]==0 || ucImageIn[nScanBottomY][nScanRightX]==0)
				{
					ucImageOut[Y][X]=0;
				}
				else{ ucImageOut[Y][X]=255; }
			}
		}
		return 0;
	}

	// 侵蝕
	int Erosion(UCHAR **ucImageIn, UCHAR **ucImageOut, int nImageWidth, int nImageHeight)
	{
		int nScanLeftX=0, nScanRightX, nScanTopY, nScanBottomY;

		for (int Y=0; Y<nImageHeight; Y++)	//膨脹
		{
			for (int X=0; X<nImageWidth; X++)
			{
				nScanLeftX = X-1;
				nScanRightX = X+1;
				nScanTopY = Y-1;
				nScanBottomY = Y+1;

				if (nScanLeftX<0){ nScanLeftX=0; }
				if (nScanRightX>nImageWidth-1){ nScanRightX=nImageWidth-1; }
				if (nScanTopY<0){ nScanTopY=0; }
				if (nScanBottomY>nImageHeight-1){ nScanBottomY=nImageHeight-1; }

				if (ucImageIn[nScanTopY][nScanLeftX]==0		|| ucImageIn[Y][nScanLeftX]==0		|| ucImageIn[nScanBottomY][nScanLeftX]==0 || 
					ucImageIn[nScanTopY][X]==0				|| ucImageIn[Y][X]==0				|| ucImageIn[nScanBottomY][X]==0 || 
					ucImageIn[nScanTopY][nScanRightX]==0	|| ucImageIn[Y][nScanRightX]==0		|| ucImageIn[nScanBottomY][nScanRightX]==0)
				{
					ucImageOut[Y][X]=0;
				}
				else{ ucImageOut[Y][X]=255; }
			}
		}
		return 0;
	}
	// 斷開
	int Opening(UCHAR **ucImageIn, UCHAR **ucImageOut, int nImageWidth, int nImageHeight)
	{
		int nScanLeftX, nScanRightX, nScanTopY, nScanBottomY;

		for (int Y=0; Y<nImageHeight; Y++)	//侵蝕
		{
			for (int X=0; X<nImageWidth; X++)
			{
				nScanLeftX = X-1;
				nScanRightX = X+1;
				nScanTopY = Y-1;
				nScanBottomY = Y+1;

				if (nScanLeftX<0){ nScanLeftX=0; }
				if (nScanRightX>nImageWidth-1){ nScanRightX=nImageWidth-1; }
				if (nScanTopY<0){ nScanTopY=0; }
				if (nScanBottomY>nImageHeight-1){ nScanBottomY=nImageHeight-1; }

				if (ucImageIn[nScanTopY][nScanLeftX]==0		&&	ucImageIn[Y][nScanLeftX]==0		&& ucImageIn[nScanBottomY][nScanLeftX]==0 && 
					ucImageIn[nScanTopY][X]==0				&&	ucImageIn[Y][X]==0				&& ucImageIn[nScanBottomY][X]==0 && 
					ucImageIn[nScanTopY][nScanRightX]==0	&&	ucImageIn[Y][nScanRightX]==0	&& ucImageIn[nScanBottomY][nScanRightX]==0)
				{
					ucTemp[Y][X]=0;
				}
				else{ ucTemp[Y][X]=255; }
			}
		}

		for (int Y=0; Y<nImageHeight; Y++)	//膨脹
		{
			for (int X=0; X<nImageWidth; X++)
			{
				nScanLeftX = X-1;
				nScanRightX = X+1;
				nScanTopY = Y-1;
				nScanBottomY = Y+1;

				if (nScanLeftX<0){ nScanLeftX=0; }
				if (nScanRightX>nImageWidth-1){ nScanRightX=nImageWidth-1; }
				if (nScanTopY<0){ nScanTopY=0; }
				if (nScanBottomY>nImageHeight-1){ nScanBottomY=nImageHeight-1; }

				if (ucTemp[nScanTopY][nScanLeftX]==0	|| ucTemp[Y][nScanLeftX]==0		|| ucTemp[nScanBottomY][nScanLeftX]==0 || 
					ucTemp[nScanTopY][X]==0				|| ucTemp[Y][X]==0				|| ucTemp[nScanBottomY][X]==0 || 
					ucTemp[nScanTopY][nScanRightX]==0	|| ucTemp[Y][nScanRightX]==0	|| ucTemp[nScanBottomY][nScanRightX]==0)
				{
					ucImageOut[Y][X]=0;
				}
				else{ ucImageOut[Y][X]=255; }
			}
		}
		return 0;
	}
	// 閉合
	int Closing(UCHAR **ucImageIn, UCHAR **ucImageOut, int nImageWidth, int nImageHeight)
	{
		int nScanLeftX=0, nScanRightX, nScanTopY, nScanBottomY;

		for (int Y=0; Y<nImageHeight; Y++)	//膨脹
		{
			for (int X=0; X<nImageWidth; X++)
			{
				nScanLeftX=X-1; nScanRightX=X+1; nScanTopY=Y-1; nScanBottomY=Y+1;

				if (nScanLeftX<0){ nScanLeftX=0; }
				if (nScanRightX>nImageWidth-1){ nScanRightX=nImageWidth-1; }
				if (nScanTopY<0){ nScanTopY=0; }
				if (nScanBottomY>nImageHeight-1){ nScanBottomY=nImageHeight-1; }

				if (ucImageIn[nScanTopY][nScanLeftX]==0 || ucImageIn[Y][nScanLeftX]==0 || ucImageIn[nScanBottomY][nScanLeftX]==0 || 
					ucImageIn[nScanTopY][X]==0 || ucImageIn[Y][X]==0 || ucImageIn[nScanBottomY][X]==0 || 
					ucImageIn[nScanTopY][nScanRightX]==0 || ucImageIn[Y][nScanRightX]==0 || ucImageIn[nScanBottomY][nScanRightX]==0)
				{
					ucTemp[Y][X]=0;
				}
				else{ ucTemp[Y][X]=255; }
			}
		}

		for (int Y=0; Y<nImageHeight; Y++)	//侵蝕
		{
			for (int X=0; X<nImageWidth; X++)
			{
				nScanLeftX=X-1; nScanRightX=X+1; nScanTopY=Y-1; nScanBottomY=Y+1;

				if (nScanLeftX<0){ nScanLeftX=0; }
				if (nScanRightX>nImageWidth-1){ nScanRightX=nImageWidth-1; }
				if (nScanTopY<0){ nScanTopY=0; }
				if (nScanBottomY>nImageHeight-1){ nScanBottomY=nImageHeight-1; }

				if (ucTemp[nScanTopY][nScanLeftX]==0 && ucTemp[Y][nScanLeftX]==0 && ucTemp[nScanBottomY][nScanLeftX]==0 && 
					ucTemp[nScanTopY][X]==0 && ucTemp[Y][X]==0 && ucTemp[nScanBottomY][X]==0 && 
					ucTemp[nScanTopY][nScanRightX]==0 && ucTemp[Y][nScanRightX]==0 && ucTemp[nScanBottomY][nScanRightX]==0)
				{
					ucImageOut[Y][X]=0;
				}
				else{ ucImageOut[Y][X]=255; }
			}
		}
		return 0;
	}

	// Sobel
	int Sobel(UCHAR **ucImageIn, UCHAR **ucImageOut, int nImageWidth, int nImageHeight)
	{
		int nScanLeftX=0, nScanRightX, nScanTopY, nScanBottomY;
		int i;

		for (int Y=0; Y<nImageHeight; Y++ )	
		{
			for (int X=0; X<nImageWidth; X++ )
			{
				nScanLeftX=X-1; nScanRightX=X+1; nScanTopY=Y-1; nScanBottomY=Y+1;
				if (nScanLeftX<0){ nScanLeftX=0; }
				if (nScanRightX>nImageWidth-1){ nScanRightX=nImageWidth-1; }
				if (nScanTopY<0){ nScanTopY=0; }
				if (nScanBottomY>nImageHeight-1){ nScanBottomY=nImageHeight-1; }

				i = abs(-ucImageIn[nScanTopY][nScanLeftX] - 2*ucImageIn[nScanTopY][X] - ucImageIn[nScanTopY][nScanRightX]
						+ ucImageIn[nScanBottomY][nScanLeftX] + 2*ucImageIn[nScanBottomY][X] + ucImageIn[nScanBottomY][nScanRightX])
					+abs(-ucImageIn[nScanTopY][nScanLeftX] - 2*ucImageIn[Y][nScanLeftX] - ucImageIn[nScanBottomY][nScanLeftX] + ucImageIn[nScanTopY][nScanRightX]
						+ 2*ucImageIn[Y][nScanRightX] + ucImageIn[nScanBottomY][nScanRightX]);

				if ( i>100 ){ ucImageOut[Y][X]=0; }
				else{ ucImageOut[Y][X]=255; }
			}
		}
		return 0;
	}

	// Laplace
	int Laplace(UCHAR **ucImageIn,UCHAR **ucImageOut, int nImageWidth,int nImageHeight)
	{
		int nScanLeftX=0, nScanRightX, nScanTopY, nScanBottomY;
		int i;

		for (int Y=0; Y<nImageHeight; Y++ )	
		{
			for (int X=0; X<nImageWidth; X++ )
			{
				nScanLeftX=X-1;	nScanRightX=X+1;	nScanTopY=Y-1;	nScanBottomY=Y+1;
				if (nScanLeftX<0){ nScanLeftX=0; }
				if (nScanRightX>nImageWidth-1){ nScanRightX=nImageWidth-1; }
				if (nScanTopY<0){ nScanTopY=0; }
				if (nScanBottomY>nImageHeight-1){ nScanBottomY=nImageHeight-1; }

				i = ucImageIn[nScanTopY][nScanLeftX] + ucImageIn[nScanTopY][X] + ucImageIn[nScanTopY][nScanRightX]
					+ ucImageIn[Y][nScanLeftX] + ucImageIn[Y][nScanRightX] + ucImageIn[nScanBottomY][nScanLeftX]
					+ ucImageIn[nScanBottomY][X] + ucImageIn[nScanBottomY][nScanRightX] - 8*ucImageIn[Y][X];

				if ( i>35 ){ ucImageOut[Y][X]=0; }
				else{ ucImageOut[Y][X]=255; }
			}
		}
		return 0;
	}
	#define SWAP(x,y) {int t; t = x; x = y; y = t;} 
	void bubbleSort(int number[])
	{ 
		int flag = 1;
		int i; 
		int MAX=9;
		for(i = 0; i < MAX-1 && flag == 1; i++) { 
			flag = 0; 
			int j;
			for(j = 0; j < MAX-i-1; j++) { 
				if(number[j+1] < number[j]) { 
					SWAP(number[j+1], number[j]); 
					flag = 1; 
				} 
			} 
		} 
	} 
	// 中值濾波
	int Mediam(UCHAR **ucImageIn, UCHAR **ucImageOut, int nImageWidth, int nImageHeight)
	{
		int nScanLeftX=0, nScanRightX, nScanTopY, nScanBottomY;
		int Array[9]={};
		int i;

		for (int Y=0; Y<nImageHeight; Y++ )	
		{
			for (int X=0; X<nImageWidth; X++ )
			{
				nScanLeftX=X-1; nScanRightX=X+1; nScanTopY=Y-1; nScanBottomY=Y+1;
				if (nScanLeftX<0){ nScanLeftX=0; }
				if (nScanRightX>nImageWidth-1){ nScanRightX=nImageWidth-1; }
				if (nScanTopY<0){ nScanTopY=0; }
				if (nScanBottomY>nImageHeight-1){ nScanBottomY=nImageHeight-1; }
				Array[0]=ucImageIn[nScanTopY][nScanLeftX];
				Array[1]=ucImageIn[nScanTopY][X];
				Array[2]=ucImageIn[nScanTopY][nScanRightX];
				Array[3]=ucImageIn[Y][nScanLeftX];
				Array[4]=ucImageIn[Y][X];
				Array[5]=ucImageIn[Y][nScanRightX];
				Array[6]=ucImageIn[nScanBottomY][nScanLeftX];
				Array[7]=ucImageIn[nScanBottomY][X];
				Array[8]=ucImageIn[nScanBottomY][nScanRightX];
				bubbleSort(Array);
				ucImageOut[Y][X]=Array[4];
			}
		}
		return 0;
	}
	// RCD偵測
	int RCD(UCHAR **ucImageIn, double Center_r[3], int nImageWidth, int nImageHeight,double Tr)
	{
		int a, b, c, d, i;
		int acc,accC,accF=0,V=0,Tf=8000,Tmin=30,random[4];	//球
		//double pick[4][2],D1,D2,D3,Ta=4,Tr=0.9,Td=1.3,den,Pi=3.14159,d4123;
		double pick[4][2],D1,D2,D3,Ta=4,Td=1.4,den,Pi=3.14159,d4123;

		V=0;
		for (int Y=0; Y<nImageHeight; Y++ )//跑一遍圖看有幾個黑
		{
			for (int X=0; X<nImageWidth; X++ )
			{
				if (ucImageIn[Y][X]<10)
				V=V+1;//V為圖片中黑點的數量
			}
		}
		int **BlackPixes;//宣告寬度為黑點數大小的矩陣;
		BlackPixes = (int**)malloc2d(V,2,sizeof(int));
    
		acc=-1;

		for (int Y=0; Y<nImageHeight; Y++ )//將圖片中的黑點的座標存至BlackPixes
		{
			for (int X=0; X<nImageWidth; X++ )
			{
				if (ucImageIn[Y][X]<10 )
				{
					acc=acc+1;
					BlackPixes[acc][0]=Y;
					BlackPixes[acc][1]=X;
				}
			}
		}

		//step2//         

		accF=0;
		while (accF<Tf && V>Tmin )//設置循環條件	還沒到容忍界線 && PIXEL數夠
		{
			//任意取不重複四點
			random[0]=rand()%V;                        //0~V-1 中間取一個
			pick[0][0]=BlackPixes[random[0]][0];       //V中隨機取	   Y
			pick[0][1]=BlackPixes[random[0]][1];					// x

			random[1]=rand()%V;                        //0~V-1 中間取一個
			pick[1][0]=BlackPixes[random[1]][0];       //V中隨機取
			pick[1][1]=BlackPixes[random[1]][1];

			random[2]=rand()%V;                        //0~V-1 中間取一個
			pick[2][0]=BlackPixes[random[2]][0];       //V中隨機取
			pick[2][1]=BlackPixes[random[2]][1];

			random[3]=rand()%V;                        //0~V-1 中間取一個
			pick[3][0]=BlackPixes[random[3]][0];       //V中隨機取
			pick[3][1]=BlackPixes[random[3]][1];


			//step3//
			i=0;
			while (i<4)									//四點四圓四次討論 有一個成立就跳脫此WHILE 四個都不成立就重新取四點
			{											
				if(i=0) { a=0; b=1; c=2; d=3;}			//123圓 4判斷; 134圓 2判斷; 124圓 3判斷; 234圓 1判斷
				if(i=1) { a=0; b=2; c=3; d=1;}
				if(i=2) { a=0; b=1; c=3; d=2;}
				if(i=3) { a=1; b=2; c=3; d=0;}

				D1=sqrt(pow((pick[a][0]-pick[b][0]),2)+pow((pick[a][1]-pick[b][1]),2));		//abc各點距離
				D2=sqrt(pow((pick[a][0]-pick[c][0]),2)+pow((pick[a][1]-pick[c][1]),2));
				D3=sqrt(pow((pick[b][0]-pick[c][0]),2)+pow((pick[b][1]-pick[c][1]),2));
    	
				if (  D1<=Ta || D2<=Ta || D3<=Ta )
				{	
					i++;
					continue;																//abc距離過近 i++ 換下個組合
				}

				den=4*((pick[b][1]-pick[a][1])*(pick[c][0]-pick[a][0])-(pick[c][1]-pick[a][1])*(pick[b][0]-pick[a][0]));
				if (den==0)
				{
					i++;
					continue;																//分母為0 同一直線上 i++ 換下個組合
				}

				Center_r[0]=((2*(pow(pick[b][1],2)+pow(pick[b][0],2)-pow(pick[a][1],2)-pow(pick[a][0],2))*(pick[c][0]-pick[a][0])-2*(pow(pick[c][1],2)+pow(pick[c][0],2)-pow(pick[a][1],2)-pow(pick[a][0],2))*(pick[b][0]-pick[a][0]))/den);

				Center_r[1]=((2*(pow(pick[c][1],2)+pow(pick[c][0],2)-pow(pick[a][1],2)-pow(pick[a][0],2))*(pick[b][1]-pick[a][1])-2*(pow(pick[b][1],2)+pow(pick[b][0],2)-pow(pick[a][1],2)-pow(pick[a][0],2))*(pick[c][1]-pick[a][1]))/den);

				Center_r[2]=(sqrt(pow((pick[a][1]-Center_r[0]),2)+pow((pick[a][0]-Center_r[1]),2)));			//克拉瑪 求圓心半徑
																											
				d4123=abs(sqrt(pow((pick[d][1]-Center_r[0]),2)+pow((pick[d][0]-Center_r[1]),2))-Center_r[2]);	//d點與圓abc之間的距離

				if (d4123<Td){ break; }

				i++;
			};

			if (i==4)														//四種狀況都不合理 重新取四點
			{         
				accF=accF+1;												//錯誤程度加一
				continue;													//回到步驟二
			}
			
			//step4//

			accC=0;
			for (int Y=0;Y<V;Y++ )           
			{
				d4123=sqrt(pow((BlackPixes[Y][1]-Center_r[0]),2)+pow((BlackPixes[Y][0]-Center_r[1]),2))-Center_r[2];

				if (abs(d4123)<Td){ accC=accC+1; }
			}

			//step5//

			if (accC<(2*Pi*Center_r[2]*Tr)){ accF=accF+1; }//錯誤程度加一
			else{ break; }
		};

		if (accF>=Tf || V<Tmin){ Center_r[0]=Center_r[1]=Center_r[2]=0; }//找不到 或PIXEL值過少 回傳 000

		return 0;
	}
};