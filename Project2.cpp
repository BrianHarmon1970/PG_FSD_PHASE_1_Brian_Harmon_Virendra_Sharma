//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "SoundAPI.h"
//DeviceCollection *g_pDevices ;
//---------------------------------------------------------------------------
USEFORM("Unit1.cpp", Form1);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{

  //		g_pDevices = new DeviceCollection ;
  //		g_pDevices->Initialize() ;

		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TForm1), &Form1);
		Application->Run();

   //     delete g_pDevices ;
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch ( CoException *e )
	{

		UnicodeString str ;
		std::string cstr = e->ErrorMessage() ;
		str.StringOfChar( *cstr.data(), 128 ) ;

		ShowMessage( "Fuck You!!!!" ) ;
//        delete e ;
    }
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
