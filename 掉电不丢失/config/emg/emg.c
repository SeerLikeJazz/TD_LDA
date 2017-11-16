#include "emg.h"

#define N 32
float32_t feat[33];
float32_t feat_matrix[410][32];
uint16_t LABEL[410] ;
float32_t feat_num_perclass[6]={0,0,0,0,0,0};
uint16_t count=0;
uint8_t channel_used[8]={0,1,2,3,4,5,6,7};
uint8_t channel_index;
float32_t pDataDst[6];
float32_t LDA_TEMP[32][6];
float32_t MeanMat[192] ;
float32_t CovMat[32][32]; 
float32_t CovMat_Inverse[32][32];
float32_t CovMat_class[32][32] ;
float32_t Temp_Mat[32];
float32_t Temp_Mat_T[32];
float32_t Bias_Mat[6];
float32_t a[32][32],b[32][64],c[32][32];
//////////////////////////////////////////////
//函数名称：feature_extraction
//功能：将存储在data的300个数据点进行特征提取
/////////////////////////////////////////////
void feature_extraction(void)
{ uint16_t i=0,j=0;
  //static u16 count_temp=0;
  for(i=0;i<DATA_CHANNEL;i++)
	{
	  channel_index=channel_used[i];
	  feat[i*4]=0;feat[i*4+1]=0;feat[i*4+2]=0;feat[i*4+3]=0;
	  for(j=0;j<TIME_INTERVAL;j++)
		{
			uint16_t  j0=(j+count_temp *TIME_INCREMENT)%600;//LZX//<从main中copy>count_temp=6-46, 56-96.............
			int16_t  j1=(j-1+count_temp *TIME_INCREMENT)%600;
			int16_t  j2=(j-2+count_temp *TIME_INCREMENT)%600;
			feat[i*4]+=fabs(data[j0][channel_index]); //LZX//<MAV,有点不一样>
			if (j>0)
			{
			feat[i*4+1]+= (data[j0][channel_index]*(data[j1][channel_index])>0); //LZX//<ZC>
				feat[i*4+2]+= fabs(data[j0][channel_index]-data[j1][channel_index]); //LZX//<WV>
			}
			if (j>1)
			{
				feat[i*4+3]+= ((data[j0][channel_index]-data[j1][channel_index])*(data[j1][channel_index]-data[j2][channel_index])>0);//LZX//<SSC>
			
			}
	  } 
	}
 //count_temp++;
}

uint8_t LDA_predict(float32_t feat[33])	
{ uint8_t i,j=0;
  float32_t gx; 
  float32_t MAXgx;
  uint8_t predict_temp; 
  feat[32]=1;
///////////////////////////////////
//初始化FEAT
  for(i=0;i<6;i++)
	{
		pDataDst[i]=0;
	}
  for(i=0;i<6;i++)
  {
	for(j=0;j<32;j++)
	  { 
		 pDataDst[i]+=LDA_TEMP[j][i]*feat[j];
	  }
  }
  
  for(i=0;i<6;i++)
  {
		 pDataDst[i]+=Bias_Mat[i];
	  
  }
  
  for (i=0;i<6;i++)
  {
    gx=pDataDst[i];
	if(i==0)
	{
	  MAXgx =gx;
	  predict_temp=i;
	}
	else
	{
	  if(gx>MAXgx)
	  {
	    MAXgx=gx;
		predict_temp=i;
	  }	
	}
  }
	count++;
  return predict_temp;
}

