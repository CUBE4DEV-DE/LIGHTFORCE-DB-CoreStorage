#pragma once

// The overall purpose of the file "XStream.h", is to provide a class called XStream,
// that allows developers to handle data streams and paths. The class has methods
// that set the paths of the data and the stream and manage flushing and waiting
// of data. The class also has a timer, which can be used to check periodically
// whether there is any data to be flushed from the data path to the stream path.

// Overall, XStream offers developers an easy way to handle data streams and
// paths, making it easier to manage I / O operations for their applications.

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
	// Class:   XStream
	// Method:  XStream
	// Description: The XStream class is defined in the file "XStream.h"
	//  within the "CoreStorage" namespace. It is designed to help
	//  developers manage data streams and paths within their applications.
	//  The class can be used to set paths for data and stream locations,
	//  as well as manage flushing and waiting of data.
	//  The class also includes a timer that can be used to periodically check
	//  for data to be flushed from the data path to the stream path.Additionally,
	//  the class provides methods for flushing and waiting for data,
	//  as well as setting the timers.The XStream class is a useful tool
	//  for developers who are working on I / O operations.
	//----------------------------------------------------------------------------- 
	class CORESTORAGE_EXPORT XStream :public QObject
	{
		Q_OBJECT
	public:
		XStream();
		virtual ~XStream();

		void setPath(const QString& datapath, const QString& streampath, int seconds = 60 * 5);
		void setStreamPath(const QString& path) { m_StreamPath = path; }
		void setDataPath(const QString& path);

	private slots:
		void timer();
	public slots:
		void flush();
		void wait();
		void flush(const QDateTime& datetime, bool run = false);
		//bool rollback(const QDateTime& rollbackdatetime);
	private:

		const QString& streamPath() { return m_StreamPath; }
		const QString& dataPath() { return m_DataPath; }

		QString m_StreamPath;
		QString m_DataPath;
		QTimer m_Timer;
	};


};
using namespace CoreStorage;