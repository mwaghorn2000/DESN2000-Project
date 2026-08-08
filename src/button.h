#ifndef DOORBELL_H
#define DOORBELL_H

void doorbell_init(void);
void doorbell_poll(void);
int  doorbell_pending(void);
void doorbell_clear(void);

#endif