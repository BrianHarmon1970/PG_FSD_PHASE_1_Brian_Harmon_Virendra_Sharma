

#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <windows.h>

#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <Audiopolicy.h>
#include <Devicetopology.h>

#include <Endpointvolume.h>

#include <AudioSessionTypes.h>
#include <audioendpoints.h>

#include <audioenginebaseapo.h>
#include <audioengineendpoint.h>

/*
#include <spatialaudioclient.h>
#include <spatialaudiohrtf.h>
#include <spatialaudiometadata.h>
*/

#include <vector>
using namespace std ;

static GUID IDevice_Description = { 0xa45c254e, 0xdf1c, 0x4efd,
		{ 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0/*, 2*/}} ;
static GUID IDevice_Interface_FriendlyName = { 0x026e516e, 0xb814, 0x414b,
		{ 0x83, 0xcd, 0x85, 0x6d, 0x6f, 0xef, 0x48, 0x22/*, 2*/ }} ;
static GUID IDevice_FriendlyName = { 0xa45c254e, 0xdf1c, 0x4efd,
		{ 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 }} ;

class CoException
{
	HRESULT m_hresult ;
	string  m_strContextDescription ;
	string  m_strResultText ;
	string  m_strErrorMessage ;
	void GetResultText( HRESULT hr )
	{
		switch( hr )
		{
			case S_OK:   m_strResultText = "S_OK"	;	break ;
			case REGDB_E_CLASSNOTREG: m_strResultText = "REGDB_E_CLASSNOTREG" ; break ;
			case CLASS_E_NOAGGREGATION: m_strResultText = "CLASS_E_NOAGGREGATION" ; break ;
			case E_NOINTERFACE:  m_strResultText = "E_NOINTERFACE" ; break ;
			case E_POINTER:     m_strResultText = "E_POINTER" ; break ;
			case E_NOTIMPL:     m_strResultText = "E_NOTIMPL" ; break ;
			case E_ABORT:     m_strResultText = "E_ABORT" ; break ;
			case E_FAIL:     m_strResultText = "E_FAIL" ; break ;
			case E_UNEXPECTED:     m_strResultText = "E_UNEXPECTED" ;	break ;
			case E_ACCESSDENIED:     m_strResultText = "E_ACCESSDENIED" ; break ;
			case E_HANDLE:     m_strResultText = "E_HANDLE" ;	break ;
			case E_OUTOFMEMORY:    m_strResultText = "E_OUTOFMEMORY" ; break ;
			case E_INVALIDARG:     m_strResultText = "E_INVALIDARG" ; break ;
			default:    m_strResultText = "UNKNOWN HRESULT" ;
		}
	}
public:
	CoException( HRESULT hr, string description )
	{
		m_hresult = hr ;
		m_strContextDescription = "" ;
		m_strContextDescription += description ;
		GetResultText( m_hresult ) ;
	}
	string ErrorMessage( void )
	{
		m_strErrorMessage = m_strContextDescription ;
		m_strErrorMessage += ": " ;
		m_strErrorMessage += m_strResultText ;
		return m_strErrorMessage ;
	}
};
static HRESULT g_hres ;
#define CoCall( context_msg, call_code) \
	g_hres = (call_code) ; \
	if ( g_hres != S_OK ) \
		throw new CoException( g_hres, context_msg ) ;

#define CoRelease( punk ) 	\
	if ( punk != NULL ) 	\
	{     					\
		punk->Release() ; 	\
		punk = NULL ; 		\
	}


class Device
{
	wstring				m_wstrDevId ;
	IMMDevice           *m_pDevice ;
	IPropertyStore 		*m_pPropertyStore ;

