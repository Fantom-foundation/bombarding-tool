#include "utils.hpp"
#include "time_utils.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <map>

template<class Handler>
void for_each_json_line(utils::Span span, Handler &&handler)
{
    const char *const end = span.data + span.size;
    const char *const begin = span.data;
    const char *ptr = begin;
    while (ptr != end) {
        const char *p1 = reinterpret_cast<const char *>(::memchr(ptr, ']', end - ptr));
        if (!p1)
            break;
        const char *p2 = reinterpret_cast<const char *>(::memchr(p1, '\n', end - p1));
        if (!p2)
            break;
        const char *p3 = p2 - 3;
        const char *p4 = reinterpret_cast<const char *>(::memrchr(p1, '"', p3 - p1));
        if (!p4)
            break;
        handler(
            utils::Span{p1 + 2, static_cast<size_t>(p2 - p1) - 2},
            time_utils::parse_datetime_json(p4 + 1)
        );
        ptr = p2 + 1;
    }
}

template<class Handler>
void for_each_log_line(unsigned year, utils::Span span, Handler &&handler)
{
    const char *const end = span.data + span.size;
    const char *const begin = span.data;
    const char *ptr = begin;
    while (ptr != end) {
        const char *p1 = reinterpret_cast<const char *>(::memchr(ptr, '[', end - ptr));
        if (!p1)
            break;
        const char *p2 = reinterpret_cast<const char *>(::memchr(p1, '\n', end - p1));
        if (!p2)
            break;
        enum { OFFSET = 21 };
        handler(
            utils::Span{p1 + OFFSET, static_cast<size_t>(p2 - p1) - OFFSET},
            time_utils::parse_datetime_log(year, p1 + 1)
        );
        ptr = p2 + 1;
    }
}

static inline
bool span_starts_with(utils::Span span, const char *s)
{
    const size_t ns = strlen(s);
    return span.size >= ns && memcmp(span.data, s, ns) == 0;
}

static
void print_usage_and_exit()
{
    ::fprintf(stderr, "USAGE: main [-f <datetime>] [-c <ratio>] [-j] <log file>...\n");
    ::exit(2);
}

int main(int argc, char **argv)
{
    uint64_t from_timestamp = 0;
    double cutoff_ratio = 0.33;
    unsigned year = time_utils::current_year();
    bool json = false;

    for (int c; (c = getopt(argc, argv, "f:c:j")) != -1;) {
        switch (c) {
        case 'f':
            from_timestamp = time_utils::parse_datetime_json(optarg);
            break;
        case 'c':
            cutoff_ratio = ::strtod(optarg, nullptr);
            break;
        case 'y':
            year = ::strtoul(optarg, nullptr, 10);
            break;
        case 'j':
            json = true;
            break;
        default:
            print_usage_and_exit();
        }
    }
    if (optind == argc)
        print_usage_and_exit();

    std::map<std::string, uint64_t> eid_to_timestamp;

    const auto handler = [&](utils::Span span, uint64_t timestamp)
    {
        if (timestamp < from_timestamp)
            return;
        const char *const prefix = "consensus: event is atropos";
        if (span_starts_with(span, prefix)) {
            const char *s = span.data + strlen(prefix);
            while (*s == ' ')
                ++s;
            const char *t = s;
            while (*t != '\\' && *t != '"')
                ++t;
            eid_to_timestamp[std::string(s, t - s)] = timestamp;
        }
    };

    for (int i = optind; i < argc; ++i) {
        auto fd = utils::open_for_reading(argv[i]);
        auto mm = utils::MemoryMap(fd, utils::file_size(fd));
        if (json)
            for_each_json_line(static_cast<utils::Span>(mm), handler);
        else
            for_each_log_line(year, static_cast<utils::Span>(mm), handler);
    }

    std::vector<uint64_t> events;
    for (const auto &kv : eid_to_timestamp)
        events.push_back(kv.second);
    std::sort(events.begin(), events.end());

    const size_t nevents = events.size();
    printf("Total: %zu events\n", nevents);
    const size_t cutoff = cutoff_ratio * nevents;

    double r = 0;
    size_t r_nev = 0;
    for (size_t i = 0; i < nevents; ++i) {
        for (size_t j = 0; j < i; ++j) {
            const size_t ev_delta = i - j;
            if (ev_delta < cutoff) continue;
            const uint64_t time_delta = events[i] - events[j];
            const double tpns = static_cast<double>(time_delta) / ev_delta;
            const double tps = tpns * 1e-9;
            if (r < tps) {
                r = tps;
                r_nev = ev_delta;
            }
        }
    }
    printf("TPS: %g (from %zu events)\n", r, r_nev);
}
