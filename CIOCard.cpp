#include "CIOCard.h"
#include "clogfile.h"
#include <QSettings>
CIOCard::CIOCard(s_ConfigIOCardInfo sConfigIOCardInfo,int index)
{
	//弹窗
	m_bIsPopMessagebox = true;
	strMessageboxTittle = QObject::tr("IOCARD ERROR");		//相机错误
	//初始化文件路径
	m_bIsIOCardOK = 1;		//IO卡是否正常
	if (m_bIsIOCardOK)
	{
		//参数赋值初始化
		m_sConfigIOCardInfo = sConfigIOCardInfo;//sSystemInfo.m_sConfigIOCardInfo[index];	
	}
	m_vSignalQueue.clear();
	ReadQeueu();
}

CIOCard::~CIOCard()
{
	m_vSignalQueue.clear();
}


//初始化IO卡:初始化时不使能，开启检测时enable,也可通过它来控制是否踢废
s_IOCardErrorInfo CIOCard::InitIOCard()	
{
	s_IOCardErrorInfo sInfo;
	try
	{
		//初始化时也作此判断，判断配置文件中的是否初始化接口卡
		if (m_bIsIOCardOK)
		{
			if(m_Pio24b.openCard(m_sConfigIOCardInfo.iCardID))
			{
				m_Pio24b.hardReset();
				m_Pio24b.initRegisters(m_sConfigIOCardInfo.strCardInitFile.toLatin1().data(), m_sConfigIOCardInfo.strCardName.toLatin1().data());
				m_Pio24b.softReset();
				//设置补踢默认勾选
				m_Pio24b.enable(false);// true->false [11/11/2010 zhaodt]
				m_bIsIOCardOK = true;
			}
			else
			{
				m_Pio24b.initRegisters(m_sConfigIOCardInfo.strCardInitFile.toLatin1().data(), m_sConfigIOCardInfo.strCardName.toLatin1().data());
				m_bIsIOCardOK = false;
				sInfo.bResult = false;
				sInfo.strSummaryInfo = QObject::tr("Failed to initialize IOCard%1").arg(m_sConfigIOCardInfo.iCardID+1);//IO卡%1初始化失败 !
				sInfo.strDetailedInfo = sInfo.strSummaryInfo;
				if (m_bIsPopMessagebox)
				{
					s_MSGBoxInfo msginfo;
					msginfo.strMsgInfo = sInfo.strSummaryInfo;
					msginfo.strMsgtitle = strMessageboxTittle;
					emit emitMessageBoxMainThread(msginfo);
				}
				else
				{
					qDebug(sInfo.strSummaryInfo.toLatin1().data());
				}
			}
			////一体要初始化两块接口卡
			//if(m_eSystemType == 2 || m_eSystemType == 3 || m_eSystemType == 5)
			//{
			//	if(m_pio24b1.openCard(1))
			//	{
			//		m_pio24b1.hardReset();
			//		m_pio24b1.initRegisters("\\PIO24B_reg_init1.txt", "PIO24B");
			//		m_pio24b1.softReset();
			//		m_pio24b1.enable(false);// true->false [11/11/2010 zhaodt]
			//	}
			//	else
			//	{
			//		m_bIsIOCardOK = FALSE;
			//		CString strValue;
			//		strValue.LoadString(ID_MSG6);
			//		AfxMessageBox(strValue) ;

			//		//CString strInfo;
			//		CString strError,strInfo;
			//		strError.LoadString(IDS_LOG_ERRORINFO);
			//		strInfo.LoadString(IDS_LOG_INITCARD1ERROR);
			//		//strInfo.Format(tr("IO卡1初始化失败!"));
			//		m_LogManager.MyWriteLogFile(strError,strInfo,OperationLog);
			//	}
			//}
		}
	}
	catch (...)
	{
		m_bIsIOCardOK = false;
		sInfo.bResult = false;
		sInfo.strSummaryInfo = QObject::tr("Initialize the IOCard%1 abnormal!").arg(m_sConfigIOCardInfo.iCardID+1);//IO卡%1初始化异常!
		sInfo.strDetailedInfo = sInfo.strSummaryInfo;
		if (m_bIsPopMessagebox)
		{
			s_MSGBoxInfo msginfo;
			msginfo.strMsgInfo = sInfo.strSummaryInfo;
			msginfo.strMsgtitle = strMessageboxTittle;
			emit emitMessageBoxMainThread(msginfo);
		}
		else
		{
			qDebug(sInfo.strSummaryInfo.toLatin1().data());
		}
	}
	return sInfo;
}

