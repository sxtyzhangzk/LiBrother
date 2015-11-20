#ifndef __PROGRAM_LAUNCHER_H__
#define __PROGRAM_LAUNCHER_H__

#include <string>
#include <vector>
#include <queue>
#include <chrono>

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

	bool waitForStop(int nID, int nTimeoutMillisecond, bool bTerminate);

protected:
#ifdef _WIN32
	typedef void * handle_t;
#endif
	struct TTask
	{
		std::string strPath;
		std::vector<std::string> vArgs;
		std::chrono::time_point<std::chrono::system_clock> lastRun;
	};
	std::vector<handle_t> m_vBackendProcesses;
	std::priority_queue<TTask> m_qTasks;
};


#endif