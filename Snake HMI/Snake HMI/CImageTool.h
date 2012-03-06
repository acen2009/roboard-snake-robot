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
	//�ʺA�x�}�ܼƫŧi
	void* malloc2d( int w, int h, int size )
	{
		void **a = (void**) malloc( w*sizeof(void*) + w*h*size );
		for(int j=0; j<w; j++ )
			a[j] = ((char *)(a+w)) + j*h*size;
		return a;
	}

	// Ū��
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

	// ��RGB���oYCbCr��Y��
	double GetValueY(UCHAR R, UCHAR G, UCHAR B)
	{
		double ValueY = (0.299)*R + (0.587)*G + (0.114)*B;
		return ValueY;
	}

	// ��RGB���oYCbCr��Cb��
	double GetValueCb(UCHAR R, UCHAR G, UCHAR B)
	{
		double ValueCb = (-0.169)*R - (0.331)*G + (0.499)*B;
		return ValueCb;
	}

	// ��RGB���oYCbCr��Cr��
	double GetValueCr(UCHAR R, UCHAR G, UCHAR B)
	{
		double ValueCr = (0.499)*R + (-0.418)*G + (-0.0813)*B;
		return ValueCr;
	}

	// ��RGB���oYCbCr��Cr��
	double GetVectorCbCr(double Cb, double Cr)
	{
		double VectorCbCr = sqrt( pow(Cb, 2) + pow(Cr, 2) );
		return VectorCbCr;
	}

	// ���oCbCr��������
	double GetCbCrAngle(double Cb, double Cr)
	{
		double fThida;

		if (Cb>0 && Cr>0)// �Ĥ@�H��
		{
			fThida = 180*atan(Cr/Cb)/M_PI;
		}
		else if (Cb<0 && Cr>0)// �ĤG�H��
		{
			fThida = 180 + 180*atan(Cr/Cb)/M_PI;
		}
		else if (Cb<0 && Cr<0)// �ĤT�H��
		{
			fThida = 180 + 180*atan(Cr/Cb)/M_PI;
		}
		else if (Cb>0 && Cr<0)// �ĥ|�H��
		{
			fThida = 2*180 + 180*atan(Cr/Cb)/M_PI;
		}
		else if (Cb>0 && Cr==0)// �����s�ר�
		{
			fThida = 0;
		}
		else if (Cb<0 && Cr==0)// ����180�ר�
		{
			fThida = 180;
		}
		else if (Cb==0 && Cr>0)// ����90��
		{
			fThida = 90;
		}
		else if (Cb==0 && Cr<0)// ����270��
		{
			fThida = 270;
		}
		else if (Cb==0 && Cr==0)// ���I
		{
			return -1;
		}

		return fThida;
	}

	// ����
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

	// �I�k
	int Erosion(UCHAR **ucImageIn, UCHAR **ucImageOut, int nImageWidth, int nImageHeight)
	{
		int nScanLeftX=0, nScanRightX, nScanTopY, nScanBottomY;

		for (int Y=0; Y<nImageHeight; Y++)	//����
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
	// �_�}
	int Opening(UCHAR **ucImageIn, UCHAR **ucImageOut, int nImageWidth, int nImageHeight)
	{
		int nScanLeftX, nScanRightX, nScanTopY, nScanBottomY;

		for (int Y=0; Y<nImageHeight; Y++)	//�I�k
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

		for (int Y=0; Y<nImageHeight; Y++)	//����
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
	// ���X
	int Closing(UCHAR **ucImageIn, UCHAR **ucImageOut, int nImageWidth, int nImageHeight)
	{
		int nScanLeftX=0, nScanRightX, nScanTopY, nScanBottomY;

		for (int Y=0; Y<nImageHeight; Y++)	//����
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

		for (int Y=0; Y<nImageHeight; Y++)	//�I�k
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
	// �����o�i
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
	// RCD����
	int RCD(UCHAR **ucImageIn, double Center_r[3], int nImageWidth, int nImageHeight,double Tr)
	{
		int a, b, c, d, i;
		int acc,accC,accF=0,V=0,Tf=8000,Tmin=30,random[4];	//�y
		//double pick[4][2],D1,D2,D3,Ta=4,Tr=0.9,Td=1.3,den,Pi=3.14159,d4123;
		double pick[4][2],D1,D2,D3,Ta=4,Td=1.4,den,Pi=3.14159,d4123;

		V=0;
		for (int Y=0; Y<nImageHeight; Y++ )//�]�@�M�Ϭݦ��X�Ӷ�
		{
			for (int X=0; X<nImageWidth; X++ )
			{
				if (ucImageIn[Y][X]<10)
				V=V+1;//V���Ϥ������I���ƶq
			}
		}
		int **BlackPixes;//�ŧi�e�׬����I�Ƥj�p���x�};
		BlackPixes = (int**)malloc2d(V,2,sizeof(int));
    
		acc=-1;

		for (int Y=0; Y<nImageHeight; Y++ )//�N�Ϥ��������I���y�Цs��BlackPixes
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
		while (accF<Tf && V>Tmin )//�]�m�`������	�٨S��e�Ԭɽu && PIXEL�ư�
		{
			//���N�������ƥ|�I
			random[0]=rand()%V;                        //0~V-1 �������@��
			pick[0][0]=BlackPixes[random[0]][0];       //V���H����	   Y
			pick[0][1]=BlackPixes[random[0]][1];					// x

			random[1]=rand()%V;                        //0~V-1 �������@��
			pick[1][0]=BlackPixes[random[1]][0];       //V���H����
			pick[1][1]=BlackPixes[random[1]][1];

			random[2]=rand()%V;                        //0~V-1 �������@��
			pick[2][0]=BlackPixes[random[2]][0];       //V���H����
			pick[2][1]=BlackPixes[random[2]][1];

			random[3]=rand()%V;                        //0~V-1 �������@��
			pick[3][0]=BlackPixes[random[3]][0];       //V���H����
			pick[3][1]=BlackPixes[random[3]][1];


			//step3//
			i=0;
			while (i<4)									//�|�I�|��|���Q�� ���@�Ӧ��ߴN���榹WHILE �|�ӳ������ߴN���s���|�I
			{											
				if(i=0) { a=0; b=1; c=2; d=3;}			//123�� 4�P�_; 134�� 2�P�_; 124�� 3�P�_; 234�� 1�P�_
				if(i=1) { a=0; b=2; c=3; d=1;}
				if(i=2) { a=0; b=1; c=3; d=2;}
				if(i=3) { a=1; b=2; c=3; d=0;}

				D1=sqrt(pow((pick[a][0]-pick[b][0]),2)+pow((pick[a][1]-pick[b][1]),2));		//abc�U�I�Z��
				D2=sqrt(pow((pick[a][0]-pick[c][0]),2)+pow((pick[a][1]-pick[c][1]),2));
				D3=sqrt(pow((pick[b][0]-pick[c][0]),2)+pow((pick[b][1]-pick[c][1]),2));
    	
				if (  D1<=Ta || D2<=Ta || D3<=Ta )
				{	
					i++;
					continue;																//abc�Z���L�� i++ ���U�ӲզX
				}

				den=4*((pick[b][1]-pick[a][1])*(pick[c][0]-pick[a][0])-(pick[c][1]-pick[a][1])*(pick[b][0]-pick[a][0]));
				if (den==0)
				{
					i++;
					continue;																//������0 �P�@���u�W i++ ���U�ӲզX
				}

				Center_r[0]=((2*(pow(pick[b][1],2)+pow(pick[b][0],2)-pow(pick[a][1],2)-pow(pick[a][0],2))*(pick[c][0]-pick[a][0])-2*(pow(pick[c][1],2)+pow(pick[c][0],2)-pow(pick[a][1],2)-pow(pick[a][0],2))*(pick[b][0]-pick[a][0]))/den);

				Center_r[1]=((2*(pow(pick[c][1],2)+pow(pick[c][0],2)-pow(pick[a][1],2)-pow(pick[a][0],2))*(pick[b][1]-pick[a][1])-2*(pow(pick[b][1],2)+pow(pick[b][0],2)-pow(pick[a][1],2)-pow(pick[a][0],2))*(pick[c][1]-pick[a][1]))/den);

				Center_r[2]=(sqrt(pow((pick[a][1]-Center_r[0]),2)+pow((pick[a][0]-Center_r[1]),2)));			//�J�Ժ� �D��ߥb�|
																											
				d4123=abs(sqrt(pow((pick[d][1]-Center_r[0]),2)+pow((pick[d][0]-Center_r[1]),2))-Center_r[2]);	//d�I�P��abc�������Z��

				if (d4123<Td){ break; }

				i++;
			};

			if (i==4)														//�|�ت��p�����X�z ���s���|�I
			{         
				accF=accF+1;												//���~�{�ץ[�@
				continue;													//�^��B�J�G
			}
			
			//step4//

			accC=0;
			for (int Y=0;Y<V;Y++ )           
			{
				d4123=sqrt(pow((BlackPixes[Y][1]-Center_r[0]),2)+pow((BlackPixes[Y][0]-Center_r[1]),2))-Center_r[2];

				if (abs(d4123)<Td){ accC=accC+1; }
			}

			//step5//

			if (accC<(2*Pi*Center_r[2]*Tr)){ accF=accF+1; }//���~�{�ץ[�@
			else{ break; }
		};

		if (accF>=Tf || V<Tmin){ Center_r[0]=Center_r[1]=Center_r[2]=0; }//�䤣�� ��PIXEL�ȹL�� �^�� 000

		return 0;
	}
};