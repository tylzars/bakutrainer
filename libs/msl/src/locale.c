#include <stdlib.h>
#include <runtime.h>

// Time names and formats of the locale record (data_02064520); no code in the ROM reads them.
typedef struct {
    /* 00 */ char *amPm;
    /* 04 */ char *dateTime;
    /* 08 */ char *time12;
    /* 0c */ char *date;
    /* 10 */ char *time;
    /* 14 */ char *dayNames;
    /* 18 */ char *monthNames;
    /* 1c */ char *mUnk_1c;
} LocaleTime;

// Collation record of the locale: the order table covers the characters from 0x20 on.
typedef struct {
    /* 00 */ int first;
    /* 04 */ int mUnk_04;
    /* 08 */ int mUnk_08;
    /* 0c */ unsigned short *order;
} LocaleCollate;

CharConv data_020644dc = {mbtowc, wctomb};

LocaleCollate data_020644e4 = {0x20, 0x6e, 0, data_02064628};

LocaleTime data_020644f4 = {
    "AM|PM",
    "%a %b %e %T %Y",
    "%I:%M:%S %p",
    "%m/%d/%y",
    "%T",
    "Sun|Sunday|Mon|Monday|Tue|Tuesday|Wed|Wednesday|Thu|Thursday|Fri|Friday|Sat|Saturday",
    "Jan|January|Feb|February|Mar|March|Apr|April|May|May|Jun|June|Jul|July|Aug|August|Sep|September|Oct|October|Nov|"
    "November|Dec|December",
    "",
};

Locale data_02064520 = {&data_020644f4, &data_020644e4, &data_020644dc};
