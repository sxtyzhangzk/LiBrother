#ifndef __PROGRAM_LAUNCHER_H__
#define __PROGRAM_LAUNCHER_H__

#include <string>
#include <vector>
#include <thread>
#include <mutex>

class CProgramLauncher
{
public:
	enum TLaunchType
	{
		StartOnly,
		RunBackend,
		RunWait,
		RunAsTask
	};
public:
	int runProgram(
		const std::string& strPath, 
		const std::vector<std::string>& vArgs, 
		TLaunchType type, 
		int nMilliseconds);

	bool stopProgram(int nID, int nTimeoutMillisecond, bool bTerminate);

protected:
#ifdef _WIN32
	typedef void * handle_t;
#endif
	struct TTask
	{
		TLaunchType type;
		handle_t hProcess;
		std::string strPath;
		std::vector<std::string> strArgs;
		int nWaitTime;

		std::timed_mutex * pmutexLauncher;
	};

	int getAvaliableTaskID();
	bool StartProcess(TTask& task);
	bool WaitForProcess(TTask& task, int nTimeout, bool bTerminate);
	void LauncherThread(TTask * task);

protected:
	std::vector<TTask *> m_vTasks;
};


#endif