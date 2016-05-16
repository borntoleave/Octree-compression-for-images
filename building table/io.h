//Indexed Color Using Octree 
//Rongzhong Li
//March 8
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cmath>
using namespace std;

//----------function declare-------------------
int * d2b(int dec,int b);//decimal to binary
void printbin(int *bin,int b);//print out binary array
int b2d(int r,int g,int b);//translate the 3 binary digits RGB to decimal
int *idx2clr(int *index);
void ary3D_mal_(int ***&clr,int L,int C,int D);
void ary3D_del(int ***&clr,int L,int C);
void ary2D_mal(int **&ary,int L,int C);//malloc memory for raw RGB color array
void ary2D_del(int **&ary,int L);//clean up raw RGB color matrix
//for each raw pixel, we have an array for RGB and an array for color node index
int raw_ini(char *buff,int **raw,int ***clr,int L,int C);//initialize memory for raw RGB color array
void printRGB(int ***raw,int H,int W,int C);//print raw R,G,B color matrix in decimal 
void bmp_wrt(std::ofstream &f,int *newC,int **colortable,int W,int H);
void raw_wrt(std::ofstream &rawout,int *newC,int **colortable,int L);


int * d2b(int dec,int b)//decimal to binary array
{
int n=0,*temp=new int [b],*bin=new int [b];
while (dec>0)
{temp[n++]=dec%2;dec/=2;}
while (n>0)bin[b-n]=temp[--n];
return bin;
}

void printbin(int *bin,int b)//print binary number
{
for(int n=0;n<b;n++)
cout <<bin[n];
cout <<endl;
}

int b2idx(int r,int g,int b)//calculate the index with RGB digits 
{
return r*4+g*2+b;
}

int b2d(int *bin)//binary array to decimal
{int d=0;
for(int b=0;b<8;b++)
d+=bin[b]*pow(2,7-b);
return d;
}

int *idx2clr(int *index)//given indexes(node position), calculate RGB color
{
int *bin=new int[3];int *clr=new int[3];
for(int i=0;i<8;i++)
	{
	bin=d2b(index[i],3);
	for(int c=0;c<3;c++)
	clr[c]+=bin[c]*pow(2,7-i);
	}
return clr;
}

void gen_stack(int **&stack)
{
stack=new int *[1000];for (int i=0;i<1000;i++)stack[i]=new int [8];
}

void ary3D_mal(int ***&clr,int L,int C,int D)//create 3D array
{
clr=new int **[L];
for(int l=0;l<L;l++)
	{clr[l]=new int *[C];
	for(int c=0;c<C;c++)
		clr[l][c]=new int [D];}
}
void ary2D_mal(int **&ary,int L,int C)
{
ary=new int *[L];
for(int l=0;l<L;l++)
	ary[l]=new int [C];
}

void ary3D_del(int ***&clr,int L,int C)//delete the memory for 3D array
{
for(int l=0;l<L;l++)
	{for(int c=0;c<C;c++)
		delete [] clr[l][c];
		delete [] clr[l];}
delete [] clr;
}

void ary2D_del(int **&ary,int L)
{
for(int l=0;l<L;l++)
	delete [] ary[l];
delete [] ary;
}

int raw_ini(char * buff,int **raw,int ***clr,int L,int C)//initialize the pixels' colors
{
for(int l=0;l<L;l++)
		{for(int c=0;c<C;c++)
			{
			raw[l][c]=static_cast<unsigned char>(buff[l*C+c]);
			clr[l][c]=d2b(raw[l][c],8);//printbin(bin[c],8);
			}
		}
}

void printRGB(int **raw,int L,int C)//show the raw file in RGB manner
{for(int l=0;l<L;l++)
		{for(int c=0;c<C;c++)
			{cout<<raw[l][c];cout<<',';}
		cout<<'\t';}
}

void assign_idx(int *newC,int **raw,int L,int **colortable)//asssign idexed colors to original image
{
int dif,min;
for(int l=0;l<L;l++)
{min=pow(256,2)*3;
for(int idx=0;idx<256;idx++)
	{dif=0;
	for(int c=0;c<3;c++)
	dif+=pow((raw[l][c]-colortable[idx][c]),2);
	if(dif<min)		//find the nearest color in the table
	{newC[l]=idx;min=dif;}
	//cout <<idx<<"\t";
	}
}
cout <<endl;
}
//--------------------write raw file------------------------
void raw_wrt(std::ofstream &rawout,int *newC,int **colortable,int L)
{
for(int l=0;l<L;l++)
{
unsigned char r=colortable[newC[l]][0];//int ri=r;cout <<ri<<",";
unsigned char g=colortable[newC[l]][1];//int gi=g;cout <<gi<<",";
unsigned char b=colortable[newC[l]][2];//int bi=b;cout <<bi<<"\t";
rawout.write(reinterpret_cast<char *>(&r),1);
rawout.write(reinterpret_cast<char *>(&g),1);
rawout.write(reinterpret_cast<char *>(&b),1);
}


}


//---------------------write bmp file----------------------------
void bmp_wrt(std::ofstream &f,int *newC,int **colortable,int W,int H)
{
unsigned char *img = NULL;
int filesize = 54 + 3*W*H,r,g,b;  //W is your image width, H is image height, both int
if( img )
    free( img );
img = (unsigned char *)malloc(3*W*H);
memset(img,0,sizeof(img));
for(int h=0; h<H; h++)
{
    for(int w=0; w<W; w++)
{ int j=H-1-h;
    r = colortable[newC[h*W+w]][0];
    g = colortable[newC[h*W+w]][1];
    b = colortable[newC[h*W+w]][2];
    if (r > 255) r=255;
    if (g > 255) g=255;
    if (b > 255) b=255;
    img[(w+j*W)*3+2] = (unsigned char)(r);
    img[(w+j*W)*3+1] = (unsigned char)(g);
    img[(w+j*W)*3+0] = (unsigned char)(b);
}
}

unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
unsigned char bmppad[3] = {0,0,0};

bmpfileheader[ 2] = (unsigned char)(filesize    );
bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
bmpfileheader[ 4] = (unsigned char)(filesize>>16);
bmpfileheader[ 5] = (unsigned char)(filesize>>24);

bmpinfoheader[ 4] = (unsigned char)(       W    );
bmpinfoheader[ 5] = (unsigned char)(       W>> 8);
bmpinfoheader[ 6] = (unsigned char)(       W>>16);
bmpinfoheader[ 7] = (unsigned char)(       W>>24);
bmpinfoheader[ 8] = (unsigned char)(       H    );
bmpinfoheader[ 9] = (unsigned char)(       H>> 8);
bmpinfoheader[10] = (unsigned char)(       H>>16);
bmpinfoheader[11] = (unsigned char)(       H>>24);

f.write(reinterpret_cast<char *>(bmpfileheader),14);
f.write(reinterpret_cast<char *>(bmpinfoheader),40);
for(int h=H-1; h>=0; h--)
{
    f.write(reinterpret_cast<char *>(img+(W*(H-h-1)*3)),3*W);
    f.write(reinterpret_cast<char *>(bmppad),(4-(W*3)%4)%4);
}

}

int s=0;
int *idxstack=new int [8];//store the path during traversal


