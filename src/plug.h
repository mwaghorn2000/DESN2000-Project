#ifndef PLUG_H
#define PLUG_H

// time before to turn on
#define PLUG_LEAD 30
// time after to turn off
#define PLUG_FOLLOW 90

void plug_set(int on);
int plug_service(int now_minutes, int coffee_time, int enabled);

#endif
