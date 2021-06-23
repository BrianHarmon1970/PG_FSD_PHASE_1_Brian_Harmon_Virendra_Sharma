
//#include <stdlib>
//#include <windows.h>
//#include <process.h>
#include <consoleAPI.h>
#include <fileAPI.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


using namespace std ;

// 0xB3-0xDA 
const unsigned char TL_CORNER = 201 ;
const unsigned char TR_CORNER = 187 ;

const unsigned char H_BAR = 205 ;
const unsigned char V_BAR = 186 ;

const unsigned char BL_CORNER = 200 ;
const unsigned char BR_CORNER = 188 ;

const int WIDE = 10 ;

/*
// Creates a "Pseudo Console" (ConPTY).
HRESULT WINAPI CreatePseudoConsole(
                                _In_ COORD size,        // ConPty Dimensions
                                _In_ HANDLE hInput,     // ConPty Input
                                _In_ HANDLE hOutput,    // ConPty Output
                                _In_ DWORD dwFlags,     // ConPty Flags
                                _Out_ HPCON* phPC);     // ConPty Reference

// Resizes the given ConPTY to the specified size, in characters.
HRESULT WINAPI ResizePseudoConsole(_In_ HPCON hPC, _In_ COORD size);

// Closes the ConPTY and all associated handles. Client applications attached
// to the ConPTY will also terminated.
VOID WINAPI ClosePseudoConsole(_In_ HPCON hPC);
*/

char Buffer[256] = "Brian Harmon" ;
HANDLE g_hConInput ;
DWORD  g_dwConSave ;

class ConsoleBuffer
{
} ;

class Console
{
	protected:
		HANDLE 						m_hConInput ;
		DWORD   					m_dwConSave ;
		HANDLE 						m_hConfile ;
		CONSOLE_SCREEN_BUFFER_INFO	m_sbiScreenInfo ;
	public:
	Console( void ) { }
	void OpenConsole( void )
	{
		g_hConInput = GetStdHandle( STD_INPUT_HANDLE ) ;
		if (g_hConInput == INVALID_HANDLE_VALUE)
			ErrorExit("GetStdHandle");
		// Save the current input mode, to be restored on exit.
		if ( !GetConsoleMode(g_hConInput, &g_dwConSave) )
			ErrorExit("GetConsoleMode");

		// Enable the window and mouse input events.
		DWORD ConMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
		if (! SetConsoleMode(g_hConInput, ConMode ) )
			ErrorExit("SetConsoleMode");
		
		// open the console file descriptor to get access to the charinfo buffer
		if ( !( m_hConfile = CreateFileA( "CONOUT$", GENERIC_READ | GENERIC_WRITE , 
				FILE_SHARE_WRITE | FILE_SHARE_READ , NULL, 
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL 
				)))
			ErrorExit(" CreateFile" ) ;
		else 
		{
			// load console screeninfo structure
			if( !GetConsoleScreenBufferInfo( m_hConfile, & m_sbiScreenInfo ) )
				ErrorExit( "GetConsoleScreenBufferInfo" ) ;
		}
			
			

	}
	void CloseConsole( void )
	{
		// Restore input mode.
		SetConsoleMode( m_hConInput, m_dwConSave ) ;
	}


