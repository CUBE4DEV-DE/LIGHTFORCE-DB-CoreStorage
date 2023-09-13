#pragma once

#include <QJsonObject>
#include <QMap>

#include "../../Core/CoreStorage/XFunctionInject.h"

// The selected code file seems to contain a utility class XLogInject
// that provides method implementations for logging system calls and
// events.The class and methods can be utilized to debug the system by
// tracking various logs and events.The purpose of this class is to
// provide an easy - to - use logging interface that supports all
// types of system logs and events.

namespace CoreStorage
{

#define XLOG 1

	//-----------------------------------------------------------------------------
	// Author:  Tobias Post
	// Company: CUBE4DEV GmbH
	// Date:    18.02.2023
	// Context: DefaultNamespace
	// Class:   XLogInject
	// Method:  XFunctionLog
	//----------------------------------------------------------------------------- 

	typedef std::function<void(const QString& module, const QString& filename, const QString& line, const QString& classname, const QString& method, const QString& event, const QString& status, const QJsonObject& jsonobj)> XFunctionLog;

	//-----------------------------------------------------------------------------
	// Author:  Tobias Post
	// Company: CUBE4DEV GmbH
	// Date:    16.02.2023
	// Context: DefaultNamespace
	// Class:   XFunctionInject
	// Method:  XLogInject
	// Description:
	//	The XLogInject class is a class that has the purpose of logging system
	//	events and calls.It contains several methods that are related to system
	//	logging, such as inject, hasObjectFunction, call and LogAppend.The
	//	inject method and injects method adds logging functions to a global
	//	m_ObjectInjects object.The hasObjectFunction method and hasObjectFunctions
	//	method are used to check for a function named name in the global
	//	m_ObjectInjects object.The call method and calls method both send a log
	//	message to a file specified by the filepath.Finally, the LogAppend method
	//	takes in a few parameters and appends a log message to several files
	//	specified by the filepath
	//----------------------------------------------------------------------------- 
	class CORESTORAGE_EXPORT XLogInject :public  XFunctionInject<XFunctionLog>
	{
	public:
		void call(const QString& module, const QString& filename, const QString& line, const QString& classname, const QString& method, const QString& event, const QString& status, const QJsonObject& jsonobj)
		{
			//qDebug() << module << filename << line << classname << method << event << status;
			if (function())
				function()(module, filename, line, classname, method, event, status, jsonobj);
		}
		void call(const QString& name, const QString& module, const QString& filename, const QString& line, const QString& classname, const QString& method, const QString& event, const QString& status, const QJsonObject& jsonobj)
		{
			//qDebug() << name << module << filename << line << classname << method << event << status;
			if (function(name))
				function(name)(module, filename, line, classname, method, event, status, jsonobj);
		}
		void calls(const QString& module, const QString& filename, const QString& line, const QString& classname, const QString& method, const QString& event, const QString& status, const QJsonObject& jsonobj)
		{
			//qDebug() << module << filename << line << classname << method << event << status;
			call(module, filename, line, classname, method, event, status, jsonobj);
			for (XFunctionLog f : functions())
				f(module, filename, line, classname, method, event, status, jsonobj);
		}
		void calls(const QString& name, const QString& module, const QString& filename, const QString& line, const QString& classname, const QString& method, const QString& event, const QString& status, const QJsonObject& jsonobj)
		{
			//qDebug() << name << module << filename << line << classname << method << event << status;
			call(name, module, filename, line, classname, method, event, status, jsonobj);
			for (XFunctionLog f : functions(name))
				f(module, filename, line, classname, method, event, status, jsonobj);
		}
	};

	//-----------------------------------------------------------------------------
	// Author:  Tobias Post
	// Company: CUBE4DEV GmbH
	// Date:    16.02.2023
	// Context: DefaultNamespace
	// Class:   XLogInject
	// Method:  XLogsInject
	// Description:
	//	The selected code file contains a XLogsInject class with
	//	multiple methods that are related to system logging. Here's a
	//	brief summary of what each method does:
	//	inject(name, function) and injects(name, function) are both
	//		injectors for logging functions.They will add a logging function
	//		with the given name to a global m_ObjectInjects object.
	//	hasObjectFunction(name) and hasObjectFunctions(name) are
	//		both used to check for a function named name in the global
	//		m_ObjectInjects object, which is used for system logging
	//	call(name, module, filename, line, classname, method,
	//		event, status, jsonobj) and calls(name, module, filename,
	//		line, classname, method, event, status, jsonobj) both
	//		send a log message to a file specified by the filepath.
	//		This function is used for logging system calls.
	//	LogAppend(filepath, context, module, filename,
	//		line, classname, method, event, status, jsonobj) takes
	//		in a few parameters and appends a log message to several files
	//		specified by the filepath.This function is used for logging system events.

