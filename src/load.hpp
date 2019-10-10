#pragma once

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "info.h"

namespace blomb {
    static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

    class Load {
    private:
        MemInfo memInfo;

    public:
        Load() {
            memInfo = get_mem_info();
        }

        double get_memory_usage() {
            return (double) memInfo.avail / (double) memInfo.total;
        }

        double get_cpu_usage() {
            double percent;
            FILE *file;
            unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

            file = fopen("/proc/stat", "r");
            fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
                   &totalSys, &totalIdle);
            fclose(file);

            if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
                totalSys < lastTotalSys || totalIdle < lastTotalIdle) {
                //Overflow detection. Just skip this value.
                percent = -1.0;
            } else {
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

            return calculate_cpu_load();
        }

        void print_load(double cpu, double memory, double tps) {
            printf("CPU usage: %5.2f \n", cpu);
            printf("Memory usage: %5.2f \n", memory);
            printf("Transactions per second: %5.2f \n", tps);

            // TODO: Use libcurl or similar
            const std::string command = "curl -X GET '157.230.33.232/update?cpu=" + \
                std::to_string(cpu) + "&memory=" + std::to_string(memory) + \
                "&tps=" + std::to_string(tps) + "' >/dev/null 2>&1";
            system(command.c_str());
        }
    };
}
