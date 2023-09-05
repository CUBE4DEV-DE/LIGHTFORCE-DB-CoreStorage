#pragma once

#include "CoreStorageGlobal.h"

#include <QString>
#include <QStringList>
#include <QObject>

namespace CoreStorage
{
	//-----------------------------------------------------------------------------
	// Author:  Tobias Post
	// Company: CUBE4DEV GmbH
	// Date:    02.10.2022
	// Context: DefaultNamespace
	// Class:   XBackup
	// Method:  XBackup
	//----------------------------------------------------------------------------- 
	class CORESTORAGE_EXPORT XRepository :public QObject
	{
		Q_OBJECT
	public:
		XRepository();
		virtual ~XRepository();

		void setPath(const QString& datapath, const QString& repositorypath);
		void setRepositoryPath(const QString& path);
		void setDataPath(const QString& path);

		void includeFilter(const QStringList& filter) { m_IncludeFilter << filter; }
		void includeFilter(const QString& filter) { m_IncludeFilter << filter; }
		
	public slots:
		void wait();
		void version(const QString& rootversion, const QString& subversion, bool run = false);
		bool rollback(const QString& rootversion,const QString& subversion);
	private:

		const QString& repositoryPath() { return m_RepositoryPath; }
		const QString& dataPath() { return m_DataPath; }

		QString m_RepositoryPath;
		QString m_DataPath;

		QStringList m_IncludeFilter;
	};

};
using namespace CoreStorage;