	wstring DeviceInterfaceName ;
	wstring DeviceDescription ;
	wstring DeviceName ;
public:
	Device( )
	{
		m_pDevice = NULL ;
		m_pPropertyStore = NULL ;
	}
	~Device()
	{
		CoRelease( m_pPropertyStore ) ;
		CoRelease( m_pDevice ) ;
	}
	Device( IMMDevice *pdev ) { m_pDevice = pdev ; }
	void getProperties( void )
	{
		LPWSTR pstrId ;
		m_pDevice->GetId( &pstrId ) ;
		m_wstrDevId = *pstrId ;
		CoTaskMemFree(pstrId);

		static PROPERTYKEY key;
		PROPVARIANT PropVar ;
		CoCall( "OpenPropertyStore",
			m_pDevice->OpenPropertyStore( STGM_READ, &m_pPropertyStore )
		);
		PropVariantInit( &PropVar ) ;

		key.pid = 2 ;
		key.fmtid = IDevice_Description ;
		CoCall(
			"GetDeviceDescription",
			m_pPropertyStore->GetValue( key, &PropVar )
		) ;
		DeviceDescription = PropVar.pwszVal ;

		key.pid = 2 ;
		key.fmtid = IDevice_Interface_FriendlyName ;
		CoCall(
			"GetIFName",
			m_pPropertyStore->GetValue( key, &PropVar )
		);
		DeviceInterfaceName = PropVar.pwszVal ;

		key.fmtid = IDevice_FriendlyName;
		key.pid = 14;
		CoCall(
			"GetDeviceName",
			m_pPropertyStore->GetValue( key, &PropVar )
		);
		DeviceName = PropVar.pwszVal ;
		PropVariantClear( &PropVar ) ;
	}
	wstring IFName( void ) { return DeviceInterfaceName ;  }
	wstring Name( void ) { return DeviceName ; }
	wstring Description( void ) { return DeviceDescription ; }
	/*
	IAudioClient
	IAudioEndpointVolume
	IAudioMeterInformation
	IAudioSessionManager
	IAudioSessionManager2
	IBaseFilter
	IDeviceTopology
	IDirectSound
	IDirectSound8
	IDirectSoundCapture
	IDirectSoundCapture8
	IMFTrustedOutput
	*/
	void GetInterface( REFIID iftype,
		void **ifAudio ,
		unsigned long clsCtx = CLSCTX_ALL )
	{
		CoCall(
			"Device::GetInterface",
			m_pDevice->Activate( iftype, clsCtx, NULL, (void**)ifAudio )
		) ;
	}


};
class AudioInterface
{
protected:
	REFIID  m_idInterfaceType ;
	void    **m_ppInterface ;
	Device  *m_pDevice ;
public:
	AudioInterface( void **ppIf, REFIID iType ) : m_idInterfaceType( iType )
	{
		m_ppInterface = ppIf ;
	}

	void Activate( void )
	{
		m_pDevice->GetInterface( m_idInterfaceType, m_ppInterface ) ;

	}
    bool    IfOk( void ) { return ( *m_ppInterface != NULL ? true : false ) ; }
};

const IID IID_IAudioClient = __uuidof( IAudioClient );
const IID IID_IAudioEndpointVolume = __uuidof(IAudioEndpointVolume );
const IID IID_IAudioMeterInformation = __uuidof( IAudioMeterInformation );

class AudioClient : public AudioInterface
{
	IAudioClient* 	m_pAudioClient ;
public:
	AudioClient( Device d ) :
		AudioInterface( (void**)&m_pAudioClient, IID_IAudioClient )
	{
		m_pDevice = &d ;
		Activate() ;
	}
};

class AudioEndpointVolumeInterface : public AudioInterface
{
	IAudioEndpointVolume    *m_pAudioEndpointVolume ;

