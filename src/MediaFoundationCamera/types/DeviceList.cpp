#include "DeviceList.h"
#include <comdef.h>
#include <atlcomcli.h>
#include <Strmif.h>
#include <Dshow.h>

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

    std::string convertWideChar( WCHAR* aWideChar )
    {
        _bstr_t     b( aWideChar );
        const char* c    = b;
        std::string lRet = std::string( c );

        return lRet;
    }
}

DeviceList::DeviceList() : mppDevices( nullptr )
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
        SafeRelease( &mppDevices[ i ] );
    }
    CoTaskMemFree( mppDevices );
    mppDevices  = nullptr;
    mDeviceCount = 0;
}

HRESULT DeviceList::EnumerateDevices()
{
    HRESULT       lQueryResult = S_OK;
    IMFAttributes *pAttributes = nullptr;

    this->Clear();

    lQueryResult = MFCreateAttributes( &pAttributes
                                     , 1 );

    if( SUCCEEDED( lQueryResult ) )
    {
        lQueryResult = pAttributes->SetGUID( MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE
                                           , MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
        );
    }

    if( SUCCEEDED( lQueryResult ) )
    {
        lQueryResult = MFEnumDeviceSources( pAttributes
                                          , &mppDevices
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

    *ppActivate = mppDevices[ index ];
    ( *ppActivate )->AddRef();

    return S_OK;
}

std::string DeviceList::GetDevicePropertyString( const UINT32 aIndex, GUID aGuidKey )
{
    std::string lResult{};
    if( aIndex >= Count() )
    {
        return lResult;
    }

    HRESULT          lQueryResult         = S_OK;
    WCHAR*           lPropertyValue       = nullptr;
    UINT32           lPropertyValueLength = 0;
    IAMCameraControl* lIAMCameraControl;
    IMFActivate*     lpDevice             = mppDevices[ aIndex ];

    // The parameter's type provided to the IID_PPV_ARGS has to match the desired interfaces type
    lQueryResult = lpDevice->ActivateObject( IID_PPV_ARGS( &lIAMCameraControl ) ); /*! initializes the IMFMediaSource pointer */
    if( lQueryResult != S_OK )
    {
        CComQIPtr<IAMCameraControl> lpCameraControl( lIAMCameraControl ); /*! initializes the IAMCameraControl pointer */
        long lFlag = 0x0001;
        long lPanValue, lTiltValue, lRollValue, lZoomValue, lExposure, lIrisValue, lFocusValue;
        lpCameraControl->Get( CameraControl_Pan, &lPanValue, &lFlag );
        lpCameraControl->Get( CameraControl_Tilt, &lTiltValue, &lFlag );
        lpCameraControl->Get( CameraControl_Roll, &lRollValue, &lFlag );
        lpCameraControl->Get( CameraControl_Zoom, &lZoomValue, &lFlag );
        lpCameraControl->Get( CameraControl_Exposure, &lExposure, &lFlag );
        lpCameraControl->Get( CameraControl_Iris, &lIrisValue, &lFlag );
        lpCameraControl->Get( CameraControl_Focus, &lFocusValue, &lFlag );
    }



    //CComQIPtr<IAMVideoProcAmp>  lpVideo( *lppMediaSource ); /*! initializes the IAMVideoProcAmp pointer */

    /* Now we have access to the functions of both IAMCameraControl and IAMVideoProcAmp interfaces of the Captore Device */

    /* Camera Control properties: https://docs.microsoft.com/en-us/previous-versions/ms779747%28v%3dvs.85%29 
     * Camera Control flags:      https://docs.microsoft.com/en-us/previous-versions/ms779746%28v%3dvs.85%29 
     */
    
    /* VideoProcAmp properties : https://docs.microsoft.com/en-us/previous-versions/ms787924%28v%3dvs.85%29
     * VideoProcAmp flags:       https://docs.microsoft.com/en-us/previous-versions/ms787923%28v%3dvs.85%29
     */

    lQueryResult         = lpDevice->GetAllocatedString( aGuidKey // alpahbetical order of MF attributes: https://docs.microsoft.com/en-us/windows/desktop/medfound/alphabetical-list-of-media-foundation-attributes
                                                       , &lPropertyValue
                                                       , &lPropertyValueLength );

    if( !SUCCEEDED( lQueryResult ) )
    {
        return lResult;
    }

    lResult = convertWideChar( lPropertyValue );
    CoTaskMemFree( lPropertyValue );
    lPropertyValue = nullptr;
    
    return lResult;
}

void DeviceList::PrintDeviceProperties( GUID aGuidKey )
{
    for( UINT32 i = 0; i < Count(); ++i )
    {
        std::string lDeviceName = GetDevicePropertyString( i , aGuidKey );
        if( !lDeviceName.empty() )
        {
            std::cout << lDeviceName.c_str() << std::endl;
        }
    }
}

bool DeviceList::UpdateDeviceList()
{
    HRESULT lQueryResult = S_OK;
    Clear();

    lQueryResult = EnumerateDevices();
    if( FAILED( lQueryResult ) )
    {
        return false;
    }

    std::cout << "Number of devices found: " << Count() << std::endl;

    return true;
}
