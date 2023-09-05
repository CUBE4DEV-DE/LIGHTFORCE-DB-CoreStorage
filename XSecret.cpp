#include "XSecret.h"

#include <QCryptographicHash>

#include "XFile.h"

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XSecret
// Method:  XSecret
// Params:  QObject* parent
//----------------------------------------------------------------------------- 
XSecret::XSecret(QObject *parent): QObject(parent)
{
	
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XSecret
// Method:  ~XSecret
//----------------------------------------------------------------------------- 
XSecret::~XSecret()
{
	
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XSecret
// Method:  setPath
// Params:  const QString& datapath
//----------------------------------------------------------------------------- 
void XSecret::setPath(const QString& datapath)
{
	m_DataPath = datapath;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XSecret
// Method:  setCredential
// Params:  const QString& name
// Params:  const QString& password
//----------------------------------------------------------------------------- 
void XSecret::setCredential(const QString& name, const QString& password)
{
	QString passwordmd5 = QCryptographicHash::hash(password.toLatin1(), QCryptographicHash::Algorithm::Md5);
	m_Credential[name] = passwordmd5;

	if (!m_DataPath.isEmpty())
	{
		XFile::write(m_DataPath + QDir::separator() + name + ".sec", passwordmd5);
	}
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XSecret
// Method:  isCredential
// Params:  const QString& user
// Params:  const QString& password
//----------------------------------------------------------------------------- 
bool XSecret::isCredential(const QString& name, const QString& password)
{
	if (password.isEmpty())
		return false;

	QString passwordmd5 = QCryptographicHash::hash(password.toLatin1(), QCryptographicHash::Algorithm::Md5);

	if (m_Credential.contains(name)&&m_Credential[name]==passwordmd5)
		return true;

	if (!m_DataPath.isEmpty())
	{
		QString filepasswordmd5=XFile::read(m_DataPath + QDir::separator() + name + ".sec");
		if (filepasswordmd5.isEmpty())
			return false;

		if (filepasswordmd5 == passwordmd5)
			return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XSecret
// Method:  credentialKeyword
// Params:  const QString& user
// Params:  const QString& password
// Params:  
//----------------------------------------------------------------------------- 
QString XSecret::credentialKeyword(const QString& name, const QString& password)
{
	QString namemd5 = QCryptographicHash::hash(name.toLatin1(), QCryptographicHash::Algorithm::Md5);
	QString passwordmd5 = QCryptographicHash::hash(password.toLatin1(), QCryptographicHash::Algorithm::Md5);

	return QByteArray((namemd5+passwordmd5).toUtf8()).toBase64();
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XSecret
// Method:  setCredential
// Params:  const QString& datapath
// Params:  const QString& name
// Params:  const QString& password
//----------------------------------------------------------------------------- 
void XSecret::setCredential(const QString& datapath, const QString& name, const QString& password)
{
	XSecret secret;
	secret.setPath(datapath);
	secret.setCredential(name, password);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XSecret
// Method:  isCredential
// Params:  const QString& datapath
// Params:  const QString& name
// Params:  const QString& password
// Params:  
//----------------------------------------------------------------------------- 
bool XSecret::isCredential(const QString& datapath, const QString& name, const QString& password)
{
	XSecret secret;
	secret.setPath(datapath);
	return secret.isCredential(name, password);

}
