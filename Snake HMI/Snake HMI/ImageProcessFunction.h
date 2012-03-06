#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#define M_PI		3.14159265358979323846

using namespace std;

class ImageProcessFunction
{
protected:
	UCHAR **ucTemp;

public:
	ImageProcessFunction(int nImageWidth, int nImageHeight)
	{
		ucTemp = (UCHAR**)malloc2d(nImageWidth, nImageHeight, sizeof(UCHAR));
	}

	~ImageProcessFunction()
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


	int ReadBMP(UCHAR ***ucImageOut, int nImageWidth, int nImageHeight, char *fileName)//讀圖
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

				if (ucImageIn[nScanLeftX][nScanTopY]==0 || ucImageIn[nScanLeftX][Y]==0 || ucImageIn[nScanLeftX][nScanBottomY]==0 || 
					ucImageIn[X][nScanTopY]==0 || ucImageIn[X][Y]==0 || ucImageIn[X][nScanBottomY]==0 || 
					ucImageIn[nScanRightX][nScanTopY]==0 || ucImageIn[nScanRightX][Y]==0 || ucImageIn[nScanRightX][nScanBottomY]==0)
				{
					ucImageOut[X][Y]=0;
				}
				else{ ucImageOut[X][Y]=255; }
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

				if (ucImageIn[nScanLeftX][nScanTopY]==0		|| ucImageIn[nScanLeftX][Y]==0		|| ucImageIn[nScanLeftX][nScanBottomY]==0 || 
					ucImageIn[X][nScanTopY]==0				|| ucImageIn[X][Y]==0				|| ucImageIn[X][nScanBottomY]==0 || 
					ucImageIn[nScanRightX][nScanTopY]==0	|| ucImageIn[nScanRightX][Y]==0		|| ucImageIn[nScanRightX][nScanBottomY]==0)
				{
					ucImageOut[X][Y]=0;
				}
				else{ ucImageOut[X][Y]=255; }
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

				if (ucImageIn[nScanLeftX][nScanTopY]==0		&&	ucImageIn[nScanLeftX][Y]==0		&& ucImageIn[nScanLeftX][nScanBottomY]==0 && 
					ucImageIn[X][nScanTopY]==0				&&	ucImageIn[X][Y]==0				&& ucImageIn[X][nScanBottomY]==0 && 
					ucImageIn[nScanRightX][nScanTopY]==0	&&	ucImageIn[nScanRightX][Y]==0	&& ucImageIn[nScanRightX][nScanBottomY]==0)
				{
					ucTemp[X][Y]=0;
				}
				else{ ucTemp[X][Y]=255; }
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

				if (ucTemp[nScanLeftX][nScanTopY]==0	|| ucTemp[nScanLeftX][Y]==0		|| ucTemp[nScanLeftX][nScanBottomY]==0 || 
					ucTemp[X][nScanTopY]==0				|| ucTemp[X][Y]==0				|| ucTemp[X][nScanBottomY]==0 || 
					ucTemp[nScanRightX][nScanTopY]==0	|| ucTemp[nScanRightX][Y]==0	|| ucTemp[nScanRightX][nScanBottomY]==0)
				{
					ucImageOut[X][Y]=0;
				}
				else{ ucImageOut[X][Y]=255; }
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

				if (ucImageIn[nScanLeftX][nScanTopY]==0 || ucImageIn[nScanLeftX][Y]==0 || ucImageIn[nScanLeftX][nScanBottomY]==0 || 
					ucImageIn[X][nScanTopY]==0 || ucImageIn[X][Y]==0 || ucImageIn[X][nScanBottomY]==0 || 
					ucImageIn[nScanRightX][nScanTopY]==0 || ucImageIn[nScanRightX][Y]==0 || ucImageIn[nScanRightX][nScanBottomY]==0)
				{
					ucTemp[X][Y]=0;
				}
				else{ ucTemp[X][Y]=255; }
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

				if (ucTemp[nScanLeftX][nScanTopY]==0 && ucTemp[nScanLeftX][Y]==0 && ucTemp[nScanLeftX][nScanBottomY]==0 && 
					ucTemp[X][nScanTopY]==0 && ucTemp[X][Y]==0 && ucTemp[X][nScanBottomY]==0 && 
					ucTemp[nScanRightX][nScanTopY]==0 && ucTemp[nScanRightX][Y]==0 && ucTemp[nScanRightX][nScanBottomY]==0)
				{
					ucImageOut[X][Y]=0;
				}
				else{ ucImageOut[X][Y]=255; }
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

