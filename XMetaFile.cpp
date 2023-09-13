#include "XMetaFile.h"
#include "XFile.h"

qint32 XMetaFile::m_MaxTouch=1024;

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  access
// Params:  const QString& filename
// Params:  const QString& user
// Params:  const QString& group
// Params:  const QString& role
// Description:
//	This method checks if a specific user, group, and role have access to
//	the given file. The method takes a filename string, user string,
//	group string, role string, and password string as parameters.
//	It returns a boolean value indicating whether the user has read or write access.
//----------------------------------------------------------------------------- 
bool XMetaFile::access(const QString& filename, const QString& user, const QString& group, const QString& role, const QString& password)
{
	return XMetaFile::access(filename, QStringList() << user, QStringList() << group, QStringList() << role,QStringList()<<password);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  access
// Params:  const QString& filename
// Params:  const QStringList& users
// Params:  const QStringList& groups
// Params:  const QStringList& roles
// Description:
//	This method is similar to the previous one, but it receives the
//	lists of users, groups and roles instead.It takes the same parameters
//	as before, as well as the passwords string list.It returns a boolean
//	value indicating whether the user has read or write access.
//----------------------------------------------------------------------------- 
bool XMetaFile::access(const QString& filename, const QStringList& users, const QStringList& groups,const QStringList& roles, const QStringList& passwords)
{
	QMap<QString,QStringList> map= XFile::read<QMap<QString, QStringList>>(fileNameAccess(filename));

	if (!users.isEmpty()) map["users"] << users;
	if (!groups.isEmpty()) map["groups"] << groups;
	if (!roles.isEmpty()) map["roles"] << roles;
	if (!passwords.isEmpty())
	{
		QStringList passwordsmd5;
		for (const QString& password : passwords)
			passwordsmd5 << QString(QCryptographicHash::hash(password.toLatin1(), QCryptographicHash::Algorithm::Md5));
		map["passwords"] << passwordsmd5;
	}
	
	return XFile::write<QMap<QString, QStringList>>(fileNameAccess(filename), map, true);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  isAccess
// Params:  const QString& filename
// Params:  const QString& user
// Params:  const QString& group
// Params:  const QString& role
// Description:
// This method checks if a specific user, group, and role have read access
// to the given file.The method takes a filename string, user string,
// group string, role string, and password string as parameters.
// It returns a boolean value indicating whether the user has read access.
//----------------------------------------------------------------------------- 
bool XMetaFile::isAccess(const QString& filename, const QString& user, const QString& group, const QString& role, const QString& password)
{
	if (!XFile::exists(filename + ".acc.meta"))
		return true;

	QMap<QString, QStringList> map= XFile::readCache<QMap<QString, QStringList>>(fileNameAccess(filename));

	QStringList passwords= map["passwords"];
	// Password Check
	if (!passwords.isEmpty())
	{
		if (!passwords.contains(QString(QCryptographicHash::hash(password.toLatin1(), QCryptographicHash::Algorithm::Md5)), Qt::CaseSensitive))
			return false;
	}
	// User Or Group Or Role
	QStringList users = map["users"];
	if (users.contains(user, Qt::CaseInsensitive)||users.contains(user+"+", Qt::CaseInsensitive)||users.contains(user + "-", Qt::CaseInsensitive))
		return true;

	QStringList groups = map["groups"];
	if (groups.contains(group, Qt::CaseInsensitive)||groups.contains(group+"+", Qt::CaseInsensitive) || groups.contains(group+"-", Qt::CaseInsensitive))
		return true;
	QStringList roles = map["roles"];
	if (roles.contains(role, Qt::CaseInsensitive)|| roles.contains(role+"+", Qt::CaseInsensitive) || roles.contains(role+"-", Qt::CaseInsensitive))
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  isAccessRead
// Params:  const QString& filename
// Params:  const QString& user
// Params:  const QString& group
// Params:  const QString& role
// Params:  const QString& password
// Description:
//	This method is similar to the previous one, but it only checks if the
//	user has read access to the file.It receives the same parameters
//	as before.It returns a boolean value indicating whether the
//	user has read access.
//----------------------------------------------------------------------------- 
bool XMetaFile::isAccessRead(const QString& filename, const QString& user, const QString& group, const QString& role,const QString& password)
{
	if (!XFile::exists(filename + ".acc.meta"))
		return true;

	QMap<QString, QStringList> map = XFile::readCache<QMap<QString, QStringList>>(fileNameAccess(filename));

	QStringList passwords = map["passwords"];
	// Password Check
	if (!passwords.isEmpty())
	{
		if (!passwords.contains(QString(QCryptographicHash::hash(password.toLatin1(), QCryptographicHash::Algorithm::Md5)), Qt::CaseSensitive))
			return false;
	}
	// User Or Group Or Role
	QStringList users = map["users"];
	if (users.contains(user, Qt::CaseInsensitive) || users.contains(user + "-", Qt::CaseInsensitive))
		return true;

	QStringList groups = map["groups"];
	if (groups.contains(group, Qt::CaseInsensitive) || groups.contains(group + "-", Qt::CaseInsensitive))
		return true;
	QStringList roles = map["roles"];
	if (roles.contains(role, Qt::CaseInsensitive) || roles.contains(role + "-", Qt::CaseInsensitive))
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  isAccessWrite
// Params:  const QString& filename
// Params:  const QString& user
// Params:  const QString& group
// Params:  const QString& role
// Params:  const QString& password
// Description:
//	This method checks if a specific user, group, and role have write access
//	to the given file.The method takes a filename string, user string,
//	group string, role string, and password string as parameters.
//	It returns a boolean value indicating whether the user has write access.
//----------------------------------------------------------------------------- 
bool XMetaFile::isAccessWrite(const QString& filename, const QString& user, const QString& group, const QString& role,const QString& password)
{
	if (!XFile::exists(filename + ".acc.meta"))
		return true;

	QMap<QString, QStringList> map = XFile::readCache<QMap<QString, QStringList>>(fileNameAccess(filename));

	QStringList passwords = map["passwords"];
	// Password Check
	if (!passwords.isEmpty())
	{
		if (!passwords.contains(QString(QCryptographicHash::hash(password.toLatin1(), QCryptographicHash::Algorithm::Md5)), Qt::CaseSensitive))
			return false;
	}
	// User Or Group Or Role
	QStringList users = map["users"];
	if (users.contains(user, Qt::CaseInsensitive) || users.contains(user + "+", Qt::CaseInsensitive))
		return true;
	QStringList groups = map["groups"];
	if (groups.contains(group, Qt::CaseInsensitive) || groups.contains(group + "+", Qt::CaseInsensitive))
		return true;
	QStringList roles = map["roles"];
	if (roles.contains(role, Qt::CaseInsensitive) || roles.contains(role + "+", Qt::CaseInsensitive))
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  accessUser
// Params:  const QString& filename
// Params:  const QString& user
// Params:  bool readonly
//----------------------------------------------------------------------------- 
bool XMetaFile::accessUser(const QString& filename, const QString& user, bool readonly)
{
	return accessProp(filename, "users", readonly? user+'-':user);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  accessGroup
// Params:  const QString& filename
// Params:  const QString& group
// Params:  bool readonly
//----------------------------------------------------------------------------- 
bool XMetaFile::accessGroup(const QString& filename, const QString& group, bool readonly)
{
	return accessProp(filename, "groups", readonly ? group+ '-' : group);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  accessRole
// Params:  const QString& filename
// Params:  const QString& role
// Params:  bool readonly
//----------------------------------------------------------------------------- 
bool XMetaFile::accessRole(const QString& filename, const QString& role, bool readonly)
{
	return accessProp(filename, "roles", readonly ? role+ '-' : role);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  accessPassword
// Params:  const QString& filename
// Params:  const QString& password
// Params:  bool readonly
//----------------------------------------------------------------------------- 
bool XMetaFile::accessPassword(const QString& filename, const QString& password, bool readonly)
{
	return accessProp(filename, "passwords", readonly ? password+ '-' : password);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    21.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  accessProp
// Params:  const QString& filename
// Params:  const QString& prop
// Params:  const QString& value
//----------------------------------------------------------------------------- 
bool XMetaFile::accessProp(const QString& filename, const QString& prop, const QString& value)
{
	QMap<QString, QStringList> map = XFile::read<QMap<QString, QStringList>>(fileNameAccess(filename));
	map[prop] << value;
	if (prop=="passwords")
	{
		map["passwords"] << QString(QCryptographicHash::hash(value.toLatin1(), QCryptographicHash::Algorithm::Md5));
	}
	return XFile::write<QMap<QString, QStringList>>(fileNameAccess(filename), map, true);
}


//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  isAccessUser
// Params:  const QString& filename
// Params:  const QString& user
//----------------------------------------------------------------------------- 
bool XMetaFile::isAccessUser(const QString& filename, const QString& user, bool readonly)
{
	return XMetaFile::isAccess(filename, readonly ? user+ '-' : user);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  isAccessGroup
// Params:  const QString& filename
// Params:  const QString& group
//----------------------------------------------------------------------------- 
bool XMetaFile::isAccessGroup(const QString& filename, const QString& group, bool readonly)
{
	return XMetaFile::isAccess(filename, "", readonly ? group+ '-' : group);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  isAccessRole
// Params:  const QString& filename
// Params:  const QString& role
//----------------------------------------------------------------------------- 
bool XMetaFile::isAccessRole(const QString& filename, const QString& role, bool readonly)
{
	return XMetaFile::isAccess(filename, "", "", readonly ? role + '-' : role);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  isAccessPassword
// Params:  const QString& filename
// Params:  const QString& password
//----------------------------------------------------------------------------- 

bool XMetaFile::isAccessPassword(const QString& filename, const QString& password, bool readonly)
{
	return XMetaFile::isAccess(filename, "", "", "", readonly ? password+ '-' : password);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  touchRead
// Params:  const QString& filename
// Params:  const QString& user
// Params:  const QString& role
// Params:  const QDateTime& timestamp
//----------------------------------------------------------------------------- 
bool XMetaFile::touchRead(const QString& filename, const QString& user, const QString& group, const QString& role, const QDateTime& timestamp)
{
	return touch(filename, "Read", user, group, role,timestamp);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  touchWrite
// Params:  const QString& filename
// Params:  const QString& user
// Params:  const QString& role
// Params:  const QDateTime& timestamp
//----------------------------------------------------------------------------- 
bool XMetaFile::touchWrite(const QString& filename, const QString& user, const QString& group, const QString& role, const QDateTime& timestamp)
{
	return touch(filename, "Write", user, group, role, timestamp);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  touchCreate
// Params:  const QString& filename
// Params:  const QString& user
// Params:  const QString& role
// Params:  const QDateTime& timestamp
//----------------------------------------------------------------------------- 
bool XMetaFile::touchCreate(const QString& filename, const QString& user, const QString& group, const QString& role, const QDateTime& timestamp)
{
	return touch(filename, "Create", user, group,role, timestamp);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  touchCopy
// Params:  const QString& filename
// Params:  const QString& user
// Params:  const QString& role
// Params:  const QDateTime& timestamp
//----------------------------------------------------------------------------- 
bool XMetaFile::touchCopy(const QString& filename, const QString& user, const QString& group, const QString& role, const QDateTime& timestamp)
{
	return touch(filename, "Copy", user, group, role, timestamp);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  touchLog
// Params:  const QString& filename
// Params:  const QString& info
// Params:  const QString& user
// Params:  const QString& role
// Params:  const QDateTime& timestamp
//----------------------------------------------------------------------------- 
bool XMetaFile::touchLog(const QString& filename, const QString& info, const QString& group, const QString& user, const QString& role, const QDateTime& timestamp)
{
	return touch(filename, info, user, group, role, timestamp);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  clear
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
bool XMetaFile::remove(const QString& filename)
{
	return removeLog(filename)&& removeAccess(filename);
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  logClear
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
bool XMetaFile::removeLog(const QString& filename)
{
	return XFile::remove(fileNameLog(filename));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  accessClear
// Params:  const QString& filename
//----------------------------------------------------------------------------- 
bool XMetaFile::removeAccess(const QString& filename)
{
	return XFile::remove(fileNameAccess(filename));
}

//-----------------------------------------------------------------------------
// Author:  Tobias Post
// Company: CUBE4DEV GmbH
// Date:    20.10.2022
// Context: DefaultNamespace
// Class:   XMetaFile
// Method:  touch
// Params:  const QString& filename
// Params:  const QString& info
// Params:  const QString& user
// Params:  const QString& role
// Params:  const QDateTime& timestamp
//----------------------------------------------------------------------------- 
bool XMetaFile::touch(const QString& filename, const QString& info, const QString& user, const QString& group, const QString& role,const QDateTime& timestamp)
{
	QList<QStringList> list=XFile::read<QList<QStringList>>(fileNameLog(filename));
	while (list.count()> m_MaxTouch) list.takeFirst();
	list.append( QStringList() << timestamp.toString(Qt::DateFormat::ISODate) << user <<group << role  << info);
	return XFile::write<QList<QStringList>>(fileNameLog(filename), list,true);
}
