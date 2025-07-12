//
// Created by halfblood on 7/11/25.
//

#include "flexisim_stats.h"


namespace dramsim3 {
FlexisimStats::FlexisimStats(const Config& config, const int channel_id): SimpleStats(config, channel_id) {}

FlexisimRunStat FlexisimStats::GetStat(){
    UpdateFinalStats();

    FlexisimRunStat stats;
    stats.bandwidth_ = calculated_["average_bandwidth"];
    stats.num_reads_ = counters_["num_reads_done"];
    stats.num_writes_ = counters_["num_writes_done"];
    stats.avg_read_latency_ = calculated_["average_read_latency"];

    return stats;
}

}