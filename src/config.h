// set Mode_Sender or Mode_Reciver
#define Mode Mode_Sender

#if Mode == Mode_Sender
const int Call_Num = 0;

#elif Mode == Mode_Receiver

#else
    #error you need select Mode in config.h
#endif