#include "systeminfo.h"
#include <QSysInfo>
#include <QThread>
#ifdef Q_OS_WIN
#define NOMINMAX
#include <windows.h>
#endif
SystemInfo readSystemInfo() {
 SystemInfo s; s.os=QSysInfo::prettyProductName(); s.architecture=QSysInfo::currentCpuArchitecture();
 s.host=QSysInfo::machineHostName(); s.cpu=QString("%1 logical processors").arg(QThread::idealThreadCount());
 s.appVersion="1.0.0";
#ifdef Q_OS_WIN
 MEMORYSTATUSEX m{}; m.dwLength=sizeof(m);
 if(GlobalMemoryStatusEx(&m)) s.memory=QString("%1 GB total • %2% used").arg(double(m.ullTotalPhys)/1073741824.0,0,'f',1).arg(m.dwMemoryLoad);
#else
 s.memory="Memory details are available in the operating system monitor";
#endif
 return s;
}
