//***************************************************************************************
// GameTimer.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include <windows.h>
#include "GameTimer.h"
////����GameTimer�࣬ ��Ϊ.cpp�ļ�




//��GameTimer���װ��ϣ��������Կ��Ƽ�ʱ���Ŀ��أ����Լ���֡��֡��ʱ�����������Լ�����Ϸ������ʱ�䡣
//֮�������ļ���WinMain.cpp)����һ��GameTimerʵ�� : GameTimer gt;
//GameTimerΪ��  gtΪʵ��




//<<<<<<<<<<<<<<<<<<<<<<���캯��>>>>>>>>>>>>>>>>>>>>>>>>>
//Ȼ����cpp�ļ������ǰ���ʵ�֡������ǹ��캯�������캯�����ѯ���ܼ�������Ƶ��(һ�ζ�����)�����ĺ���QueryPerformanceCounter���ص�ǰʱ��ļ���������ֵ����mSencondsPerCount�����ֵ���Դ�������ʱ�䡣ע�⣬������Ĳ�����ϵͳ����ʱ��ֵ�Զ���ֵ����������ʱ���������������Ӷ��ı����ʱ�Ĵ���ֵ����������Ĳ���������Ҫ�Լ���ʼ����
GameTimer::GameTimer(): mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0), mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{

	//���������ÿ����ٴΣ�������countsPerSec�з���
	//ע�⣬�˴�ΪQueryPerformanceFrequency����
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}


//��������ʵ��Tick������������������ÿ֡��ʱ��������mDeltaTime��
void GameTimer::Tick()
{
	if (mStopped)
	{
		//�����ǰ��ֹͣ״̬����֡���ʱ��Ϊ0
		mDeltaTime = 0.0;
		return;
	}

	//���㵱ǰʱ�̵ļ���ֵ
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	//���㵱ǰ֡��ǰһ֡��ʱ���(������*ÿ�ζ�����)
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	//׼�����㵱ǰ֡����һ֡��ʱ���
	mPrevTime = mCurrTime;

	//�ų�ʱ���Ϊ��ֵ
	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}



//������������������ͷ�ļ��е�public   ǰ����������������������������
//��󽫼���õ���mDeltaTime��װ�ɺ��������ⲿֻ�����á�
float GameTimer::DeltaTime()const
{
	return (float)mDeltaTime;
}



//Ȼ��ʵ��Reset���������ü�ʱ�����ⲽ����Ϣѭ��֮ǰ����ִ�У���ִֻ��һ�Σ�������һ����Ϸִֻ��һ�����á�
///////��Ӧ�ó�����������������mBaseTimeһ��ᱣ�ֲ���
void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;//��ǰʱ����Ϊ��׼ʱ��
	mPrevTime = currTime;//��ǰʱ����Ϊ��һ֡ʱ�䣬��Ϊ�����ˣ���ʱû����һ֡
	mStopTime = 0;       //����ֹͣ��һ��ʱ��Ϊ0
	mStopped = false;   ////ֹͣ״̬Ϊ��    ��ֹͣ
}


//������ʵ��Stop��Start�����������������ǿ�ʼ����ͣ��ʱ�����൱���޸���״̬�Ŀ��ء�Ϊ�˱�����⣬���ǽ����ڴ�����ͣ��״̬��Ϊֹͣ����֮ǰ��ͣ��״̬��Ϊ��ͣ����ϸ�뿴ע�͡�
void GameTimer::Stop()
{
	//���û��ֹͣ��������ֹͣ�����ֹͣ��ʲô��������
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;//����ǰʱ����Ϊֹͣ��һ�̵�ʱ��(����)
		mStopped = true;    //�޸�Ϊֹͣ״̬   ֹͣ
	}
}
void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);


	// Accumulate the time elapsed between stop and start pairs.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  mBaseTime       mStopTime        startTime     


	//�����ֹͣ״̬����������ֹͣ��������ͣʱ�䣬�޸�ֹͣ״̬

	if (mStopped)
	{
		//�ۼ�  ��ͣʱ��
		mPausedTime += (startTime - mStopTime);

		//�޸�ֹͣ״̬
		mPrevTime = startTime;//�൱��������һ֡ʱ��
		mStopTime = 0;        //�൱������ֹͣ��ʱ��
		mStopped = false;     //ֹͣ״̬Ϊ��    ��ֹͣ
	}

	//�������ֹͣ״̬����ʲô������
}




//������������������ͷ�ļ��е�public   ǰ����������������������������
//<<<<<<<<<<<<<<<<<<<<<<��Ա����>>>>>>>>>>>>>>>>>>>>>>>>>
// Returns the total time elapsed since Reset() was called
//Ȼ������ʵ����TotalTime���������������Ŀ���Ƿ����ܵ���ȥ��ʱ�������ú󣬵���������ͣʱ�䡣ͬ����Ϊ�˱�����⣬���ǽ����ڴ�����ͣ��״̬��Ϊֹͣ����֮ǰ��ͣ��״̬��Ϊ��ͣ��
float GameTimer::TotalTime()const
{
	// If we are stopped, do not count the time that has passed since we stopped.
	// Moreover, if we previously already had a pause, the distance 
	// mStopTime - mBaseTime includes paused time, which we do not want to count.
	// To correct this, we can subtract the paused time from mStopTime:  
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime

	//�����ʱ����ͣ״̬������ֹͣʱ�̵�ʱ��ȥ��֮ǰ��ͣ����ʱ��
	if( mStopped )
	{
		return (float)(((mStopTime - mPausedTime)-mBaseTime)*mSecondsPerCount);
	}

	// The distance mCurrTime - mBaseTime includes paused time,
	// which we do not want to count.  To correct this, we can subtract 
	// the paused time from mCurrTime:  
	//
	//  (mCurrTime - mPausedTime) - mBaseTime 
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mCurrTime
	
	//���������ͣ״̬�����õ�ǰʱ�̵�ʱ��ȥ����ͣ��ʱ��
	else
	{
		return (float)(((mCurrTime-mPausedTime)-mBaseTime)*mSecondsPerCount);
	}
}




//������������������ͷ�ļ��е�public   ǰ����������������������������
//������ǻ���װ�˻�ȡmStopped�����ĺ��������ⲿ���á�������WinMain.cpp��  int run();
bool GameTimer::MStopped()
{
	return  mStopped;
}







