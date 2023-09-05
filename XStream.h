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
	// Class:   XStream
	// Method:  XStream
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