#pragma once

#include "sys/types.h"
#include "sys/sysinfo.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

namespace blomb {
    struct sysinfo memInfo;
    static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

    class Load {
    public:
        Load() {
            FILE* file = fopen("/proc/stat", "r");
            fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
                   &lastTotalSys, &lastTotalIdle);
            fclose(file);
        }

        double get_memory_usage() {
            sysinfo (&memInfo);
            auto totalVirtualMem = memInfo.totalram;
            totalVirtualMem += memInfo.totalswap;
            totalVirtualMem *= memInfo.mem_unit;

            auto virtualMemUsed = memInfo.totalram - memInfo.freeram;
            virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
            virtualMemUsed *= memInfo.mem_unit;

            return (double)virtualMemUsed / (double)totalVirtualMem;
        }

        double get_cpu_usage() {
            double percent;
            FILE* file;
            unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

            file = fopen("/proc/stat", "r");
            fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
                   &totalSys, &totalIdle);
            fclose(file);

            if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
                totalSys < lastTotalSys || totalIdle < lastTotalIdle){
                //Overflow detection. Just skip this value.
                percent = -1.0;
            }
            else{
                total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
                        (totalSys - lastTotalSys);
                percent = total;
                total += (totalIdle - lastTotalIdle);
                percent /= total;
                percent *= 100;
            }

            lastTotalUser = totalUser;
            lastTotalUserLow = totalUserLow;
            lastTotalSys = totalSys;
            lastTotalIdle = totalIdle;

            return percent;
        }

        void print_load(double cpu, double memory, double tps) {
            printf ("CPU usage: %5.2f \n", cpu);
            printf ("Memory usage: %5.2f \n", memory);
            printf ("Transactions per second: %5.2f \n", tps);
            
            std::string command = "curl -X GET '157.230.33.232/update?cpu=" + std::to_string(cpu) + "&memory=" + std::to_string(memory) + "&tps=" + std::to_string(tps) + "' >/dev/null 2>&1";
            system(command.c_str());
        }
    };
}