	//----------------------------------------------------------------------------- 

	class CORESTORAGE_EXPORT XLogsInject :public  XFunctionInject<XFunctionLog>
	{
	public:
		static void inject(const QString& name, XFunctionLog function);
		static void injects(const QString& name, XFunctionLog function);
		static bool hasObjectFunction(const QString& name);
		static bool hasObjectFunctions(const QString& name);
		static void call(const QString& name, const QString& module, const QString& filename, const QString& line, const QString& classname, const QString& method, const QString& event, const QString& status, const QJsonObject& jsonobj);
		static void calls(const QString& name, const QString& module, const QString& filename, const QString& line, const QString& classname, const QString& method, const QString& event, const QString& status, const QJsonObject& jsonobj);
		static void LogAppend(const QString& filepath, const QString& context, const QString& module, const QString& filename, const QString& line, const QString& classname, const QString& method, const QString& event, const QString& status, const QJsonObject& jsonobj);
		static QByteArray LogContent(const QString& context, const QString& module, const QString& filename,const QString& line,const QString& classname, const QString& method, const QString& event,const QString& status,const QJsonObject& jsonobj);
		static void LogAppend(const QStringList& filepathlist, const QString& context, const QString& module, const QString& filename, const QString& line, const QString& classname, const QString& method, const QString& event, const QString& status, const QJsonObject& jsonobj);
	private:
		static XLogInject m_ObjectInjects;
	};

#ifdef XLOG
#define X_LOGJSON(name,module,classname, method, event, status,jsonobj) XLogsInject::calls(name,module, QString("%1").arg(__FILE__),QString("%1").arg(__LINE__),classname, method, event, status,jsonobj)  
#define X_LOG(name,module,classname, method, event, status) XLogsInject::calls(name,module, QString("%1").arg(__FILE__),QString("%1").arg(__LINE__),classname, method, event, status,QJsonObject())  

#define X_SYSTEMJSON(module,classname, method, event, status,jsonobj) XLogsInject::calls("SYSTEM",module, QString("%1").arg(__FILE__),QString("%1").arg(__LINE__),classname, method, event, status,jsonobj)  
#define X_SYSTEM(module,classname, method, event, status) XLogsInject::calls("SYSTEM",module, QString("%1").arg(__FILE__),QString("%1").arg(__LINE__),classname, method, event, status,QJsonObject())  

#define X_DATAJSON(module,classname, method, event, status,jsonobj) XLogsInject::calls("DATA",module, QString("%1").arg(__FILE__),QString("%1").arg(__LINE__),classname, method, event, status,jsonobj)  
#define X_DATA(module,classname, method, event, status) XLogsInject::calls("DATA",module, QString("%1").arg(__FILE__),QString("%1").arg(__LINE__),classname, method, event, status,QJsonObject())  

#define X_WARNINGJSON(module,classname, method, event, status,jsonobj) XLogsInject::calls("WARNING",module, QString("%1").arg(__FILE__),QString("%1").arg(__LINE__),classname, method, event, status,jsonobj)  
#define X_WARNING(module,classname, method, event, status) XLogsInject::calls("WARNING",module, QString("%1").arg(__FILE__),QString("%1").arg(__LINE__),classname, method, event, status,QJsonObject())  

#define X_CRITICALJSON(module,classname, method, event, status,jsonobj) XLogsInject::calls("CRITICAL",module, QString("%1").arg(__FILE__),QString("%1").arg(__LINE__),classname, method, event, status,jsonobj) 
#define X_CRITICAL(module,classname, method, event, status) XLogsInject::calls("CRITICAL",module, QString("%1").arg(__FILE__),QString("%1").arg(__LINE__), method, event, status,QJsonObject()) 

#endif


};
using namespace CoreStorage;
