#include "XStream.h"
#include <QDir>

#include "XDirectory.h"
#include "XFile.h"
//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XStream
// Method:  XStream
//----------------------------------------------------------------------------- 

XStream::XStream()
{
	QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(timer()));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XStream
// Method:  ~XStream
//----------------------------------------------------------------------------- 

XStream::~XStream()
{
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XStream
// Method:  setPath
// Params:  const QString& datapath
// Params:  const QString& backuppath
//----------------------------------------------------------------------------- 

void XStream::setPath(const QString& datapath, const QString& streampath, int seconds)
{
	setStreamPath(streampath);
	setDataPath(datapath);

	if (seconds > 0)
	{
		m_Timer.setInterval(seconds * 1000);
		m_Timer.start();
	}
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XStream
// Method:  setDataPath
// Params:  const QString& path
//----------------------------------------------------------------------------- 
void XStream::setDataPath(const QString& path)
{
	if (m_DataPath != path)
	{
		m_DataPath = path;
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XStream
// Method:  timer
//----------------------------------------------------------------------------- 
void XStream::timer()
{
	flush();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    03.10.2022
// Context: DefaultNamespace
// Class:   XStream
// Method:  timestamp
// Params:  const QDateTime& datetime
//----------------------------------------------------------------------------- 

void XStream::flush(const QDateTime& datetime, bool run)
{
	QString dailybackup = QString("Log[%1]").arg(datetime.toString("yyyyMMdd"));
	if (!QDir(streamPath()).exists(dailybackup))
	{
		XDirectory::copy(dataPath(), streamPath() + QDir::separator() + dailybackup + QDir::separator() + "Start", run);
	}

	// FileBased Diff
	QString diffbackup = dailybackup + QDir::separator() + QString("Stamp[%1]").arg(datetime.toString("HHmmss"));
	if (!QDir(streamPath()).exists(diffbackup))
	{
		QStringList backuppaths;
		for (const QString dirname : XDirectory::dirs(streamPath() + QDir::separator() + dailybackup))
		{
			if (dirname.startsWith("Start") || dirname.startsWith("Stamp"))
				backuppaths << streamPath() + QDir::separator() + dailybackup + QDir::separator() + dirname;
		}
		XDirectory::diff(dataPath(), backuppaths, streamPath() + QDir::separator() + diffbackup, run);
		XDirectory::remove(dataPath(), run);
	}

}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XStream
// Method:  timestamp
//----------------------------------------------------------------------------- 

void XStream::flush()
{
	QDateTime datetime(QDateTime::currentDateTime());
	flush(datetime, true);
	XFile::wait();
	XDirectory::wait();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    18.10.2022
// Context: DefaultNamespace
// Class:   XStream
// Method:  wait
//----------------------------------------------------------------------------- 

void XStream::wait()
{
	XFile::wait();
	XDirectory::wait();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    02.10.2022
// Context: DefaultNamespace
// Class:   XStream
// Method:  rollback
// Params:  QDateTime rollbackdatetime
//----------------------------------------------------------------------------- 

//bool XStream::rollback(const QDateTime& rollbackdatetime)
//{
//	QString fullrollbackup = QString("Log[%1]").arg(rollbackdatetime.toString("yyyyMMdd"));
//	if (!QDir(streamPath()).exists(fullrollbackup))
//		return false;
//
//	XDirectory::remove(dataPath(), false);
//
//	QStringList backuppaths;
//	for (const QString dirname : XDirectory::dirs(streamPath() + QDir::separator() + fullrollbackup))
//	{
//		if (dirname.startsWith("Start") || dirname.startsWith("Stamp"))
//			backuppaths << streamPath() + QDir::separator() + fullrollbackup + QDir::separator() + dirname;
//	}
//	XDirectory::merge(backuppaths, dataPath(), false);
//
//	return true;
//}
//
