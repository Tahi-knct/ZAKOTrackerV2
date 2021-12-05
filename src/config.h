// set Mode_Sender or Mode_Reciver
#define Mode Mode_Sender

#if Mode == Mode_Sender

#elif Mode == Mode_Reciver

#else
#error you need select Mode in config.h
#endif