	// Volume Range Specifiers
	float                   m_fDBMin ;
	float                   m_fDBMax ;
	float                   m_fDBIncrement ;

public:
	AudioEndpointVolumeInterface( Device d )  :
		AudioInterface( (void**)&m_pAudioEndpointVolume, IID_IAudioEndpointVolume )
	{
		m_pDevice = &d ;
		Activate() ;
		getVolumeRange() ;
	}

//    IAudioEndpointVolume::SetChannelVolumeLevel
	void SetVolumeLevel( int channel, float volume )
	{
		CoCall(
			"SetVolumeLevel( channel )" ,
			m_pAudioEndpointVolume->SetChannelVolumeLevel( channel, volume, NULL )
		) ;
	}
	void getVolumeRange( void )
	{
		CoCall(
			"AudioEndpointVolumeInterface::getVolumeRange",
			m_pAudioEndpointVolume->GetVolumeRange(
			   &m_fDBMin , &m_fDBMax, &m_fDBIncrement )
		) ;
	}
	float   MaxDB( void ) { return m_fDBMax ; }
	float   MinDB( void ) { return m_fDBMin ; }
	float   IncrementDB( void ) { return m_fDBIncrement ; }

//	IAudioEndpointVolume::SetChannelVolumeLevelScalar
	void SetVolumeScalar( int channel ) ;
//	IAudioEndpointVolume::SetMasterVolumeLevel
	void SetMasterVolume( float level )
	{
		CoCall( "AudioEndpointVolumeInterface::SetMasterVolume",
			m_pAudioEndpointVolume->SetMasterVolumeLevel(  level, NULL )
       ) ;
    }
//	IAudioEndpointVolume::SetMasterVolumeLevelScalar
	void SetMasterScalar( void ) ;
//	IAudioEndpointVolume::SetMute
	void Mute( void ) ;
//	IAudioEndpointVolume::VolumeStepDown
	void VolumeUp( void ) ;
//	IAudioEndpointVolume::VolumeStepUp
	void VolumeDown( void ) ;

};
const int MAX_CHANNELS = 128 ;
class AudioMeter : public AudioInterface
{
	IAudioMeterInformation*    	m_pAudioMeter ;

	bool                        m_bHardwareMeter ;
	bool                        m_bHardwareVolume ;
	bool                        m_bHardwareMute ;
	bool                        m_bHardwareSupport ;
	unsigned long               m_nHardwareSupportMask ;
	unsigned int				m_nChannelCount ;
	float                       m_nPeakValue ;
	float                       m_afChannelPeakValues[ MAX_CHANNELS ] ;
public:
	AudioMeter( Device *d ) :
		AudioInterface( (void**)&m_pAudioMeter, IID_IAudioMeterInformation )
	{
		//m_pAudioMeter = NULL ;
		m_nChannelCount = 0 ;

		m_pDevice = d ;
		Activate() ;
		//QueryHardwareSupport( ) ;
	}
	~AudioMeter()
	{
		if( m_pAudioMeter != NULL )
		{
			m_pAudioMeter->Release() ;
			m_pAudioMeter = NULL ;
		}
	}

	bool QueryHardwareSupport( void )
	{
		unsigned long supportmask ;
		CoCall(
			"AudioMeter::QueryHardwareSupport",
			m_pAudioMeter->QueryHardwareSupport( &supportmask )
	   );
	   m_bHardwareSupport = ( supportmask ? true : false ) ;
	   m_bHardwareMeter = (
		(supportmask & ENDPOINT_HARDWARE_SUPPORT_METER ) ?
			true : false ) ;
	   m_bHardwareVolume = (
		( supportmask & ENDPOINT_HARDWARE_SUPPORT_VOLUME ) ?
			true : false ) ;
	   m_bHardwareMute = (
		( supportmask & ENDPOINT_HARDWARE_SUPPORT_MUTE ) ?
			true : false ) ;
	   m_nHardwareSupportMask = supportmask ;
	   return m_bHardwareSupport ;
	}
	bool hasHardwareSupport( void ) { return m_bHardwareSupport ; }
	bool hasHardwareVolume( void ) { return m_bHardwareVolume ; }
	bool hasHardwareMeter( void ) { return m_bHardwareMeter ; }
	bool hasHardwareMute( void ) { return m_bHardwareMute ; }

	unsigned long ChannelCount( void )
	{
		CoCall
		(
			"AudioMeter::ChannelCount",
		   m_pAudioMeter->GetMeteringChannelCount( &m_nChannelCount )
		) ;
		return ( m_nChannelCount ) ;
	}
	float*    ChannelsPeakValues( void )
	{
		CoCall
		(
			"AudioMeter::ChannelsPeakValues",
		   m_pAudioMeter->GetChannelsPeakValues
		   (
				m_nChannelCount,
				&m_afChannelPeakValues[0]
		   )
		) ;
		return ( m_afChannelPeakValues ) ;

	}
	float PeakValue( void )
	{
		CoCall
		(
			"AudioMeter::PeakValue",
		   m_pAudioMeter->GetPeakValue( &m_nPeakValue )
		) ;
		return ( m_nPeakValue ) ;
	}
};

