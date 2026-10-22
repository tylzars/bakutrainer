#include <Main/Link/wireless_state.h>

// Names of the api ids (nothing reads this table).
const char *data_02063cc8[5] = {
    data_02063d1c, data_02063da8, data_02063d6c, data_02063d80, data_02063dec,
};

// Names of the link states, indexed by state.
const char *data_02063cdc[10] = {
    data_02063d30, data_02063d08, data_02063e04, data_02063d44, data_02063e34,
    data_02063f0c, data_02063e4c, data_02064088, data_02063f54, data_02063d58,
};

// State of the wireless link.
Unk_WirelessWork data_020c8380;