				i = abs(-ucImageIn[nScanLeftX][nScanTopY] - 2*ucImageIn[X][nScanTopY] - ucImageIn[nScanRightX][nScanTopY]
						+ ucImageIn[nScanLeftX][nScanBottomY] + 2*ucImageIn[X][nScanBottomY] + ucImageIn[nScanRightX][nScanBottomY])
					+abs(-ucImageIn[nScanLeftX][nScanTopY] - 2*ucImageIn[nScanLeftX][Y] - ucImageIn[nScanLeftX][nScanBottomY] + ucImageIn[nScanRightX][nScanTopY]
						+ 2*ucImageIn[nScanRightX][Y] + ucImageIn[nScanRightX][nScanBottomY]);

				if ( i>100 ){ ucImageOut[X][Y]=0; }
				else{ ucImageOut[X][Y]=255; }
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

				i = ucImageIn[nScanLeftX][nScanTopY] + ucImageIn[X][nScanTopY] + ucImageIn[nScanRightX][nScanTopY]
					+ ucImageIn[nScanLeftX][Y] + ucImageIn[nScanRightX][Y] + ucImageIn[nScanLeftX][nScanBottomY]
					+ ucImageIn[X][nScanBottomY] + ucImageIn[nScanRightX][nScanBottomY] - 8*ucImageIn[X][Y];

				if ( i>35 ){ ucImageOut[X][Y]=0; }
				else{ ucImageOut[X][Y]=255; }
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
				Array[0]=ucImageIn[nScanLeftX][nScanTopY];
				Array[1]=ucImageIn[X][nScanTopY];
				Array[2]=ucImageIn[nScanRightX][nScanTopY];
				Array[3]=ucImageIn[nScanLeftX][Y];
				Array[4]=ucImageIn[X][Y];
				Array[5]=ucImageIn[nScanRightX][Y];
				Array[6]=ucImageIn[nScanLeftX][nScanBottomY];
				Array[7]=ucImageIn[X][nScanBottomY];
				Array[8]=ucImageIn[nScanRightX][nScanBottomY];
				bubbleSort(Array);
				ucImageOut[X][Y]=Array[4];
			}
		}
		return 0;
	}


	void Histogram(UINT ImageWidth,UINT ImageHeight,UCHAR **GrayData,UCHAR *HistogramArray)
	{
		//COLORREF color;
		//CClientDC thisDC(this);
		int max=0;
		for (int Y_Axis=1; Y_Axis<ImageWidth-1; Y_Axis++)// 取得直方統計資料
		{
			for (int X_Axis=1; X_Axis<ImageWidth-1; X_Axis++)
			{
				HistogramArray[GrayData[Y_Axis][X_Axis]]++;
			}
		}
	}
	void Normalization(UINT ImageWidth,UINT ImageHeight,UCHAR **GrayData,UCHAR *HistogramArray,double *Probability)
	{
		
		//計算機率
		for(int i=0;i<256;i++)
		{
			Probability[i]=((double)HistogramArray[i]/(double)(ImageHeight*ImageWidth));
		}
		//計算累加機率
		for(int i=0;i<255;i++)
		{
			Probability[i+1]=Probability[i]+Probability[i+1];
		}
	}

	//觀察法(輸入伐值二值化)
	void Observation(int Threshold,UINT ImageWidth,UINT ImageHeight,UCHAR **GrayData,UCHAR **Binarization)
	{
		for(int Y=0;Y<ImageHeight;Y++)
		{
			for(int X=0;X<ImageWidth;X++)
			{
				if(GrayData[X][Y]<=Threshold)
				{
					Binarization[X][Y]=0;
				}
				else
				{
					Binarization[X][Y]=255;
				}
			}
		}

	} 
	//歐蘇法(回傳伐值)
	int Otsu(UINT ImageWidth,UINT ImageHeight,UCHAR **GrayData,double *Probability)
	{
		double max=0,p1k=0,mk=0,mg=0,sigma=0;
		int Threshold=0,HistogramArray[256]={0};
		for(int Y=0;Y<ImageHeight;Y++)
		{
			for(int X=0;X<ImageWidth;X++)
			{
				HistogramArray[GrayData[X][Y]]++;
			}
		}
		for(int i=0;i<256;i++)
		{
			Probability[i]=((double)HistogramArray[i]/(double)(ImageHeight*ImageWidth));
		}
		for(int k=0;k<256;k++)
		{
			p1k=0;
			mk=0;
			mg=0;
			for (int i=0;i<k;i++)
			{
				p1k=p1k+Probability[i];
			}
			for (int i=0;i<k;i++)
			{
				mk=mk+(double)i*Probability[i];
			}
			for (int i=0;i<256;i++)
			{
				mg=mg+(double)i*Probability[i];
			}
			if (p1k==0||p1k==1)
				sigma=0;
			else
				sigma=((pow((mg*p1k-mk),2.0))/(p1k*(1.0-p1k)));

			if (sigma>max)
			{
				max=sigma;
				Threshold=k;
			}
		}
		return Threshold;

	} 
};