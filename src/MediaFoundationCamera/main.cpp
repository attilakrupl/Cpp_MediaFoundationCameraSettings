#include <iostream>
#include "types/DeviceList.h"
#include <comdef.h>

std::string convertWideChar( WCHAR* aWideChar )
{
    _bstr_t b( aWideChar );
    const char* c = b;
    std::string lRet = std::string( c );
    return lRet;
}

HRESULT UpdateDeviceList()
{
    HRESULT    lQueryResult  = S_OK;
    WCHAR*     lFriendlyName = nullptr;
    DeviceList lDeviceList;
    
    lDeviceList.Clear();

    lQueryResult = lDeviceList.EnumerateDevices();
    if( FAILED( lQueryResult ) ) 
    {
        return lQueryResult;
    }

    std::cout << "Number of devices found: " << lDeviceList.Count() << std::endl;
    for( UINT32 iDevice = 0; iDevice < lDeviceList.Count(); iDevice++ )
    {
        lQueryResult = lDeviceList.GetDeviceName( iDevice, &lFriendlyName );
        if( FAILED( lQueryResult ) )
        {
            return lQueryResult;
        }
        std::cout << convertWideChar( lFriendlyName ).c_str() << std::endl;

        CoTaskMemFree( lFriendlyName );
        lFriendlyName = nullptr;
    }
    return lQueryResult;
}

int main()
{
    UpdateDeviceList();
    while( 1 );
    return 0;
}