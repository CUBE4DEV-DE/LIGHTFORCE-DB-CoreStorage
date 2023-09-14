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
	// Description: The methods in the XRepository class serve to manage the path and
	//	versioning of data stored within the repository.
	//	The setPath() method is used to set the paths of the data and repository directories.
	// 	The setRepositoryPath() method specifically sets the path to the repository directory.
	// 	The setDataPath() method sets the path to the data directory.
	// 	The version() method creates a version of the data stored within the repository.
	// 	It provides an historical record of past iterations of the data for
	// 	retrieval.
	// 	The wait() method waits for all pending directory and file maintenance actions to complete.
	//	rollback() offers a way to undo changes made to the repository, reverting the data to a previous version.
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