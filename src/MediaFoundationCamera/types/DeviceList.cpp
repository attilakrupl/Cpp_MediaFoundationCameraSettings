#include "DeviceList.h"
#include <comdef.h>
#include <atlcomcli.h>
#include <Strmif.h>
#include <Dshow.h>

const int SLEEPTIME = 1000;

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

    template<typename T>
    void LogValue( const std::string& aName, const T& aValue )
    {
        std::cout << aName.c_str() << " - Value: " << aValue << std::endl;
    }

    template<typename T>
    void LogRange( const std::string& aName, const T& aMin, const T& aMax, const T& aDef, const T& aStep, const T& aFlags )
    {
        std::cout << aName.c_str() << " - Min: " << aMin << ", Max: " << aMax << ", Default: " << aDef << ", Step: " << aStep << ", Flags: " << aFlags <<  std::endl;
    }

    void LogFailure( const std::string& aName )
    {
        std::cout << "Reading " << aName.c_str() << " attribute has failed!" << std::endl;
    }

    template<typename T, typename P>
    void FlipSettings( T aHandle, P aProperty, long aMin, long aMax, long aDef, long aFlags )
    {
        HRESULT lQueryResult = S_OK;
        std::cout << "Changing property: "<< std::endl;

        Sleep( SLEEPTIME );
        lQueryResult = aHandle->Set( aProperty
                                   , aMin
                                   , aFlags );
        if ( lQueryResult == S_OK )
        {
            std::cout << "Setting to min succeeded" << std::endl;
        }
        Sleep( SLEEPTIME );
        lQueryResult = aHandle->Set( aProperty
                                   , aMax
                                   , aFlags );
        if ( lQueryResult == S_OK )
        {
            std::cout << "Setting to max succeeded" << std::endl;
        }
        Sleep( SLEEPTIME );
        lQueryResult = aHandle->Set( aProperty
                                   , aDef
                                   , aFlags );
        if ( lQueryResult == S_OK )
        {
            std::cout << "Setting to default succeeded" << std::endl;
        }
    }

    template<typename T, typename P>
    void ReadAttribute(       T            aHandle
                      , const P            aProperty
                      , const std::string& aPropertyName )
    {
        HRESULT lQueryResult = S_OK;

        long lMin, lMax, lStep, lDef, lFlags;
        lQueryResult = aHandle->GetRange( aProperty, &lMin, &lMax, &lStep, &lDef, &lFlags );
        if( lQueryResult == S_OK )
        {
            LogRange( aPropertyName, lMin, lMax, lDef, lStep, lFlags );
            long    lValue;
                    lQueryResult = aHandle->Get( aProperty
                                               , &lValue
                                               , &lFlags );
            if( lQueryResult == S_OK )
            {
                LogValue( aPropertyName, lValue );
            }
            
            FlipSettings<T, P>(aHandle, aProperty, lMin, lMax, lDef, lFlags);
        }
        else
        {
            LogFailure( aPropertyName );
        }

        std::cout << std::endl;
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
    IMFActivate*     lpDevice             = mppDevices[ aIndex ];

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

    std::cout << lResult.c_str() << std::endl;

    return lResult;
}

