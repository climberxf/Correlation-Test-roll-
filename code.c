#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define max_cell 20          //小区最大数量 
#define pss_size 5
#define res_size 20
#define maxPath 60
#define dPath "D:\\users\\desktop\\first date\\data%d.txt"//date文件路径
#define pPath "D:\\users\\desktop\\first date\\PSS%d.txt"//PSS文件路径
#define rPath "D:\\users\\desktop\\first date\\result%d-%d.xls"//结果EXCEL文件路径
typedef struct cell
{//小区信息 
	int id;//小区号
	float P;//平均功率 
}CELL;
typedef struct result
{//卷积结果信息
	int pos;//存放最大值的位置
	float value;//存放最大值的值
	float pss_id;//存放pss文件的id号
	float data_id;//存放data文件id号
}RESULT;

float getPi(float a,float b);
void sort(CELL cellInfo[],int n);
int getRow(char path[]);
void get_averP(CELL *cellInfo,char dataPath[]);
float* getDate(char path[],int count);
void roll(float* dData,float* pData,int d_count,int p_count,RESULT *res,int p,int d);


int main()
{				
	float I,Q;    //I,Q分别为实部、虚部 
	int i,j,n1,n2,k=0,studyID,i1;    //n1为实际小区数,n2为pss文件数，n3为result文件数 
	CELL cellInfo[max_cell];     //小区数据 
	RESULT res[res_size];        //最大相关结果
	char dataPath[maxPath];     //date文件路径
	char pssPath[maxPath];             //PSS文件路径
	int d_count,p_count;               //data文件数据的数量及PSS文件数据的数量

	printf("请输入学号尾数：");
	scanf("%d",&studyID);
	printf("data文件的数量为:");
	scanf("%d",&n1);

	for(i=0;i<n1;i++)
	{//文件循环 
		sprintf_s(dataPath,60,dPath,i);                                      //如何路径改变
		cellInfo[i].id=i;
		get_averP(&cellInfo[i],dataPath);
	}
	sort(cellInfo,n1);
	for(i=0;i<6;i++)
	{//输出平均功率最大的前六个小区的编号 
		printf("%d ",cellInfo[i].id);
	}
	
	printf("\n请输入PSS文件个数:");
	scanf("%d",&n2);
	float *pData=NULL,*dData=NULL;
	printf("PSS-DATE\t最大位置\t最大值\n");

	int maxRelaId[3]={0};//与pss最相关的data文件id
	float max;
	for(i=0;i<n2;i++)
	{
		max=0;
		sprintf_s(pssPath,60,pPath,i);
		p_count=getRow(pssPath);
		pData=getDate(pssPath,p_count);
		for(j=0;j<6;j++)
		{
			sprintf_s(dataPath,60,dPath,cellInfo[j].id);
			d_count=getRow(dataPath);
			dData=getDate(dataPath,d_count);
			roll(dData,pData,d_count,p_count,&res[k],i,cellInfo[j].id);
			free(dData);
			if(res[k].value>max)
			{
				maxRelaId[i]=res[k].data_id;
				max=res[k].value;
			}
			printf("%d-%d\t%d\t%.3f\n",i,cellInfo[j].id,res[k].pos,res[k].value);
			k++;
		}
		free(pData);
	}
	for(i=0;i<n2;i++)
	{
		printf("与PSS%d文件最相关的是:data%d文件\n",i,maxRelaId[i]);
	}
}
/*********************************************************
*函数功能：计算每组数据功率 
*函数原型： float getPi(float a,float b)
*函数说明： float a,float b 分别是IQ圆制信号
*返回值：float型 计算的功率值 
*创建人：奚兴发
*修改记录：
*v1.0    2023.3.15
*********************************************************/
float getPi(float a,float b)
{
	return pow(a*a+b*b,0.5);
}
/*********************************************************
*函数功能：排序（降序） 
*函数原型： void sort(CELL cellInfo[],int n)
*函数说明： cellInfo[]为要排序的结构体，n为 cellInfo[]数组的个数 
*返回值：void型 
*创建人：奚兴发
*修改记录：
*v1.0    2023.3.15 
*********************************************************/
void sort(CELL cellInfo[],int n)
{
	int i,j,k;
	CELL temp;
	for(i=0;i<n-1;i++)
	{
		k=i;
		for(j=i+1;j<n;j++)
			if(cellInfo[j].P>cellInfo[k].P)
				k=j;
		if(k!=i)
		{
			temp=cellInfo[k];
			cellInfo[k]=cellInfo[i];
			cellInfo[i]=temp;
		}
	}	
}
/*********************************************************
*函数功能：计算文件的行数
*函数原型： int getRow(char path[])
*函数说明：path[]为路径 
*返回值：int型 返回文件的排数
*创建人：奚兴发
*修改记录：
*v1.0    2023.3.15 
*********************************************************/
int getRow(char path[])
{
	FILE *fp;
	int r=0;
	char c;
	if((fp=fopen(path,"r"))==NULL)
	{
		printf("Fail to open %s\n",path);
		exit(0);
	}
	while(!feof(fp))
	{
		if(c=fgetc(fp)=='\n')
			r++;
	}
	fclose(fp);
	return r;
}
/*********************************************************
*函数功能：计算文件的平均功率
*函数原型： get_averP(CELL *cellInfo,char datePath[])
*函数说明：datePath[]为路径， cellInfo为小区信息
*返回值：void型
*创建人：奚兴发
*修改记录：
*v1.0    2023.3.15 
*********************************************************/
void get_averP(CELL *cellInfo,char dataPath[])
{
	float I,Q;
	FILE *fp;
	cellInfo->P=0;
	if((fp=fopen(dataPath,"r"))==NULL)
	{//判断文件是否成功打开 
		printf("Fail to open %s\n",dataPath);
		exit(0);
	}
	while(!feof(fp))	
	{
		I=0;Q=0;
		fscanf(fp,"%f",&I);
		fscanf(fp,"%f",&Q);
		cellInfo->P+=getPi(I,Q);
	}
	cellInfo->P=cellInfo->P/(getRow(dataPath)/2);//求平均功率 
	fclose(fp);
}
/*********************************************************
*函数功能：获得路径path[]的文件的数据，存入申请的count大小的空间中
*函数原型： float* getDate(char path[],int count)；
*函数说明：path[]为文件路径，count为文件里的数据数量
*返回值：float*型，返回申请空间的首地址
*创建人：奚兴发
*修改记录：
*v1.0    2023.3.15 
*********************************************************/
float* getDate(char path[],int count)                                      //错误：函数的设置
{
	float *data=(float *)malloc(count*sizeof(float));
	FILE *fp ;
	int i=0;
	if(fopen_s(&fp,path,"r"))
	{
		printf("fail to open int getDate()\n");
		exit(0);
	}
	while(i<count)
	{
		fscanf(fp,"%f",&data[i]);
		i++;
	}
	fclose(fp);
	return data;
}
/************************************************************************************
*函数功能：卷积数组dData[],pData[]里的数据，并将卷积的位置与数据写于
			path[]文件里，将卷积的最大值的位置与值存于结构体res里
*函数原型： void roll(float* dData,float* pData,int d_count,int p_count,RESULT *res,int p,int d)
*函数说明：数组dData[],pData[]是要卷积的数据int d_count,int p_count分别是两个数组的大小，*res存入每次卷的结果
			p,d分别是数组对应的PSS,data文件号
*返回值：void型
*创建人：奚兴发
*修改记录：
*v1.0    2023.3.22 
***********************************************************************************/
void roll(float* dData,float* pData,int d_count,int p_count,RESULT *res,int p,int d)
{
	float I=0,Q=0,P_max,P;   //P_max为每两个卷积文件的最大的模                    
	                         //I,Q分别是每次卷积的复数的实部和虚部的和
	int i,j,k,position,row=0;
	                         //position是最大相关的地方
	char path[maxPath];      //结果文件的路径                       错误：只有一个文件
	FILE *fp;
	sprintf_s(path,maxPath,rPath,p,d);
	d_count=d_count/2;       //文件的数据组有多少个
	p_count=p_count/2;       //文件的数据组有多少个
	if(fopen_s(&fp,path,"w"))
	{
		printf("fail to open int roll()\n");
		exit(0);
	}
	
	for(i=0;i<d_count-p_count+1;i++)                               //错误：超出范围
	{
		P=0;I=0;Q=0;
		for(j=0,k=row;j<p_count*2-2;j+=2,k+=2)
		{
			I+=dData[k]*pData[j]-dData[k+1]*pData[j+1];
			Q+=dData[k]*pData[j+1]+dData[k+1]*pData[j];
		}
		P=getPi(I,Q);
		fprintf(fp,"%d\t%.3f\t",i,P);//给EXCEL文件输入位置和对应的相关系数
		if(P>P_max)
		{
			P_max=P;
			position=i;
		}
		row+=2;    //row指示每次循环从第几排开始
		fprintf(fp,"\n");
	}
	fclose(fp);
	res->value=P_max;
	res->pos=position;
	res->pss_id=p;
	res->data_id=d;//存入文件id
}