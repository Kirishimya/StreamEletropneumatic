#include "ActSettingsRDV.h"

//int actSettings = 0;

//bool getSetting (int s)
//{
//	if(actSettings & s)	return true;
//	return false;
//}

bool getSetting (unsigned int& ss, int s)
{
	if(ss & s)	return true;
	return false;
}

//void setSetting (int s)
//{
//	actSettings |= s;
//}

void setSetting (unsigned int& ss, int s)
{
	ss |= s;
}

//void unsetSetting (int s)
//{
//	actSettings &= ~s;
//}

void unsetSetting (unsigned int& ss, int s)
{
	ss &= ~s;
}

void unsetAll(unsigned int& ss)
{
	ss = 0;
}