void DeviceList::PrintDeviceProperties()
{
    for( UINT32 i = 0; i < Count(); ++i )
    {
        std::string lDeviceName    = GetDevicePropertyString( i , MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME );
        std::string lDeviceSymLink = GetDevicePropertyString( i , MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK );

        if( !lDeviceName.empty()
         && !lDeviceSymLink.empty() )
        {
            PrintCameraControlValues( i );
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
    std::cout << "--------------------------\n"<< std::endl;

    return true;
}

void DeviceList::PrintCameraControlValues( const UINT32 aIndex )
{
    HRESULT         lQueryResult         = S_OK;
    WCHAR*          lPropertyValue       = nullptr;
    UINT32          lPropertyValueLength = 0;
    IMFActivate*    lpDevice             = mppDevices[ aIndex ];
    IMFMediaSource* lIMFMediaSource;

    // The parameter's type provided to the IID_PPV_ARGS has to match the desired interfaces typen
    CoInitialize( nullptr );
    lQueryResult = lpDevice->ActivateObject( IID_PPV_ARGS( &lIMFMediaSource ) ); /*! initializes the IMFMediaSource pointer */

    if( lQueryResult == S_OK )
    {
        CComQIPtr<IAMVideoProcAmp> lpVideoProcAmp( lIMFMediaSource ); /*! initializes the IAMVideoProcAmp pointer */

        if ( lpVideoProcAmp != nullptr )
        {
            ReadAttribute<CComQIPtr<IAMVideoProcAmp>, tagVideoProcAmpProperty>( lpVideoProcAmp, VideoProcAmp_BacklightCompensation , "VideoProcAmp_BacklightCompensation" );
            ReadAttribute<CComQIPtr<IAMVideoProcAmp>, tagVideoProcAmpProperty>( lpVideoProcAmp, VideoProcAmp_Brightness,             "VideoProcAmp_Brightness" );
            ReadAttribute<CComQIPtr<IAMVideoProcAmp>, tagVideoProcAmpProperty>( lpVideoProcAmp, VideoProcAmp_Contrast,               "VideoProcAmp_Contrast" );
            ReadAttribute<CComQIPtr<IAMVideoProcAmp>, tagVideoProcAmpProperty>( lpVideoProcAmp, VideoProcAmp_ColorEnable,            "VideoProcAmp_ColorEnable" );
            ReadAttribute<CComQIPtr<IAMVideoProcAmp>, tagVideoProcAmpProperty>( lpVideoProcAmp, VideoProcAmp_Gain,                   "VideoProcAmp_Gain" );
            ReadAttribute<CComQIPtr<IAMVideoProcAmp>, tagVideoProcAmpProperty>( lpVideoProcAmp, VideoProcAmp_Gamma,                  "VideoProcAmp_Gamma" );
            ReadAttribute<CComQIPtr<IAMVideoProcAmp>, tagVideoProcAmpProperty>( lpVideoProcAmp, VideoProcAmp_Hue,                    "VideoProcAmp_Hue" );
            ReadAttribute<CComQIPtr<IAMVideoProcAmp>, tagVideoProcAmpProperty>( lpVideoProcAmp, VideoProcAmp_Saturation,             "VideoProcAmp_Saturation" );
            ReadAttribute<CComQIPtr<IAMVideoProcAmp>, tagVideoProcAmpProperty>( lpVideoProcAmp, VideoProcAmp_Sharpness,              "VideoProcAmp_Sharpness" );
            ReadAttribute<CComQIPtr<IAMVideoProcAmp>, tagVideoProcAmpProperty>( lpVideoProcAmp, VideoProcAmp_WhiteBalance,           "VideoProcAmp_WhiteBalance" );
        }
        else
        {
            std::cout << "Interface for camera not available\n" << std::endl;
        }

        std::cout << "---------------------------------\n" << std::endl;
    }

    if ( lQueryResult == S_OK )
    {
        CComQIPtr<IAMCameraControl> lpCameraControl( lIMFMediaSource ); /*! initializes the IAMVideoProcAmp pointer */

        if ( lpCameraControl != nullptr )
        {
            ReadAttribute<CComQIPtr<IAMCameraControl>, tagCameraControlProperty>( lpCameraControl, CameraControl_Exposure, "CameraControl_Exposure");
            ReadAttribute<CComQIPtr<IAMCameraControl>, tagCameraControlProperty>( lpCameraControl, CameraControl_Focus, "CameraControl_Focus");
            ReadAttribute<CComQIPtr<IAMCameraControl>, tagCameraControlProperty>( lpCameraControl, CameraControl_Iris, "CameraControl_Iris");
            ReadAttribute<CComQIPtr<IAMCameraControl>, tagCameraControlProperty>( lpCameraControl, CameraControl_Pan, "CameraControl_Pan");
            ReadAttribute<CComQIPtr<IAMCameraControl>, tagCameraControlProperty>( lpCameraControl, CameraControl_Roll, "CameraControl_Roll");
            ReadAttribute<CComQIPtr<IAMCameraControl>, tagCameraControlProperty>( lpCameraControl, CameraControl_Tilt, "CameraControl_Tilt");
            ReadAttribute<CComQIPtr<IAMCameraControl>, tagCameraControlProperty>( lpCameraControl, CameraControl_Zoom, "CameraControl_Zoom");
        }
        else
        {
            std::cout << "Interface for camera not available\n" << std::endl;
        }

        std::cout << "---------------------------------\n" << std::endl;
    }

    lQueryResult = lpDevice->DetachObject();
    lIMFMediaSource->Shutdown();
    lIMFMediaSource->Release();
    lIMFMediaSource = NULL;


/* Now we have access to the functions of both IAMCameraControl and IAMVideoProcAmp interfaces of the Captore Device */

/* Camera Control properties: https://docs.microsoft.com/en-us/previous-versions/ms779747%28v%3dvs.85%29
 * Camera Control flags:      https://docs.microsoft.com/en-us/previous-versions/ms779746%28v%3dvs.85%29
 */

 /* VideoProcAmp properties : https://docs.microsoft.com/en-us/previous-versions/ms787924%28v%3dvs.85%29
  * VideoProcAmp flags:       https://docs.microsoft.com/en-us/previous-versions/ms787923%28v%3dvs.85%29
  */
}
