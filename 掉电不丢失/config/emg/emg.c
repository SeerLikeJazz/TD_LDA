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
//�������ƣ�feature_extraction
//���ܣ����洢��data��300�����ݵ����������ȡ
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
			uint16_t  j0=(j+count_temp *TIME_INCREMENT)%600;//LZX//<��main��copy>count_temp=6-46, 56-96.............
			int16_t  j1=(j-1+count_temp *TIME_INCREMENT)%600;
			int16_t  j2=(j-2+count_temp *TIME_INCREMENT)%600;
			feat[i*4]+=fabs(data[j0][channel_index]); //LZX//<MAV,�е㲻һ��>
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
//��ʼ��FEAT
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
	//TEMP_MAT.pData=Temp_Mat;//������ʱ�����STM32��������
	//TEMP_MAT_transpose.numCols =32;
	//TEMP_MAT_transpose.numRows =1;
	//TEMP_MAT.pData=Temp_Mat_T;//������ʱ�����STM32��������	
	
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
  
	 //�����ֵ����
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
//����Э���������

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
		 //////////////////////����Э�������
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
//////////////////////����������ͶӰ����32*32��Э�������������CovMat_Inverse���ֵ����MeanMatת�õĳ˻�
//////////////////////LDA_TEMP�����洢
 
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

////////////////////////////////////////////////����ƫ��
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



//LZX//<��������>
void qiuni(void)
{
    int16_t i,j,m,k;
	float32_t t;
	for(i=0;i<32;i++)//���Ƶ�a[32,32]
    { 
   
		 for(k=0;k<32;k++)
		 {
			     a[i][k] =CovMat[i][k];
		 }	  
	   
    }
   for(i=0;i<N;i++)//�ұ߸�ֵ0
   {
 	 for(j=N;j<2*N;j++)
	 {
		 b[i][j]=0;
	 }
   }
  for(i=0;i<N;i++)//б���и�ֵ1
  {
	b[i][N+i]=1;	
  }
  for(i=0;i<N;i++)//��a����ֵ��b���
  {
	for(j=0;j<N;j++)
      {	
		b[i][j]=a[i][j];
	  }
   }
  for(m=0;m<N;m++)          //��ÿ�н��д��� line_750 
  {  
        t=b[m][m];                  //Ԥ��b[m][m]��  
        i=m;                         
   while(b[m][m]==0) 
   {     
     b[m][m]=b[i+1][m];   
     i++;  
   }  
   if(i>m)  
   { 
        b[i][m]=t;                  //ʵ�ֽ�����    
                      
 //��������������Ӧλ�õ�Ԫ�� 
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
		b[i][j]=b[i][j]-b[i][m]*b[m][j]/b[m][m]; //m=0ʱ������һ�е�-b[i][0]/b[0][0]���ӵ����¸��С���������ÿ�е�һ��Ԫ��b[i][0]��Ϊ0��  
		}	
	}		
	for(j=2*N-1;j>=m;j--) 
		{
        b[m][j]/=b[m][m];   //�Ե�m�����б任��ͬ����b[m][m]��ʹb[m][m]Ϊ1��   	
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
        for(j=2*N-1;j>=m;j--)           //ǧ��ע�⣬�˴�j����ݼ�������b[i][m]�ȱ�Ϊ0������ļ������Ч��     
        b[i][j]-=b[i][m]*b[m][j];        
	}
	 m--;  
}	
  for(i=0;i<N;i++)                         //�����������ά����c�С�   
 {
	 for(j=0;j<N;j++)    
     c[i][j]=b[i][N+j];   
 }
 }
