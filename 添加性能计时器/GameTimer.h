//***************************************************************************************
// GameTimer.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#ifndef GAMETIMER_H
#define GAMETIMER_H


//����GameTimer�࣬ ��Ϊ.h�ļ�


class GameTimer
{
public:
	GameTimer();

	float TotalTime()const; //��Ϸ���е���ʱ�䣨��������ͣ��
	float DeltaTime()const; //��ȡmDeltaTime����
	bool MStopped();	//��ȡmStopped����


	void Reset(); //���ü�ʱ��
	void Start(); //��ʼ��ʱ��.
	void Stop();  //ֹͣ��ʱ��
	void Tick();  //����֡��֮֡��ʱ����


private:
	double mSecondsPerCount;//������ÿһ����Ҫ������
	double mDeltaTime;//ÿ֡ʱ�䣨ǰһ֡�͵�ǰ֡��ʱ��

	__int64 mBaseTime;//���ú�Ļ�׼ʱ��
	__int64 mPausedTime;//��ͣ����ʱ��
	__int64 mStopTime;//ֹͣ��һ�̵�ʱ��
	__int64 mPrevTime;//��һ֡ʱ��
	__int64 mCurrTime;//��֡ʱ��


	  bool mStopped;
	//bool isStoped;		//�Ƿ�Ϊֹͣ��״̬
};

#endif // GAMETIMER_H