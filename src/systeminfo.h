#pragma once
#include <QString>
struct SystemInfo { QString os, architecture, host, cpu, memory, appVersion; };
SystemInfo readSystemInfo();
