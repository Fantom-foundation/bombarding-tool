#pragma once

#include <vector>

#include "configuration.hpp"

extern "C" {
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
}

namespace blomb {

namespace cdetail {
int run_process(const char* cmd)
{
    pid_t fork_res = fork();
    if (fork_res == -1)
        return -1;

    if (fork_res)
        return fork_res;

    if (execl("/bin/sh", "sh", "-c", cmd, NULL)) {
        perror("execl");
        exit(1);
    }
}

int terminate_process(pid_t pid, int sig) {
    if (!pid)
        return 0;

    if (kill(pid, sig))
        return -1;
    if (waitpid(pid, NULL, 0) == -1)
        return -1;
    return 0;
}

int wait_process(pid_t pid) {
    if (!pid)
        return 0;

    if (waitpid(pid, NULL, 0) == -1)
        return -1;
    return 0;
}

}

class Process {
private:
    pid_t pid;

public:
    Process(const std::string& cmd)
    {
        if ((pid = cdetail::run_process(cmd.c_str())) == -1)
            throw std::runtime_error("Cannot execute command '" + cmd + "'");
    }

    Process(const Process&) = delete;

    Process& operator=(const Process&) = delete;

    Process(Process&& p) noexcept
        : pid(p.pid)
    {
        p.pid = 0;
    }

    Process& operator=(Process&& p) noexcept
    {
        pid = p.pid;
        p.pid = 0;
        return *this;
    }

    ~Process()
    {
        cdetail::wait_process(pid);
    }
};

void execute(const std::vector<std::string>& cmd) {
    for (const auto& c : cmd)
        Process{c};
}

}
