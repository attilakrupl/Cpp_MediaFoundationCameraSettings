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
private:
    UINT32      mDeviceCount;
    IMFActivate **m_ppDevices;

public:
    DeviceList();
    ~DeviceList();

    void    Clear();
    UINT32  Count() const;
    HRESULT EnumerateDevices();
    HRESULT GetDevice( const UINT32 index, IMFActivate **ppActivate );
    HRESULT GetDeviceName( const UINT32 index, WCHAR **ppszName );
};