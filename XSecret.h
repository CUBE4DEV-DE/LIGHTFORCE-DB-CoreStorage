#pragma once

#include <QObject>

#pragma once

#include "CoreStorageGlobal.h"

#include <QString>
#include <QStringList>
#include <QObject>
#include <QMap>

namespace CoreStorage
{
//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XSecret
// Method:  XSecret
//----------------------------------------------------------------------------- 
class CORESTORAGE_EXPORT XSecret:public QObject
{
	Q_OBJECT
public:
	XSecret(QObject *parent=Q_NULLPTR);
	virtual ~XSecret();
	void setPath(const QString& datapath);

	void setCredential(const QString& name, const QString& password);
	bool isCredential(const QString& name, const QString& password);
	static QString credentialKeyword(const QString& name, const QString& password);

	static void setCredential(const QString& datapath,const QString& name, const QString& password);
	static bool isCredential(const QString& datapath, const QString& name, const QString& password);
	
private:
	const QString& dataPath() { return m_DataPath; }
	QString m_DataPath;
	QMap<QString,QString> m_Credential;
};

};
using namespace CoreStorage;