class DeviceCollection
{
	IMMDeviceEnumerator *m_pEnumerator ;
	IMMDeviceCollection *m_pEndPoints ;
	std::vector<Device*>       m_vDevices ;
	unsigned     m_nDeviceCount ;
public:
	DeviceCollection( void )
	{
		m_pEnumerator = NULL ;
		m_pEndPoints = NULL ;
		m_nDeviceCount = 0 ;
	}
	~DeviceCollection()
	{
		int size = m_vDevices.size() ;
		for ( int i = size-1 ; i >= size ; --i )
		{
			delete m_vDevices[ i ] ;
		}
		CoRelease( m_pEndPoints ) ;
		CoRelease( m_pEnumerator ) ;
	}
	void Initialize( void )
	{
		const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
		const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

		//CoCall( "CoInit", CoInitialize( 0 )) ;
		CoCall( "CoCreate", CoCreateInstance(
			 CLSID_MMDeviceEnumerator, NULL,


			 CLSCTX_ALL,
			 IID_IMMDeviceEnumerator,
			 (void**)&m_pEnumerator )
		);

		CoCall( "EnumerateEndPoints",
			m_pEnumerator->EnumAudioEndpoints(
			eAll,
			DEVICE_STATE_ACTIVE |
			DEVICE_STATE_DISABLED /*|
			DEVICE_STATE_NOTPRESENT |
			DEVICE_STATE_UNPLUGGED*/,
			&m_pEndPoints )
		) ;

		if ( m_pEndPoints != NULL )
			m_pEndPoints->GetCount( &m_nDeviceCount ) ;

		for ( unsigned n = 0 ; n < m_nDeviceCount ; ++n )
		{
			IMMDevice   *pdev ;
			m_pEndPoints->Item( n, &pdev ) ;
			m_vDevices.push_back( new Device( pdev ) ) ;
			m_vDevices[ n ]->getProperties() ;
		}
	}
	vector<Device*>& Devices( void ) { return m_vDevices ; }
};
							   /*
int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		DeviceCollection DeviceEnumeration ;
		DeviceEnumeration.Initialize() ;
		int n = DeviceEnumeration.Devices().size() ;
		for ( int i = 0 ; i < n ; ++i )
		{
			//cout << endl ;
			wstring w = DeviceEnumeration.Devices().at( i )->Name() ;
			std::string str( w.begin(), w.end() );
			cout << "[ " << i << " ]"  ;
			cout << " Name: " << str.data()  << std::endl ;

			w = DeviceEnumeration.Devices().at( i )->Description() ;
			string desc_str( w.begin(), w.end() ) ;
			cout << "[ " << i << " ]"  ;
			cout << " Desc: " << desc_str.data()  << endl ;

			w = DeviceEnumeration.Devices().at( i )->IFName() ;
			string ifname_str( w.begin(), w.end() ) ;
			cout << "[ " << i << " ]"  ;
			cout << " IFName: " << ifname_str.data() << endl ;

			// Query the Device interface
			AudioMeter *meter = new AudioMeter( *DeviceEnumeration.Devices().at( i )) ;
			if ( meter->IfOk() )
			{
				int channelcount ;
				channelcount = meter->ChannelCount() ;
				bool hwSupport = ( meter->hasHardwareSupport() ? true : false ) ;
				cout << " HwMeter: " << ( hwSupport ? "Yes." : "No." ) << endl ;
				cout << " Channels: " << channelcount << endl ;
				cout << " Levels: " << meter->PeakValue() ;

				//if ( i == 3 )

				for ( int j = 0 ; j < 100  ; ++j )
				{
					Sleep( 10 ) ;
					cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" ;
					cout << " Levels: " << meter->PeakValue() ;

				}
				cout << endl ;

				if ( meter->ChannelCount() > 1 )
				{
					for ( int j = 0 ; j < 100  ; ++j )
					{
						Sleep( 10 ) ;
						cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" ;
						cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" ;
						cout << " Levels: " ;
						cout << "One: " << meter->ChannelsPeakValues()[0] ;
						cout << "	Two: " << meter->ChannelsPeakValues()[1] ;
					}
				}
				cout << endl ;
				delete meter ;
			}
		}
		cout << "Done. ( Press key to end )" << endl ;
		getch() ;
	}
	catch( CoException *e )
	{
		cout << e->ErrorMessage().data() << endl ;
		delete e ;
		getch() ;
	}
	return 0;
}
								 */