void LDA_train(float32_t feat_matrix[410][32])
{  
	
uint16_t i,j,k,p,q=0;
    
	//TEMP_MAT.numCols =1;
	//TEMP_MAT.numRows =32;
	//TEMP_MAT.pData=Temp_Mat;//传递临时矩阵给STM32矩阵库对象
	//TEMP_MAT_transpose.numCols =32;
	//TEMP_MAT_transpose.numRows =1;
	//TEMP_MAT.pData=Temp_Mat_T;//传递临时矩阵给STM32矩阵库对象	
	
  //INITIALIZE DATA 
	for(i=0;i<32;i++)
	{
		for(j=0;j<6;j++)
		{
	       LDA_TEMP[i][j]=0;
		}
	}
    for(i=0;i<192;i++)
	{
	  MeanMat[i]=0;
	
	}
	for(i=0;i<32;i++)
	{
	  for(j=0;j<32;j++)
		{
		  CovMat[i][j]=0;	
      CovMat_class[i][j]=0;		
	    CovMat_Inverse[i][j]=0;			
		}
	}
	
	 for(i=0;i<32;i++)
	{
	  Temp_Mat[i]=0;
	  Temp_Mat_T[i]=0;
		
	}	
	 for(i=0;i<6;i++)
	{
	 Bias_Mat[i]=0;
		
	}
	 for(i=0;i<DATA_LENGTH/(1000*ACTION_TIME_TRAINING);i++)
  {
    for(j=0;j<41;j++)
    LABEL[j+i*41]=action_serial[i];//LZX//<u16 action_serial[10]={0,5,1,5,2,5,3,5,4,5};>
  }
  
	 //计算均值矩阵
  for(j=0;j<6;j++)
  {
   for(i=0;i<410;i++)
   {
     if((LABEL[i]-UNILABEL[j])==0)  //LZX//<u16 UNILABEL[6]={0,1,2,3,4,5};>
	 {  
	   for(k=0;k<32;k++)
		 { 			
		   MeanMat[j*32+k]=MeanMat[j*32+k]+feat_matrix[i][k]; 		 				 
		 } 
	  feat_num_perclass[j]=feat_num_perclass[j]+1;	
	 }   
   }	 
  }
 for(i=0;i<6;i++)
 {
   for(k=0;k<32;k++)	 
		MeanMat[i*32+k]=MeanMat[i*32+k]/feat_num_perclass[i];	 
 }
//计算协方差矩阵呢

 for(j=0;j<6;j++)
 {  
   for(i=0;i<410;i++)
   {
     if(LABEL[i]==UNILABEL[j])
	 {   			
	     for(k=0;k<32;k++)
		 {
		   Temp_Mat[k]=feat_matrix[i][k]-MeanMat[j*32+k];   
           
		 }
		 //////////////////////计算协方差矩阵
         for(p=0;p<32;p++)
		 {
		   for(q=0;q<32;q++)
		   {
			  CovMat_class[p][q]=Temp_Mat[p]*Temp_Mat[q];
		   }		 
		 }		 
		 /////////////////////
		 for(p=0;p<32;p++)
		 {
		   for(q=0;q<32;q++)
		   {
			  CovMat[p][q]=CovMat[p][q]+CovMat_class[p][q];
		   }		 
		 }	
		 
	 }
	 
   }	 
 
}
         for(p=0;p<32;p++)
		 {
		   for(q=0;q<32;q++)
		   {
			  CovMat[p][q]=CovMat[p][q]/404;//410-6
		   }		 
		 } 
		 for(i=0;i<32;i++)
         {
           CovMat[i][i]+=1;//+I
         }
	     qiuni();
for(i=0;i<32;i++)
 { 
   
		 for(k=0;k<32;k++)
		 {
			 CovMat_Inverse[i][k]=c[i][k];
			     
		 }	  
	   
 }	
//////////////////////接下来计算投影矩阵32*32的协方差矩阵的逆矩阵CovMat_Inverse与均值矩阵MeanMat转置的乘积
//////////////////////LDA_TEMP用来存储
 
	for(i=0;i<32;i++)
	{
		for(j=0;j<6;j++)
		{  
		   for(p=0;p<32;p++)
		   {		     			   
			  LDA_TEMP[i][j]+=CovMat_Inverse[i][p]*MeanMat[j*32+p];		      		 
		    } 		
		}
	}
////////////////////// 

////////////////////////////////////////////////计算偏置
for(i=0;i<6;i++)
{
  for(j=0;j<32;j++)
	{
      Bias_Mat[i]+=MeanMat[i*32+j]*LDA_TEMP[j][i];

	}
}
for(i=0;i<6;i++)
{
  Bias_Mat[i]=-0.5*Bias_Mat[i];
}

 LED7=0;//DENOTE THE END OF TRAINING
 trainig_flag=0;

}



//LZX//<矩阵求逆>
void qiuni(void)
{
    int16_t i,j,m,k;
	float32_t t;
	for(i=0;i<32;i++)//复制到a[32,32]
    { 
   
		 for(k=0;k<32;k++)
		 {
			     a[i][k] =CovMat[i][k];
		 }	  
	   
    }
   for(i=0;i<N;i++)//右边赋值0
   {
 	 for(j=N;j<2*N;j++)
	 {
		 b[i][j]=0;
	 }
   }
  for(i=0;i<N;i++)//斜对行赋值1
  {
	b[i][N+i]=1;	
  }
  for(i=0;i<N;i++)//把a方阵赋值给b左边
  {
	for(j=0;j<N;j++)
      {	
		b[i][j]=a[i][j];
	  }
   }
  for(m=0;m<N;m++)          //对每行进行处理。 line_750 
  {  
        t=b[m][m];                  //预存b[m][m]。  
        i=m;                         
   while(b[m][m]==0) 
   {     
     b[m][m]=b[i+1][m];   
     i++;  
   }  
   if(i>m)  
   { 
        b[i][m]=t;                  //实现交换。    
                      
 //交换其它各列相应位置的元素 
	    for(j=0;j<m;j++)                       
	   { 
	     t=b[m][j];   
	   b[m][j]=b[i][j];   
	   b[i][j]=t;  
	   } 
	    for(j=m+1;j<2*N;j++)                       
	   { 
	     t=b[m][j];   
	   b[m][j]=b[i][j];   
	   b[i][j]=t;  
	   }    
    }   
 for(i=m+1;i<N;i++) 
    {
		for(j=2*N-1;j>=m;j--) 
        {
		b[i][j]=b[i][j]-b[i][m]*b[m][j]/b[m][m]; //m=0时，将第一行的-b[i][0]/b[0][0]倍加到以下各行。这样以下每行第一个元素b[i][0]就为0。  
		}	
	}		
	for(j=2*N-1;j>=m;j--) 
		{
        b[m][j]/=b[m][m];   //对第m行作行变换，同除以b[m][m]，使b[m][m]为1。   	
		}
}
  for(j=2*N-1;j>=m-1;j--)
  {
     b[m][j]=b[m][j]/b[m][m];
  
  
  }  
 m=N-1;  
while(m>0)  
{ 
  for(i=0;i<m;i++) 
	{
        for(j=2*N-1;j>=m;j--)           //千万注意，此处j必须递减，否则b[i][m]先变为0，后面的计算就无效！     
        b[i][j]-=b[i][m]*b[m][j];        
	}
	 m--;  
}	
  for(i=0;i<N;i++)                         //将逆矩阵存入二维数组c中。   
 {
	 for(j=0;j<N;j++)    
     c[i][j]=b[i][N+j];   
 }
 }
