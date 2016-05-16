//Indexed Color Using Octree
//Rongzhong Li
//March 8
//

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include "octree.h"
using namespace std;

//-----------main--------------------
int main(int argc, char *argv[])
{
    int W,H,C,L;//width, height,channel,length
    if(argc==4)C=3;else if(argc==5)C=atoi(argv[4]);else {cerr<<"Format:\t"<<argv[0]<<" <filename> <width> <height> <channel>"<<endl;exit(-1);}//3 channels by default
    ifstream infile(argv[1],ios::in|ios::binary);
    ofstream rawout("./idxC.raw",ios::out|ios::binary);
    ofstream bmpout("./idxC.bmp",ios::out|ios::binary);
    W=atoi(argv[2]);H=atoi(argv[3]);L=W*H;
    unsigned char ch;
    char *buff;buff= new char [L*C];
    infile.read(buff,L*C);		//read binary data to buffer
    //------------malloc------------
    int ***clr;//create arrays for color(combined binary index), raw color and new color.
    int **raw;
    int **temp;
    ary3D_mal(clr,L,C,8);
    ary2D_mal(raw,L,C);
    ary2D_mal(temp,3,8);
    int channel_tmp[3];
    int ** colortable=new int *[256];			//created for 256 color table
    for (int idx=0;idx<256;idx++)colortable[idx] =new int [3];
    //------------initialize-----------
    raw_ini(buff,raw,clr,L,C);		//initialize the arrays with color data
    //print
    //printRGB(raw,L,C);			//print color in the R,G,B manner
    octree T(8);				//build tree root, 8 digit color limit
    for(int l=0;l<L;l++)
    {T.lowest=new treenode(0);
        T.add(T.root,clr[l],1);	//add pixels to the tree
        cout <<l+1<<" th pixel\t"<<T.clr_cnt<<"colors\t";
        if(T.clr_cnt>T.limit)
        {
            T.find();	//find the lowest node that can be combined
            cout<<"del "<<T.lowest->childcount<<"children at level_"<<T.lowest->level<<"\t";
            for(int c=0;c<3;c++)channel_tmp[c]=0;
            for(int c=0;c<3;c++){
                channel_tmp[c]=round(1.0*T.lowest->total_channel[c]/T.lowest->numvisits);
                temp[c]=d2b(channel_tmp[c],8);//calculate the average color
                cout <<channel_tmp[c]<<",";
            }
            T.del(T.lowest);T.clr_cnt-=T.lowest->childcount-1;//delete the children nodes
            T.add(T.root,temp,T.lowest->numvisits);//add the averaged color
        }
        
        cout <<endl;
    }
    T.clr_cnt=0;
    cout <<"Colortable:"<<endl;	//check the number of remaining nodes
    T.traverse(0,T.root,8,idxstack);	//traverse the tree to get the color table
    //----------------------write file----------------------------------------
    int *newC=new int [L];
    assign_idx(newC,raw,L,T.colortable);		//assign new colors to the original image
    cout<<endl<<T.clr_cnt<<" colors"<<endl;
    raw_wrt(rawout,newC,T.colortable,L);		//write to raw file
    bmp_wrt(bmpout,newC,T.colortable,W,H);		//write to bmp file
    
    cout <<"The new images are idxC.raw and idxC.bmp."<<endl<<endl;
    
    ary2D_del(raw,L);			//delete large arrays
    ary3D_del(clr,L,C);
    ary2D_del(temp,3);
}


