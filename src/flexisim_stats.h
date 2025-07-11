#ifndef __FLEXISIM_STATS_
#define __FLEXISIM_STATS_
#include "simple_stats.h"

namespace dramsim3 {


struct FlexisimRunStat {
    double bandwidth_;
    size_t num_reads_;
    size_t num_writes_;
    double avg_read_latency_;
};


class FlexisimStats: public SimpleStats {
    public:
    FlexisimStats(const Config &config, int channel_id);
    FlexisimRunStat GetFlexisimMemorySimStat();



};

}





#endif // __FLEXISIM_STATS_
