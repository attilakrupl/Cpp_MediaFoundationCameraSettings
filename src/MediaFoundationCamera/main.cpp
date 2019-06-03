/*!
 * \file   main.cpp
 * \author Attila Kr�pl
 * \date   2019/06/03
 * \info   www.krupl.com
 */

#include "stdafx.h"

/* MF_DEVSOURCE_ATTRIBUTEs represented as string, which can be used with the current solution
 *      MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME 
 *      MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK
 *      MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ID
 */

int main()
{
    DeviceList lDeviceList;
    if( lDeviceList.UpdateDeviceList() )
    {
        lDeviceList.PrintDeviceProperties();
        while( 1 )
        {};
    }
    return 0;
}