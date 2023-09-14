#pragma once

#include "CoreStorageGlobal.h"

#include <QString>
#include <QStringList>
#include <QObject>
#include <QTimer>

namespace CoreStorage
{
	//-----------------------------------------------------------------------------
	// Author:  Tobias Post
	// Company: CUBE4DEV GmbH
	// Date:    02.10.2022
	// Context: DefaultNamespace
	// Class:   XExpire
	// Method:  XExpire
	// Description: The purpose of the XExpire class is to manage the expiry
	//	of data saved to disk. It manages the path in which the data is stored
	//	as well as flushes any data that exceeds a specified time limit. The
	//	time limit and the checking interval are configurable, allowing for
	//	effective customization of expiry policies. The class also has a
	//	wait method that waits for any unfinished file or directory moves.
	//----------------------------------------------------------------------------- 
	class CORESTORAGE_EXPORT XExpire :public QObject
	{
		Q_OBJECT
	public:
		XExpire();
		virtual ~XExpire();

		void setPath(const QString& datapath,qint32 maxminutes=60*24, int seconds = 60 * 5);
		void setMaxMinutes(qint32 maxminutes) { m_MaxMinutes = maxminutes;  }
	private slots:
		void timer();
	public slots:
		void flush();
		void wait();
		void flush(const QDateTime& datetime, bool run = false);
	private:
		const QString& dataPath() { return m_DataPath; }
		QString m_DataPath;
		QTimer m_Timer;
		qint32 m_MaxMinutes;
	};


};
using namespace CoreStorage;