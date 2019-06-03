/*!
 * \file   devicelist.h
 * \author Attila Krüpl
 * \date   2019/06/03
 * \info   www.krupl.com
 */

#pragma once

class DeviceList
{
public:
    DeviceList();
    ~DeviceList();

    UINT32 Count() const;
    bool   UpdateDeviceList();
    void   PrintDeviceProperties();
    void   PrintCameraControlValues( const UINT32 aIndex );


private:
    UINT32      mDeviceCount;
    IMFActivate **mppDevices;

    void    Clear();
    HRESULT EnumerateDevices();
    HRESULT GetDevice( const UINT32 aIndex, IMFActivate** aPpActivate );
    std::string GetDevicePropertyString( const UINT32 aIndex, GUID aGuidKey );
};