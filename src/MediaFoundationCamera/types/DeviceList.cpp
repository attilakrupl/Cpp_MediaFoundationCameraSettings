#include "DeviceList.h"

namespace
{
    template <class T> void SafeRelease( T **ppT )
    {
        if( *ppT )
        {
            ( *ppT )->Release();
            *ppT = nullptr;
        }
    }

    HRESULT CopyAttribute( IMFAttributes *pSrc, IMFAttributes *pDest, const GUID& key );
}

DeviceList::DeviceList() : m_ppDevices( nullptr )
                         , mDeviceCount( 0 )
{}

DeviceList::~DeviceList()
{
    Clear();
}

UINT32 DeviceList::Count() const
{
    return mDeviceCount;
}

void DeviceList::Clear()
{
    for( UINT32 i = 0; i < mDeviceCount; i++ )
    {
        SafeRelease( &m_ppDevices[ i ] );
    }
    CoTaskMemFree( m_ppDevices );
    m_ppDevices = nullptr;

    mDeviceCount = 0;
}

HRESULT DeviceList::EnumerateDevices()
{
    HRESULT       lQueryResult = S_OK;
    IMFAttributes *pAttributes = nullptr;

    this->Clear();

    lQueryResult = MFCreateAttributes( &pAttributes, 1 );

    if( SUCCEEDED( lQueryResult ) )
    {
        lQueryResult = pAttributes->SetGUID( MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE
                                           , MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
        );
    }

    if( SUCCEEDED( lQueryResult ) )
    {
        lQueryResult = MFEnumDeviceSources( pAttributes
                                          , &m_ppDevices
                                          , &mDeviceCount );
        std::cout << "Devices available:" << mDeviceCount << std::endl;
    }

    SafeRelease( &pAttributes );

    return lQueryResult;
}

HRESULT DeviceList::GetDevice( UINT32 index, IMFActivate **ppActivate )
{
    if( index >= Count() )
    {
        return E_INVALIDARG;
    }

    *ppActivate = m_ppDevices[ index ];
    ( *ppActivate )->AddRef();

    return S_OK;
}

HRESULT DeviceList::GetDeviceName( UINT32 index, WCHAR **ppszName )
{
    if( index >= Count() )
    {
        return E_INVALIDARG;
    }

    HRESULT lQueryResult = S_OK;
    lQueryResult = m_ppDevices[ index ]->GetAllocatedString( MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME
                                                           , ppszName
                                                           , nullptr );

    return lQueryResult;
}
