#include "HtmlControl.h"

CComModule _Module; 

class TestDialog : public HtmlControl
{	
public:	
	TestDialog() : HtmlControl(L"main.html")
	{
		CONNECT_JS_CALL_SIMPLE_HANDLER(Foo, TestDialog::OnFoo);
	}

	const char* OnFoo(int argc, const char* argv[])
	{
		DISPPARAMS empty = {NULL, NULL, 0, 0};
		VARIANT retval;
		CallJS(L"TestScript", &empty, &retval);
		return "ok";
	}
}; 

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	_Module.Init(NULL, hInstance, &LIBID_ATLLib);
	
	HWND parent = (HWND)0x0;
	TestDialog test;
	test.DoModal(parent);

	_Module.Term();
	return 0;
}