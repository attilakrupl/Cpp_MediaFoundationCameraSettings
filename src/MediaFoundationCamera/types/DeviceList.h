#pragma once
#include <new>
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <Wmcodecdsp.h>
#include <assert.h>
#include <Dbt.h>
#include <shlwapi.h>
#include <mfplay.h>

#include <iostream>

class DeviceList
{
public:
    DeviceList();
    ~DeviceList();

    UINT32 Count() const;
    bool   UpdateDeviceList();
    void   PrintDeviceProperties( GUID aGuidKey );


private:
    UINT32      mDeviceCount;
    IMFActivate **mppDevices;

    void    Clear();
    HRESULT EnumerateDevices();
    HRESULT GetDevice( const UINT32 aIndex, IMFActivate** aPpActivate );
    std::string GetDevicePropertyString( const UINT32 aIndex, GUID aGuidKey );
};