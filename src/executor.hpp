#pragma once

#include <vector>
#include <sstream>

#include "configuration.hpp"



extern "C" {
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <string.h>
}

namespace blomb {

namespace cdetail {
int run_process(const char* cmd, int* out_fd)
{
    int pipefd[2];
    if (out_fd) {
        if (pipe(pipefd))
            return -1;

        int ops = fcntl(pipefd[0], F_GETFL);
        if (ops != -1) {
            ops |= O_NONBLOCK;
            ops = fcntl(pipefd[0], F_SETFL, ops);
        }
        if (ops == -1) {
            close(pipefd[0]);
            close(pipefd[1]);
            return -1;
        }
    }

    pid_t fork_res = fork();
    if (fork_res == -1) {
        if (out_fd) {
            close(pipefd[0]);
            close(pipefd[1]);
        }
        return -1;
    }

    if (fork_res) {
        if (out_fd) {
            close(pipefd[1]);
            *out_fd = pipefd[0];
        }
        return fork_res;
    }

    if (out_fd) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        close(pipefd[0]);
    }

    if (execl("/bin/sh", "sh", "-c", cmd, NULL)) {
        perror("execl");
        exit(1);
    }
}

int terminate_process(pid_t pid, int sig)
{
    if (!pid)
        return 0;

    if (kill(pid, sig))
        return -1;
    if (waitpid(pid, NULL, 0) == -1)
        return -1;
    return 0;
}

int wait_process(pid_t pid)
{
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
    int out_fd = -1;

public:
    explicit Process(const std::string& cmd, bool redirect_output = false)
    {
        int* out_fd_ptr = redirect_output ? &out_fd : nullptr;
        if ((pid = cdetail::run_process(cmd.c_str(), out_fd_ptr)) == -1)
            throw std::runtime_error("Cannot execute command '" + cmd + "'");
    }

    Process(const Process&) = delete;

    Process& operator=(const Process&) = delete;

    Process(Process&& p) noexcept
        : pid(p.pid), out_fd(p.out_fd)
    {
        p.pid = 0;
        p.out_fd = -1;
    }

    Process& operator=(Process&& p) noexcept
    {
        cleanup();
        pid = p.pid;
        out_fd = p.out_fd;
        p.pid = 0;
        p.out_fd = -1;
        return *this;
    }

    std::string read_output()
    {
        std::ostringstream res;
        int bufsz = 4096;
        char buf[bufsz];

        while (true) {
            ssize_t got = read(out_fd, buf, bufsz);
            if (got == 0 || (got < 0 && errno == EAGAIN))
                return res.str();

            if (got < 0) {
                throw std::runtime_error(strerror(errno));
            }

            res << std::string(buf, got);
        }
    }

    void wait_termination() {
        cdetail::wait_process(pid);
        pid = 0;
    }

    ~Process()
    {
        cleanup();
    }

private:
    void cleanup()
    {
        cdetail::wait_process(pid);
        if (out_fd >= 0)
            close(out_fd);
    }
};


void execute(const std::vector<std::string>& cmd)
{
    for (const auto& c : cmd) {
        Process{c};
    }
}

}
