
void* malloc2d( int w, int h, int size )
{
	void **a = (void**) malloc( w*sizeof(void*) + w*h*size );
	for(int j=0; j<w; j++ )
		a[j] = ((char *)(a+w)) + j*h*size;
	return a;
}

class Array2DforInt
{
public:
	int w, h;
	int **Data;
	Array2DforInt( int width, int height )
	{
		w = width;
		h = height;
		Data = (int**)malloc2d(w,h,sizeof(int));
	}
	~Array2DforInt(){free(Data);}
};
class Array2DforUINT
{
public:
	int w, h;
	unsigned int **Data;
	Array2DforUINT( int width, int height )
	{
		w = width;
		h = height;
		Data = (unsigned int**)malloc2d(w,h,sizeof(unsigned int));
	}
	~Array2DforUINT(){free(Data);}
};
class Array2DforChar
{
public:
	int w, h;
	char **Data;
	Array2DforChar( int width, int height )
	{
		w = width;
		h = height;
		Data = (char**)malloc2d(w,h,sizeof(char));
	}
	~Array2DforChar(){free(Data);}
};
class Array2DforUCHAR
{
public:
	int w, h;
	unsigned char **Data;
	Array2DforUCHAR( int width, int height )
	{
		w = width;
		h = height;
		Data = (unsigned char**)malloc2d(w,h,sizeof(unsigned char));
	}
	~Array2DforUCHAR(){free(Data);}
};


class Array2DforDouble
{
public:
	int w, h;
	double **Data;
	Array2DforDouble( int width, int height )
	{
		w = width;
		h = height;
		Data = (double**)malloc2d(w,h,sizeof(double));
	}
	~Array2DforDouble(){free(Data);}
};

void* malloc3d( int d, int w, int h, int size )
{
	void ***a = (void***) malloc( d*sizeof(void**) + w*d*sizeof(void*) + w*h*d*size );

	for(int k=0; k<d; k++ )
		a[k] = ((void**)(a+d)) + k*w;

	for(int k=0; k<d; k++ )
		for(int j=0; j<w; j++ )
			a[k][j] = ((char*)(a+d+w*d)) + (k*w+j)*h*size;

	return a;
}



class Array3DforInt
{
public:
	int w, h, d;
	int ***Data;
	Array3DforInt( int depth, int width, int height )
	{
		w = width;
		h = height;
		d = depth;
		Data = (int***)malloc3d(d,w,h,sizeof(int));
	}
	~Array3DforInt(){free(Data);}
};
class Array3DforUINT
{
public:
	int w, h, d;
	unsigned int ***Data;
	Array3DforUINT( int depth, int width, int height )
	{
		w = width;
		h = height;
		d = depth;
		Data = (unsigned int***)malloc3d(d,w,h,sizeof(unsigned int));
	}
	~Array3DforUINT(){free(Data);}
};
class Array3DforShort
{
public:
	int w, h, d;
	short ***Data;
	Array3DforShort( int depth, int width, int height )
	{
		w = width;
		h = height;
		d = depth;
		Data = (short***)malloc3d(d,w,h,sizeof(short));
	}
	~Array3DforShort(){free(Data);}
};

class Array3DforChar
{
public:
	int w, h, d;
	char ***Data;
	Array3DforChar( int depth, int width, int height )
	{
		w = width;
		h = height;
		d = depth;
		Data = (char***)malloc3d(d,w,h,sizeof(char));
	}
	~Array3DforChar(){free(Data);}
};
class Array3DforUCHAR
{
public:
	int w, h, d;
	unsigned char ***Data;
	Array3DforUCHAR( int depth, int width, int height )
	{
		w = width;
		h = height;
		d = depth;
		Data = (unsigned char***)malloc3d(d,w,h,sizeof(unsigned char));
	}
	~Array3DforUCHAR(){free(Data);}
};
class Array3DforFloat
{
public:
	int w, h, d;
	float ***Data;
	Array3DforFloat( int depth, int width, int height )
	{
		w = width;
		h = height;
		d = depth;
		Data = (float***)malloc3d(d,w,h,sizeof(float));
	}
	~Array3DforFloat(){free(Data);}
};
class Array3DforDouble
{
public:
	int w, h, d;
	double ***Data;
	Array3DforDouble( int depth, int width, int height )
	{
		w = width;
		h = height;
		d = depth;
		Data = (double***)malloc3d(d,w,h,sizeof(double));
	}
	~Array3DforDouble(){free(Data);}
};