//释放IO卡
s_IOCardErrorInfo CIOCard::CloseIOCard()
{
	s_IOCardErrorInfo sInfo;
	try
	{
		//停止IO卡 [11/11/2010 zhaodt]
		if (m_bIsIOCardOK)
		{
			m_bIsIOCardOK = false;
			m_mutexmIOCard.lock();
			m_Pio24b.softReset();
			m_Pio24b.enable(false);
			m_mutexmIOCard.unlock();
			qDebug("---接口卡-%d--Stop--\n",m_sConfigIOCardInfo.iCardID+1);
		}
	}
	catch (...)
	{
		sInfo.bResult = false;
		sInfo.strSummaryInfo = QObject::tr("Close the IOCard%1 abnormal!!").arg(m_sConfigIOCardInfo.iCardID+1);//关闭IO卡%1异常!
		sInfo.strDetailedInfo = sInfo.strSummaryInfo;
		if (m_bIsPopMessagebox)
		{
			s_MSGBoxInfo msginfo;
			msginfo.strMsgInfo = sInfo.strSummaryInfo;
			msginfo.strMsgtitle = strMessageboxTittle;
			emit emitMessageBoxMainThread(msginfo);
		}
		else
		{
			qDebug(strMessageboxTittle.toLatin1().data());
		}
	}
	return sInfo;
}

//读取队列--乱的，先随便写一组，需修改为所有队列
void CIOCard::ReadQeueu()
{
	s_IOSignal sSignal;
	sSignal.iGrabCount = 0;
	sSignal.iGrabDelay = 0;
	QSettings SignalQueue(m_sConfigIOCardInfo.strCardInitFile,QSettings::IniFormat);//GrabInfo.ini
	//采集参数-iniGrabInfo
	int iText = SignalQueue.value(QString("/PIO24B/18"),0xffffffff).toInt();
	iText = iText % 16;


//	m_vSignalQueue.push_back(sSignal);
}

//读取图像号
int CIOCard::ReadImageSignal(int iStation)
{
	int iSignalCount = -1;
	if (m_bIsIOCardOK)
	{
		//m_mutexmIOCard.lock();
		iSignalCount = m_Pio24b.readCounter(iStation)&0xFF; //获取图像号
		//m_mutexmIOCard.unlock();
	}
	return iSignalCount;
}
int CIOCard::ReadCounter(int param)
{
	int iSignalCount = -1;
	if (m_bIsIOCardOK)
	{
		iSignalCount = m_Pio24b.readCounter(param); //获取图像号
	}
	return iSignalCount;
}

void CIOCard::SendResult(s_ResultInfo sResultInfo)
{
	//两块接口卡同时发结果报异常？在外面互斥。
// 	m_mutexmIOCard.lock();
  	m_Pio24b.sendChckRslt(sResultInfo.tmpResult,sResultInfo.nImgNo,0);
//	m_mutexmIOCard.unlock();

}
void CIOCard::GetSample(s_ResultInfo sResultInfo)
{
	//两块接口卡同时发结果报异常？在外面互斥。
//	m_mutexmIOCard.lock();
	m_Pio24b.sendChckRslt(sResultInfo.tmpResult,sResultInfo.nImgNo,3);									
//	m_mutexmIOCard.unlock();
}

unsigned short CIOCard::readParam(int paramIndex)
{
	return m_Pio24b.readParam(paramIndex);
}

void CIOCard::writeParam(int paramIndex, unsigned short data)
{
	m_Pio24b.writeParam(paramIndex, data);
}

unsigned short CIOCard::readStatus(int counterIndex)
{
	return m_Pio24b.readStatus(counterIndex);
}

int CIOCard::Show_PIO24B_DebugDialog(void *pParentWnd)
{
	int i = m_Pio24b.Show_PIO24B_DebugDialog(pParentWnd);
	return i;
}

void CIOCard::enable(bool bSign)
{
	m_mutexmIOCard.lock();
	if (bSign == true)
	{
		m_Pio24b.softReset();
	}
	m_Pio24b.enable(bSign);
	m_mutexmIOCard.unlock();

}
void CIOCard::TestOutPut(int ComNo)
{
	m_mutexmIOCard.lock();
	m_Pio24b.setCardOutput(ComNo,1);
	Sleep(20);
	m_Pio24b.setCardOutput(ComNo,0);
	m_mutexmIOCard.unlock();

}
void CIOCard::SetOutHighLevel(int ComNo)
{
	m_mutexmIOCard.lock();
	m_Pio24b.setCardOutput(ComNo,1);
	m_mutexmIOCard.unlock();

}
void CIOCard::SetOutLowLevel(int ComNo)
{
	m_mutexmIOCard.lock();
	m_Pio24b.setCardOutput(ComNo,0);
	m_mutexmIOCard.unlock();

}

