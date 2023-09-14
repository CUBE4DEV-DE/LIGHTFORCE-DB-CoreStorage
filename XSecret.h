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
// Description: The XSecret class provides functionality for storing and validating
//	credentials(usernames and associated passwords).It achieves this by
//	internally using the QCryptographicHash and XFile classes.This class
//	supports setting the datapath, setting credentials, validating credentials,
//	and generating a keyword from the input credentials.Essentially,
//	the XSecret class is a tool for securely storing and finding usernames and
//	their matching passwords.
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