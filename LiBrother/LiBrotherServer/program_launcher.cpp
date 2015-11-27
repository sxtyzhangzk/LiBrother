#include "program_launcher.h"

#include <cassert>
#include <cstring>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

int CProgramLauncher::runProgram(
	const std::string& strPath, const std::vector<std::string>& vArgs, const std::string& strWorkDir,
	TLaunchType type, int nMilliseconds)
{
	TTask * pTask = new TTask;
	int id;
	pTask->strPath = strPath;
	for (auto arg : vArgs)
		pTask->strArgs.push_back(arg);
	pTask->strWorkDir = strWorkDir;
	pTask->type = type;

	switch (type)
	{
	case StartOnly:
		if (!StartProcess(*pTask))
		{
			delete pTask;
			return -1;
		}
#ifdef _WIN32
		CloseHandle(pTask->hProcess);
#endif
		break;

	case RunBackend:
		if (!StartProcess(*pTask))
		{
			delete pTask;
			return -1;
		}
		id = getAvaliableTaskID();
		m_vTasks[id] = pTask;
		return id;

	case RunWait:
		if (!StartProcess(*pTask))
		{
			delete pTask;
			return -1;
		}
		if (!WaitForProcess(*pTask, nMilliseconds, true))
		{
			delete pTask;
			return -2;
		}
		break;

	case RunAsTask:
		pTask->nWaitTime = nMilliseconds;
		id = getAvaliableTaskID();
		m_vTasks[id] = pTask;
		pTask->pmutexLauncher = new std::timed_mutex;
		pTask->pmutexLauncher->lock();
		pTask->pThread = new std::thread(&CProgramLauncher::LauncherThread, this, m_vTasks[id]);
		return id;

	default:
		return -3;
	}
	return 0;
}

bool CProgramLauncher::stopProgram(int nID, int nTimeoutMillisecond, bool bTerminate)
{
	if (nID >= m_vTasks.size())
		return false;
	TTask * &pTask = m_vTasks[nID];
	if (pTask->type == RunBackend)
	{
		if (WaitForProcess(*pTask, nTimeoutMillisecond, bTerminate))
		{
			delete pTask;
			pTask = nullptr;
			return true;
		}
		return false;
	}
	else if (pTask->type == RunAsTask)
	{
		std::thread *pth = pTask->pThread;
		pTask->pmutexLauncher->unlock();
		pth->join();
		delete pth;
		pTask = nullptr;
		return true;
	}
	return false;
}

int CProgramLauncher::getAvaliableTaskID()
{
	for (int i = 0; i < m_vTasks.size(); i++)
	{
		if (!m_vTasks[i])
			return i;
	}
	m_vTasks.push_back(nullptr);
	return m_vTasks.size() - 1;
}

bool CProgramLauncher::StartProcess(TTask& task)
{
#ifdef _WIN32
	std::string strCmd;
	for (auto cmd : task.strArgs)
		strCmd += " " + cmd;
	char cCmd[MAX_PATH];
#if _MSC_VER >= 1500
	strncpy_s(cCmd, strCmd.c_str(), MAX_PATH - 1);
#else
	strncpy(cCmd, strCmd.c_str(), MAX_PATH - 1);
#endif
	STARTUPINFOA startupInfo = { 0 };
	startupInfo.cb = sizeof(startupInfo);
	PROCESS_INFORMATION processInfo;
	if (!CreateProcessA(task.strPath.c_str(), cCmd, NULL, NULL, FALSE, 0, NULL, task.strWorkDir.c_str(), &startupInfo, &processInfo))
		return false;
	task.hProcess = processInfo.hProcess;
#else
	int pid = fork();
	if (pid < 0)
		return false;
	else if (pid == 0)
	{
		char strNow[256];
		getcwd(strNow, 255);
		std::string strPath;
		if (!task.strPath.empty() && task.strPath[0] != '/')
			strPath = std::string(strNow) + "/" + task.strPath;
		else
			strPath = task.strPath;

		chdir(task.strWorkDir.c_str());
		char **ppArgv = new char*[task.strArgs.size() + 2];
		ppArgv[0] = new char[strPath.length() + 1];
		strcpy(ppArgv[0], strPath.c_str());
		for (int i = 0; i < task.strArgs.size(); i++)
		{
			ppArgv[i + 1] = new char[task.strArgs[i].length() + 1];
			strcpy(ppArgv[i + 1], task.strArgs[i].c_str());
		}
		ppArgv[task.strArgs.size() + 1] = nullptr;
		execv(strPath.c_str(), ppArgv);
		exit(-1);
}
	else
		task.hProcess = pid;
#endif
	return true;
}

bool CProgramLauncher::WaitForProcess(TTask& task, int nTimeout, bool bTerminate)
{
#ifdef _WIN32
	assert(task.hProcess != INVALID_HANDLE_VALUE);
	DWORD dwMilliseconds = nTimeout;
	if (nTimeout < 0)
		dwMilliseconds = INFINITE;
	DWORD ret = WaitForSingleObject(task.hProcess, dwMilliseconds);
	if (ret == WAIT_TIMEOUT)
	{
		if (!bTerminate)
			return false;
		TerminateProcess(task.hProcess, 1);
	}
	CloseHandle(task.hProcess);
	task.hProcess = INVALID_HANDLE_VALUE;
#else
	int status;
	waitpid(task.hProcess, &status, 0);
#endif
	return true;
}

void CProgramLauncher::LauncherThread(TTask * task)
{
	while (!task->pmutexLauncher->try_lock_for(std::chrono::milliseconds(task->nWaitTime)))
	{
		if (StartProcess(*task))
		{
			WaitForProcess(*task, -1, false);
		}
	}
	task->pmutexLauncher->unlock();
	delete task->pmutexLauncher;
	delete task;
}