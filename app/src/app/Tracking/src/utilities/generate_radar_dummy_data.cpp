/*
by A21128
generate phased-array radar dummy data(single object) 
based on CA model 
*/

#include <math.h>
#include <iostream>
#include <stdlib.h>

#define MAX_STEPS_NUM 10
#define MAX_RAND_OFF 20

//azimuth 1~30:-58:4:58(degree) //fovH120
//elevation 1~4:-15:10:15  //fovV40
float arr_fRange[MAX_STEPS_NUM]={1000,990,980,970,960,950,940,930,920,910};
int arr_nTimeSeq[MAX_STEPS_NUM]={1,4,7,100,121,180,200,250,260,300};//*1.0f/120
float arr_fX12[12*MAX_STEPS_NUM];//(x,vx,ax,y,vy,ay,z,vz,az)+(u0,u1,u2)
float arr_fU[3*MAX_STEPS_NUM];
float va[6]={2,2,1,3,3,2};//x,vx,ax,y,vy,ay,z,vz,az

int frameId2sph(int frameID)
{
    float fRange=arr_fRange[frameID];
    int aziBeamId=(arr_nTimeSeq[frameID]/4)%30;
    int eleBeamId=arr_nTimeSeq[frameID]%4;
    float azimuth=-1.0*(-58+4*aziBeamId)*3.1415926/180.f;
    float elevataion=(-15+10*eleBeamId)*3.1415926/180.f;
    int index0=(frameID-0)*12;
    arr_fX12[index0+0]=fRange*cos(elevataion)*cos(azimuth);
    arr_fX12[index0+3]=fRange*cos(elevataion)*sin(azimuth);
    arr_fX12[index0+6]=fRange*sin(elevataion);

    std::cout<<"aziBeamId:"<<aziBeamId<<","<<"eleBeamId:"<<eleBeamId<<std::endl;
    std::cout<<"fXYZ:"<<arr_fX12[index0+0]<<","<<arr_fX12[index0+3]<<","<<arr_fX12[index0+6]<<std::endl;
    
    return 0;
}

void init(int frameID){//frameID==0
    //0
    // for(int k = 0; k< MAX_STEPS_NUM;k++){
    //     arr_fRange[k]=1000 - MAX_RAND_OFF*k;
    //     arr_nTimeSeq[k]=1 + k*25;
    // }

    //1当前step的状态x,vx,ax,y,vy,ay,z,vz,az k-1
    frameId2sph(frameID);//由fov:xyz大概比例关系
    arr_fX12[1]=va[0];
    arr_fX12[2]=va[1];
    arr_fX12[4]=va[2];
    arr_fX12[5]=va[3];
    arr_fX12[7]=va[4];
    arr_fX12[8]=va[5];  
}

void calcU(int frameID){
    //3
    int nFrameIDPost=frameID+1;
    float T=(arr_nTimeSeq[nFrameIDPost]-arr_nTimeSeq[frameID])/120.f;
    
    int index0=(frameID-0)*12;
    int index1=(nFrameIDPost-0)*12;
    float T2=T*T;
    float T2_2=2/T2;
    arr_fU[frameID*3+0]=arr_fX12[index0+9]=(arr_fX12[index1+0]-(arr_fX12[index0+0]+T*arr_fX12[index0+1]+0.5f*T2*arr_fX12[index0+2]))*T2_2;
    arr_fU[frameID*3+1]=arr_fX12[index0+10]=(arr_fX12[index1+3]-(arr_fX12[index0+3]+T*arr_fX12[index0+4]+0.5f*T2*arr_fX12[index0+5]))*T2_2;
    arr_fU[frameID*3+2]=arr_fX12[index0+11]=(arr_fX12[index1+6]-(arr_fX12[index0+6]+T*arr_fX12[index0+7]+0.5f*T2*arr_fX12[index0+8]))*T2_2;

    std::cout<<"fU:"<<arr_fX12[index0+9]<<","<<arr_fX12[index0+10]<<","<<arr_fX12[index0+11]<<std::endl;
}

void calcVA(int frameID){
    //4 k
    int nFrameIDPre=frameID-1;
    float T=(arr_nTimeSeq[frameID]-arr_nTimeSeq[nFrameIDPre])/120.f;
     int index_1=(nFrameIDPre-0)*12;
    int index0=(frameID-0)*12;
   
    //V
    arr_fX12[index0+1] = arr_fX12[index_1+1]+T*arr_fX12[index_1+2]+T*arr_fX12[index_1+9];
    arr_fX12[index0+4] = arr_fX12[index_1+4]+T*arr_fX12[index_1+5]+T*arr_fX12[index_1+10];
    arr_fX12[index0+7] = arr_fX12[index_1+7]+T*arr_fX12[index_1+8]+T*arr_fX12[index_1+11];

    //A
    arr_fX12[index0+2] = arr_fX12[index_1+2]+arr_fX12[index_1+9];
    arr_fX12[index0+5] = arr_fX12[index_1+5]+arr_fX12[index_1+10];
    arr_fX12[index0+8] = arr_fX12[index_1+8]+arr_fX12[index_1+11];

    std::cout<<"fV:"<<arr_fX12[index0+1]<<","<<arr_fX12[index0+4]<<","<<arr_fX12[index0+7]<<std::endl;
    std::cout<<"fA:"<<arr_fX12[index0+2]<<","<<arr_fX12[index0+5]<<","<<arr_fX12[index0+8]<<std::endl;
}


