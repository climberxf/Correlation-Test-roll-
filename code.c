#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define min 1e-15
#define max_cell 20          //小区最大数量 
#define pss_size 5
#define res_size 20

typedef struct cell
{//小区信息 
	int id;//小区号
	float P;//平均功率 
}CELL;
typedef struct result
{//卷积结果信息
	int pos;//存放最大值的位置
	float value;//存放最大值的值
}RESULT;

int get_path(char warn[],char path[][60]);
float getPi(float a,float b);
void sort(CELL cellInfo[],int n);
int getRow(char path[]);
void get_averP(CELL *cellInfo,char datePath[]);
void roll(char dPath[],char pPath[],char rPath[],RESULT *res);


int main()
{				
	FILE *fp;
	float I,Q;    //I,Q分别为实部、虚部 
	int i,j,k=0,n1,n2,n3,studyID;    //n1为实际小区数,n2为pss文件数，n3为result文件数 
	char c; 
	CELL cellInfo[max_cell];     //小区数据 
	RESULT res[res_size];        //最大相关结果
	char datePath[max_cell][60];     //date文件路径
	char pssPath[pss_size][60];             //PSS文件路径
	char resPath[res_size][60];   //卷积结果文件路径
	printf("请输入学号尾数：");
	scanf("%d",&studyID);

	n1=get_path("请输入小区数目:",datePath);
	for(i=0;i<n1;i++)
	{//文件循环 
		cellInfo[i].id=i;
		get_averP(&cellInfo[i],datePath[i]);
	}
	sort(cellInfo,n1);
	for(i=0;i<6;i++)
	{//输出平均功率最大的前六个小区的编号 
		printf("%d ",cellInfo[i].id);
	}
	printf("\n");
	n2=get_path("请输PSS文件数目:",pssPath);
	n3=get_path("请输入结果文件数目:",resPath);
	k=0;
	for(i=0;i<6;i++)
	{
		for(j=0;j<n2;j++)
		{
			roll(datePath[cellInfo[i].id],pssPath[j],resPath[k],&res[k]);
			printf("最大位置：%-5d 最大值：%.3f\n",res[k].pos,res[k].value);
			k++;
		}
	}
}
/*********************************************************
*函数功能：输入已知文件路径
*函数原型： int get_path(char warn[],char path[][50])
*函数说明： warn[]为提示语，path[][50]为路径数组
*返回值：int型 返回路径文件的个数
*创建人：奚兴发
*修改记录：
*v1.0    2023.3.15 
*********************************************************/
int get_path(char warn[],char path[][60])
{
	int i,n;
	printf("%s",warn);
	scanf("%d",&n);
	getchar();
	printf("请输入数据文件绝对路径：\n");
	for(i=0;i<n;i++)
	{
		gets(path[i]);
	} 
	return n;
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
void get_averP(CELL *cellInfo,char datePath[])
{
	float I,Q;
	FILE *fp;
	cellInfo->P=0;
	if((fp=fopen(datePath,"r"))==NULL)
	{//判断文件是否成功打开 
		printf("Fail to open %s\n",datePath);
		exit(0);
	}
	while(!feof(fp))	
	{
		I=0;Q=0;
		fscanf(fp,"%f",&I);
		fscanf(fp,"%f",&Q);
		cellInfo->P+=getPi(I,Q);
	}
	cellInfo->P=cellInfo->P/(getRow(datePath)/2);//求平均功率 
	fclose(fp);
}
/************************************************************************************
*函数功能：卷积dPath[],pPath[]里的数据，并将卷积的位置与数据写于
rPath[]文件里，将卷积的最大值的位置与值存于结构体res里
*函数原型： void roll(char dPath[],char pPath[],char rPath[],RESULT *res)
*函数说明：dPath[],pPath[],rPath[]分别为date，pss，结果文件的路径，*res存入每次卷的结果
*返回值：void型
*创建人：奚兴发
*修改记录：
*v1.0    2023.3.22 
***********************************************************************************/
void roll(char dPath[],char pPath[],char rPath[],RESULT *res)
{
	float a,b,c,d,I=0,Q=0,P_max,P;
	int d_count=getRow(dPath)/2;
	int p_count=getRow(pPath)/2,flag=0;
	int i,j,position,row=1;
	char ch;
	FILE *fp1,*fp2,*fp3;
	fp1=fopen(dPath,"r");
	fp2=fopen(pPath,"r");
	fp3=fopen(rPath,"w");
	if(fp1==NULL||fp2==NULL||fp3==NULL)
	{
		printf("Fail to open dPath,pPath and rPath\n");
		exit(0);
	}
	for(i=0;i<d_count-p_count+1;i++)
	{
		P=0;I=0;Q=0;
		while(!feof(fp2))
		{
			a=0;b=0;c=0;d=0;
			fscanf(fp1,"%f",&a);
			fscanf(fp1,"%f",&b);
			fscanf(fp2,"%f",&c);
			fscanf(fp2,"%f",&d);
			ch=fgetc(fp2);
			if(fabs(c)>min&&fabs(d)>min)
			{
				I+=a*c-b*d;
				Q+=a*d+b*c;
			}
		}
		P=getPi(I,Q);
		fprintf(fp3,"%-16.3f %d\n",P,i);
		if(P>P_max)
		{
			P_max=P;
			position=i;
		}
		row+=2;    //row指示每次循环从第几排开始
		rewind(fp2);
		rewind(fp1);
		flag=0;
		while(flag<row-1)
		{//将fp1返回下一次开始卷的地方
			ch=fgetc(fp1);
			if(ch=='\n')
				flag++;
		}
		//printf("\n");
	}
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	res->value=P_max;
	res->pos=position;
}