	void FlushKeys( void )
	{


		_INPUT_RECORD inputRecord[ 128 ] ;
		unsigned long int eventCount ;


		cout << "Flushing Keys..." << endl ;


		while( PeekConsoleInput( m_hConInput, &inputRecord[0], 128, &eventCount ) && eventCount  )
		{
			//PeekConsoleInput( g_hConInput, &inputRecord[0], 1, &eventCount ) ;
			//cout  << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << eventCount  ;
			cout << "EventType: " << ( inputRecord[eventCount-1].EventType == KEY_EVENT ? "KEY_EVENT" : "OTHER" ) << endl ;
			ReadConsoleInput( g_hConInput, &inputRecord[0], 1, &eventCount ) ;
		}

	}
	VOID ErrorExit (LPSTR lpszMessage)
	{
		fprintf(stderr, "%s\n", lpszMessage);

		// Restore input mode on exit.

		SetConsoleMode( m_hConInput, m_dwConSave ) ;

		ExitProcess(0);
	}
	inline void ClearScreen( void )
	{
		cout << "\x1B[c"  ;
		cout << "\x1B[2J"  ;	//clear screen
	}
	inline void Home( void )
	{
		cout << "\x1B[1;1H" << endl ;	// set cursor position
	}
	inline void HomeColumn( void )
	{
	}
	inline int MaxRow( void )
	{
		return m_sbiScreenInfo.dwSize.Y ;
	} 
	inline int MaxColumn( void )
	{
		return m_sbiScreenInfo.dwSize.X ;
	}
	inline void SetCursor( int row, int column )
	{
		cout << "\x1B[" ;
		cout << column << ';' ;
		cout << row << 'H' ;
	} 
	void WriteString( char* str, unsigned long int x, unsigned long int y )
	{
		COORD coord ;
		coord.X = x ;
		coord.Y = y ;
		unsigned long int wCount ;
		
		WriteConsoleOutputCharacter( m_hConfile, LPCTSTR( str ), strlen( str ),
			coord, &wCount ) ;
			
	}
	void DrawBox( int x, int y, int width, int height ) 
	{
		char str[1024] ;
		str[0] = TL_CORNER ;

		//WriteStringcout << TL_CORNER ;
		for( int i = 1 ; i < width ; ++i )
			str[ i ] = H_BAR ;
		
		str[ width-1 ] = TR_CORNER ;
		str[ width ] = 0 ;
		WriteString( str, x, y) ;	
			

		str[ 0 ] = V_BAR ; str[ 1 ] = 0 ;
		for ( int i = 1 ; i < height ; ++i ) 
		{
			WriteString( str, x, y+i ) ;
			WriteString( str, x+width-1, y+i) ;
		}
		str[0] = BL_CORNER ;

		//WriteStringcout << TL_CORNER ;
		for( int i = 1 ; i < width ; ++i )
			str[ i ] = H_BAR ;
		str[ width-1 ] = BR_CORNER ;
		str[ width ] = 0 ;
		WriteString( str, x, height-1) ;	

	}
	void DrawBorder( void )
	{
		DrawBox( 0, 0, MaxColumn(), MaxRow() ) ;
	}
	

};

unsigned int g_nLineNdx ;

VOID ErrorExit (LPSTR lpszMessage) ;
void DrawBox( int width, int height ) ;

