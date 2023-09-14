#pragma once

#include <QObject>

#pragma once

#include "CoreStorageGlobal.h"

#include <QString>
#include <QObject>
#include <QVariant>
#include <QMutex>

namespace CoreStorage
{
	//-----------------------------------------------------------------------------
	// Author:  Tobias Post
	// Company: CUBE4DEV GmbH
	// Date:    20.10.2022
	// Context: DefaultNamespace
	// Class:   XConfig
	// Method:  XConfig
	// Description:  
	//	The XConfig class is responsible for reading and writing configuration
	//	values in files. It uses the QMap class to represent the data in multiple
	//	nested levels, and it makes use of caching mechanisms and mutex locks to
	//	ensure thread safety in multiple concurrent access scenarios. The class
	//	provides methods to set and get values, as well as to retrieve all
	//	values for a given group in a file.
	//----------------------------------------------------------------------------- 
	class CORESTORAGE_EXPORT XConfig
	{
	public:
		XConfig(const QString& filename = QString(""), const QString& group=QString(""));
		virtual ~XConfig();

		void setFileName(const QString& filename) { m_FileName = filename; }
		void setGroup(const QString& group) { m_Group = group; };

		void setValue(const QString& name, const QVariant& value);
		QVariant value(const QString& name);

		static void setValue(const QString& filename, const QString& group, const QString& name, const QVariant& value);
		static QVariant value(const QString& filename, const QString& group, const QString& name);
		static QStringList values(const QString& filename, const QString& group);
		static bool hasValue(const QString& filename, const QString& group, const QString& name);

		static void setValue(const QString& group, const QString& name, const QVariant& value)
		{
			setValue("app.cfg",group, name, value);
		}
		static QVariant value(const QString& group, const QString& name)
		{
			return value("app.cfg", group, name);
		}
	private:
		
		QString m_FileName;
		QString m_Group;

		static QMutex m_Mutex;
		static QCache<QString, QMap<QString, QMap<QString, QVariant>> > m_DocumentCache;
		static QMap<QString, QDateTime> m_DocumentChanged;
	};

};
using namespace CoreStorage;