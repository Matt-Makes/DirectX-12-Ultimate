//***************************************************************************************
// GameTimer.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include <windows.h>
#include "GameTimer.h"
////创建GameTimer类， 此为.cpp文件




//将GameTimer类封装完毕，让它可以控制计时器的开关，可以计算帧与帧的时间间隔，还可以计算游戏运行总时间。
//之后在主文件（WinMain.cpp)创建一个GameTimer实例 : GameTimer gt;
//GameTimer为类  gt为实例




//<<<<<<<<<<<<<<<<<<<<<<构造函数>>>>>>>>>>>>>>>>>>>>>>>>>
//然后在cpp文件中我们挨个实现。首先是构造函数，构造函数会查询性能计数器的频率(一次多少秒)。核心函数QueryPerformanceCounter返回当前时间的计数器次数值，即mSencondsPerCount。这个值乘以次数便是时间。注意，传入其的参数，系统会用时间值自动赋值并随着运行时间增长而增长，从而改变输出时的次数值，所以输入的参数并不需要自己初始化。
GameTimer::GameTimer(): mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0), mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{

	//计算计数器每秒多少次，并存入countsPerSec中返回
	//注意，此处为QueryPerformanceFrequency函数
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}


//接着我们实现Tick函数，它是用来计算每帧的时间间隔，即mDeltaTime。
void GameTimer::Tick()
{
	if (mStopped)
	{
		//如果当前是停止状态，则帧间隔时间为0
		mDeltaTime = 0.0;
		return;
	}

	//计算当前时刻的计数值
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	//计算当前帧和前一帧的时间差(计数差*每次多少秒)
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	//准备计算当前帧和下一帧的时间差
	mPrevTime = mCurrTime;

	//排除时间差为负值
	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}



//《《《《《《《《《头文件中的public   前三个》》》》》》》》》》》》
//随后将计算得到的mDeltaTime封装成函数，让外部只读调用。
float GameTimer::DeltaTime()const
{
	return (float)mDeltaTime;
}



//然后实现Reset函数，重置计时器。这步在消息循环之前必须执行，且只执行一次，即运行一次游戏只执行一次重置。
///////在应用程序的整个生命周期里，mBaseTime一般会保持不变
void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;//当前时间作为基准时间
	mPrevTime = currTime;//当前时间作为上一帧时间，因为重置了，此时没有上一帧
	mStopTime = 0;       //重置停止那一刻时间为0
	mStopped = false;   ////停止状态为假    不停止
}


//接下来实现Stop和Start函数，这两个函数是开始和暂停计时器，相当于修改其状态的开关。为了便于理解，我们将现在处于暂停的状态称为停止，将之前暂停的状态称为暂停。详细请看注释。
void GameTimer::Stop()
{
	//如果没有停止，则让其停止（如果停止则什么都不做）
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;//将当前时间作为停止那一刻的时间(次数)
		mStopped = true;    //修改为停止状态   停止
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


	//如果是停止状态，则让其解除停止，保存暂停时间，修改停止状态

	if (mStopped)
	{
		//累加  暂停时间
		mPausedTime += (startTime - mStopTime);

		//修改停止状态
		mPrevTime = startTime;//相当于重置上一帧时刻
		mStopTime = 0;        //相当于重置停止的时刻
		mStopped = false;     //停止状态为假    不停止
	}

	//如果不是停止状态，则什么都不做
}




//《《《《《《《《《头文件中的public   前三个》》》》》》》》》》》》
//<<<<<<<<<<<<<<<<<<<<<<成员函数>>>>>>>>>>>>>>>>>>>>>>>>>
// Returns the total time elapsed since Reset() was called
//然后我们实现了TotalTime函数，这个函数的目的是返回总的逝去的时间自重置后，但不包括暂停时间。同样，为了便于理解，我们将现在处于暂停的状态称为停止，将之前暂停的状态称为暂停。
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

	//如果此时在暂停状态，则用停止时刻的时间去减之前暂停的总时间
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
	
	//如果不在暂停状态，则用当前时刻的时间去减暂停总时间
	else
	{
		return (float)(((mCurrTime-mPausedTime)-mBaseTime)*mSecondsPerCount);
	}
}




//《《《《《《《《《头文件中的public   前三个》》》》》》》》》》》》
//最后我们还封装了获取mStopped变量的函数，让外部调用。服务于WinMain.cpp的  int run();
bool GameTimer::MStopped()
{
	return  mStopped;
}