//used to show
int cartesianXYZ_2_sphricalRAE(float x,float y,float z,float* rae)////(x,y,z); //(range,azimuth,elevation)
{
//     xyz[0]=rae[0]*cos(rae[1])*cos(rae[2]);
//     xyz[1]=rae[0]*cos(rae[1])*sin(rae[2]);
//     xyz[2]=rae[0]*sin(rae[1]);
    //calc range
    float r=sqrtf(x*x+y*y+z*z);
    rae[0]=r;
    if(rae[0] < 0.1)
        rae[0]=0.1;
    //calc azimuth
    float azimuth=-atan2(y,x);//cdx1 方向问题
    rae[1]= azimuth;
    //calc elevation
    float elevation=atan2(z,sqrt(x*x+y*y));
    rae[2]=elevation;
  
    return 0;
}

//used to show
int calc_aziBeamId(float angle)
{
    float angleDegree=angle*180/3.1415926f;
    int aziBeamId=-2;
    if (angleDegree < -60+0.001)
        aziBeamId=-2;
    else if(angleDegree > 60-0.001)
        aziBeamId=-1;
    
    aziBeamId = (int)((angleDegree+60)/4);
    return aziBeamId;
}

//used to show
int calc_eleBeamId(float angle)
{
    float angleDegree=angle*180/3.1415926f;
    int eleBeamId=-2;
    if (angleDegree < -20+0.001)
        eleBeamId=-2;
    else if(angleDegree > 20-0.001)
        eleBeamId=-1;
    
    eleBeamId = (int)((angleDegree+20)/10);
    return eleBeamId;
}

void show(int frameID){
    //6 calc aziBeamId,eleBeamId;to verify   
    float rae[3]={};
    int index=(frameID-0)*12;
    cartesianXYZ_2_sphricalRAE(arr_fX12[index],arr_fX12[index+3],arr_fX12[index+6],rae);
    int aziBeamId=calc_aziBeamId(rae[1]);
    int eleBeamId=calc_eleBeamId(rae[2]);
    float fV=(arr_fX12[index]*arr_fX12[index+1]+arr_fX12[index+3]*arr_fX12[index+4]+arr_fX12[index+6]*arr_fX12[index+7])/rae[0];
    std::cout<<"    range:"<<rae[0]<<std::endl;
    std::cout<<"    azimuth:"<<rae[1]<<","<<"elevation:"<<rae[2]<<std::endl;
    std::cout<<"    azimuth:"<<rae[1]*180/3.1415926f<<","<<"elevation:"<<rae[2]*180/3.1415926f<<std::endl;
    std::cout<<"    aziBeamId:"<<aziBeamId<<","<<"eleBeamId:"<<eleBeamId<<std::endl;
    std::cout<<"    doppler:"<<fV<<std::endl;
}

int main(){

    std::cout<<"0---------------------------------"<<std::endl;
    init(0);
    for (int i = 1; i < MAX_STEPS_NUM; i++)
    {
        std::cout<<i<<"---------------------------------"<<std::endl;
        //2x/y/z
        frameId2sph(i);
        //3
        calcU(i-1);
        //4
        calcVA(i);
        //6
        //show(i);
    }

    printf("\n------------------------------------\n");
    for (int i = 0; i < MAX_STEPS_NUM; i++){
        int index0=i*12;
        float rae[3]={};
        cartesianXYZ_2_sphricalRAE(arr_fX12[index0],arr_fX12[index0+3],arr_fX12[index0+6],rae);
        int aziBeamId=calc_aziBeamId(rae[1]);
        int eleBeamId=calc_eleBeamId(rae[2]);
        float fV=(arr_fX12[index0]*arr_fX12[index0+1]+arr_fX12[index0+3]*arr_fX12[index0+4]+arr_fX12[index0+6]*arr_fX12[index0+7])/rae[0];
        
        //输出需要的单目标的dummy data，多目标的在工程中rand即可
        printf("%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,", \
                            arr_fX12[index0+0],arr_fX12[index0+1],\
                            arr_fX12[index0+3],arr_fX12[index0+4],\
                            arr_fX12[index0+6],arr_fX12[index0+7],\
                            rae[0],rae[1]*180/3.1415926f,rae[2]*180/3.1415926f, fV);
    }
    printf("\n==============================\n");
    
    return 0;
}