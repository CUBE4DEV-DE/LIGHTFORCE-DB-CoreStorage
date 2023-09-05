#pragma once

#include <QJsonObject>
#include <QMap>

#include "../../Core/CoreStorage/XFunctionInject.h"

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
