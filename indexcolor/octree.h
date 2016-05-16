//Indexed Color Using Octree
//Rongzhong Li
//March 8
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "io.h"
using namespace std;

class treenode
{
public:
    treenode(int l)
    {level=l;numvisits=0;childcount=0;
        for(int c=0;c<3;c++)total_channel[c]=0;
        for(int ch=0;ch<8;ch++)child[ch]=(treenode *)NULL;}
    ~treenode(){}
    //private://make public for convenience
    int level;//0 for root and 1~8 for nodes. the 8th is leaf.
    int childcount;
    int numvisits;
    int total_channel[3];//total of Red, total of Green, total of Blue,
    treenode *child[8];
    friend class octree;
};

class octree
{
public:
    octree(int clr_depth)
    {root=new treenode(0);limit=pow(2,clr_depth);clr_cnt=0;
        colortable=new int *[limit];
        for(int i=0;i<limit;i++)
        {colortable[i]=new int [3];
            for(int c=0;c<3;c++)colortable[i][c]=0;
        }
    }
    void add(treenode *root,int **clr,int visits);//traversely add new color to the tree
    void find();
    void del(treenode * node);
    void traverse(int token,treenode *node,int lt,int *idxstack);//do different operations according to the token
    
    //private://--------------------------------------
    treenode * root;
    int limit;//allow 2^clr_depth colors in the color map
    int clr_cnt;//color count in the color table
    int ** colortable;
    treenode *lowest;//lowest node that can be deleted
};

void octree::add(treenode *current,int **clr,int visits)
{
    if(current->level > lowest->level)
    {current->numvisits+=visits;
        for(int c=0;c<3;c++)current->total_channel[c]+=b2d(clr[c])*visits;}//sum the value of one channel
    if(current->level>7){if (current->numvisits==1)clr_cnt++;return;}//at the leaf
    int *channel=new int [3];int idx;//find or create child
    idx=b2idx(clr[0][current->level],clr[1][current->level],clr[2][current->level]);
    if(current->child[idx]==(treenode *)NULL)
    {current->child[idx]=new treenode(current->level+1);
    }
    int count=0;
    for (int ch=0;ch<8;ch++)if(current->child[ch]!=(treenode *)NULL)count++;//count children number
    current->childcount=count;//cout<< current->childcount<<" ";
    
    current=current->child[idx];
    add(current,clr,visits);
}

void octree::find()//find the lowest node that can be deleted
{
    traverse(1,root,8,idxstack);
}

void octree::del(treenode * node)
{
    for(int ch=0;ch<8;ch++)
        if(node->child[ch]!=(treenode *)NULL)
        {delete node->child[ch];node->child[ch]=(treenode *)NULL;
        }
}

void octree::traverse(int token,treenode *current,int lt,int *idxstack)
{
    if(current->level==lt)
    {
        switch(token)
        {
            case 0://traverse the tree to fill the colortable
            {
                colortable[clr_cnt]=idx2clr(idxstack);//use path histoty during traversal
                for(int c=0;c<3;c++)//cout <<current->total_channel[c]/current->numvisits<<",\t";//use average color compare the two method to make sure nothing wrong
                    cout<<colortable[clr_cnt][c]<<",";
                cout <<"\t";clr_cnt++;break;
            }
            case 1:
            {
                //don't return anything
            }
        }}
    if(current->childcount>=2&&(lowest->level<current->level||lowest->level>0&&lowest->level==current->level&&lowest->childcount>current->childcount))
    {lowest=current;}//pick the lowest level
    for(int ch=0;ch<8;ch++)
        if(current->child[ch]!=(treenode *)NULL)
        {idxstack[current->level]=ch;traverse(token,current->child[ch],lt,idxstack);}
}