void main(void)
{
	g_hConInput = GetStdHandle( STD_INPUT_HANDLE ) ;
	ifstream file( "EchoCon.cpp" ) ;

	if (g_hConInput == INVALID_HANDLE_VALUE)
			ErrorExit("GetStdHandle");

		// Save the current input mode, to be restored on exit.
	if ( !GetConsoleMode(g_hConInput, &g_dwConSave) )
			ErrorExit("GetConsoleMode");

		// Enable the window and mouse input events.
		DWORD ConMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
		if (! SetConsoleMode(g_hConInput, ConMode ) )
			ErrorExit("SetConsoleMode");

	vector<string>	m_vstrFileLines ;

	if ( file.good() )
	{
		_INPUT_RECORD inputRecord[ 128 ] ;
		unsigned long int eventCount ;

		cout << "Flushing Keys..." << endl ;
		while( PeekConsoleInput( g_hConInput, &inputRecord[0], 128, &eventCount ) && eventCount  )
		{
			//PeekConsoleInput( g_hConInput, &inputRecord[0], 1, &eventCount ) ;
			//cout  << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << eventCount  ;
			cout << "EventType: " << ( inputRecord[eventCount-1].EventType == KEY_EVENT ? "KEY_EVENT" : "OTHER" ) << endl ;
			ReadConsoleInput( g_hConInput, &inputRecord[0], 1, &eventCount ) ;
		}

		string str ;
		while ( getline( file, str ) )
		{
			m_vstrFileLines.push_back( str )  ;
		//	std::cout << str << endl ;

		}
		g_nLineNdx = 0 ;
		int nCharNdx = 0 ;
		int nWindowHeight = 5 ;
		int nWindowWidth = 150 ;
		bool bRunning = true ;
		cout << "Running." << endl ;
		while ( bRunning )
		{
			if ( ReadConsoleInput( g_hConInput, &inputRecord[0], 128, &eventCount ))
			for ( unsigned int i = 0 ; i < eventCount ; ++i )
			{
				//cout << "EventType: " << ( inputRecord[i].EventType == KEY_EVENT ? "KEY_EVENT" : "OTHER" ) << endl ;
				if( inputRecord[i].EventType == KEY_EVENT && !inputRecord[i].Event.KeyEvent.bKeyDown )
				{
					//cout << "KeyCode: " << inputRecord[i].Event.KeyEvent.wVirtualKeyCode << endl ;
					//cout << "ScanCode: "  << inputRecord[i].Event.KeyEvent.wVirtualScanCode << endl ;

					if( inputRecord[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE/*27*/ )
						bRunning = false ;

					if( inputRecord[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN/*40*/ )
					{ g_nLineNdx = ( g_nLineNdx < m_vstrFileLines.size() ? g_nLineNdx+1 : g_nLineNdx ) ; }


					if( inputRecord[i].Event.KeyEvent.wVirtualKeyCode == VK_UP/*38*/ )
					{ g_nLineNdx = ( g_nLineNdx > 0 ? g_nLineNdx-1 : g_nLineNdx ) ; }
				
					if( inputRecord[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT )
					{
						nCharNdx++ ;
						nCharNdx = ( nCharNdx < m_vstrFileLines[ g_nLineNdx ].length()-nWindowWidth ? 
							nCharNdx : 
							m_vstrFileLines[ g_nLineNdx ].length()-nWindowWidth ) ;
					}
					if( inputRecord[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT )
						{ nCharNdx-- ; nCharNdx = ( nCharNdx < 0 ? 0 : nCharNdx ) ; }

					//cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" ;
					//cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" ;
					cout << "\x1B[2K" ; // clear line
					cout << "\x1B[1024D" ; // back up cursor
					cout << "\x1B[s" ; // save curser
					cout.flush() ;
					for ( int i = 0 ; i < nWindowHeight ; ++i )
					{
						cout << "\x1B[2K" ; // clear line
						string s = m_vstrFileLines[ g_nLineNdx+i ] ;
						if ( m_vstrFileLines[ g_nLineNdx+i ].length() - nCharNdx  > nWindowWidth )
							s = m_vstrFileLines[ g_nLineNdx+i ].substr( 0, nWindowWidth+nCharNdx ) ;	
						
						cout << g_nLineNdx+1+i << ": " << s.substr( (s.length() < nCharNdx ? s.length() : nCharNdx )) ;
						cout << endl ;
						cout.flush() ;
					}
					cout << "\x1B[u" ; // restore cursor
				}
			}
			else
			{
				cout << "READ_CONSOLE_ERROR!!" << endl ;
				bRunning = false ;
			}
			 SetConsoleMode(g_hConInput, g_dwConSave );
		}
	} else cout << "Error: BAD_FILE" << endl ;
	file.close();

	//int size = m_vstrFileLines.size() ;
	//for ( int i = 0 ; i < size ; ++i )
	//{
	//	cout << " " << i+1 << "   " << m_vstrFileLines[ i ] << endl ;
	//}

	Console c ;
	
	c.OpenConsole() ;
	//cout << "\x1B[0;0H" << endl ;	// set cursor position
	//cout << "\x1B[J"  ;	//clear screen
	c.Home() ; c.ClearScreen() ;
	c.DrawBorder() ;
	

	cout << "\x1B[44;33;1m" << endl ; // set attributes
	cout << "Hello!!!!" << endl ;
    cout<<endl ;
	for ( int i = 0 ; i < 48 ; ++i )
	{
		cout << 0xB0+i << "  " << (char)(0xB0+i) << "   " ;
		if ( !( (i+1) % 8) )
			cout << endl  ;
	} ;
	//cout << endl ;

	cout << "MaxCol=" << c.MaxColumn() ;
	cout << "MaxRow=" << c.MaxRow() ;
	//c.SetCursor( 20, 20 ) ;
	//DrawBox( c.MaxColumn() - 20 , 2 ) ;
	c.DrawBorder() ;
	//cout << "\x1B[c" ;
	//cout << endl ;
	cout << "\x1B[6n" ;
	cout.flush() ;
	c.WriteString( "WriteString ( 40,20 )", 40, 20 ) ;
	c.CloseConsole() ;
}
void DrawBox( int width, int height ) 
{

	cout << TL_CORNER ;
	for( int i = 0 ; i < width ; ++i )
		cout << H_BAR ;
	cout << TR_CORNER << endl ;

	for ( int i = 0 ; i < height ; ++i ) 
	{
		cout << V_BAR ;
		for ( int x = 0 ; x < width ; ++x )
			cout << ' ' ;
		// << " Brian  " 
		cout << V_BAR << endl ;
	}

	cout << BL_CORNER ;
	for( int i = 0 ; i < width ; ++i )
		cout << H_BAR ;
	cout << BR_CORNER << endl ;

}

VOID ErrorExit (LPSTR lpszMessage)
{
	fprintf(stderr, "%s\n", lpszMessage);

	// Restore input mode on exit.

	SetConsoleMode( g_hConInput, g_dwConSave ) ;

	ExitProcess(0);
}
/*
 // Note: Most error checking removed for brevity.  
    // ...
    
    // Initializes the specified startup info struct with the required properties and
    // updates its thread attribute list with the specified ConPTY handle
    HRESULT InitializeStartupInfoAttachedToConPTY(STARTUPINFOEX* siEx, HPCON hPC)
    {
        HRESULT hr = E_UNEXPECTED;
        size_t size;

        siEx->StartupInfo.cb = sizeof(STARTUPINFOEX);
        
        // Create the appropriately sized thread attribute list
        InitializeProcThreadAttributeList(NULL, 1, 0, &size);
        std::unique_ptr<BYTE[]> attrList = std::make_unique<BYTE[]>(size);
        
        // Set startup info's attribute list & initialize it
        siEx->lpAttributeList = reinterpret_cast<PPROC_THREAD_ATTRIBUTE_LIST>(
            attrList.get());
        bool fSuccess = InitializeProcThreadAttributeList(
            siEx->lpAttributeList, 1, 0, (PSIZE_T)&size);
            
        if (fSuccess)
        {
            // Set thread attribute list's Pseudo Console to the specified ConPTY
            fSuccess = UpdateProcThreadAttribute(
                            lpAttributeList,
                            0,
                            PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
                            hPC,
                            sizeof(HPCON),
                            NULL,
                            NULL);
            return fSuccess ? S_OK : HRESULT_FROM_WIN32(GetLastError());
        }
        else
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
        return hr;
    }
    
    // ...
    
    HANDLE hOut, hIn;
    HANDLE outPipeOurSide, inPipeOurSide;
    HANDLE outPipePseudoConsoleSide, inPipePseudoConsoleSide;
    HPCON hPC = 0;

    // Create the in/out pipes:
    CreatePipe(&inPipePseudoConsoleSide, &inPipeOurSide, NULL, 0);
    CreatePipe(&outPipeOurSide, &outPipePseudoConsoleSide, NULL, 0);

    // Create the Pseudo Console, using the pipes
    CreatePseudoConsole(
        {80, 32}, 
        inPipePseudoConsoleSide, 
        outPipePseudoConsoleSide, 
        0, 
        &hPC);

    // Prepare the StartupInfoEx structure attached to the ConPTY.
    STARTUPINFOEX siEx{};
    InitializeStartupInfoAttachedToConPTY(&siEx, hPC);

    // Create the client application, using startup info containing ConPTY info
    wchar_t* commandline = L"c:\\windows\\system32\\cmd.exe";
    PROCESS_INFORMATION piClient{};
    fSuccess = CreateProcessW(
                    nullptr,
                    commandline,
                    nullptr,
                    nullptr,
                    TRUE,
                    EXTENDED_STARTUPINFO_PRESENT,
                    nullptr,
                    nullptr,
                    &siEx->StartupInfo,
                    &piClient);

    // ...
}
*/