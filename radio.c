#include  "define.h"

void radio(void)
{
RF1AINSTRB = RF_SRES;
RF1AINSTRB = RF_SRX;
//RF1AINSTRB = RF_SRX;
//  SOUND_ON;
}