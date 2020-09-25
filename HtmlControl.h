#pragma once

#include <atlbase.h>
#include <atlwin.h>
#include <atltypes.h>
#include <string>
#include <map>

extern CComModule _Module; 

#define CONNECT_JS_CALL_HANDLER(name, func) ConnectHandler(L#name, (CallbackFunction)&func)
#define CONNECT_JS_CALL_SIMPLE_HANDLER(name, func) ConnectHandler(L#name, (CallbackFunction)&func, true)

/**
 * A wrapper for html based dialogs
 */
class HtmlControl : public CWindowImpl<HtmlControl>, public DWebBrowserEvents2
{	
public:	
	HtmlControl(BSTR start);
	virtual ~HtmlControl() {}
	int DoModal(HWND parent = NULL);
	HRESULT CallJS(BSTR func, DISPPARAMS* params, VARIANT* retval);
private:
	HWND parent;
	CComBSTR startPagePath;
	CAxWindow wndIE;	

private: // IDispatch methods
	STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef(void) { return 1; }
	ULONG STDMETHODCALLTYPE Release(void) { return 1; }
	STDMETHOD(GetTypeInfoCount)(UINT*) { return E_NOTIMPL; }
	STDMETHOD(GetTypeInfo)(UINT, LCID, ITypeInfo**) { return E_NOTIMPL; }

protected: // IDispatch methods used for calls from JS
	STDMETHOD(GetIDsOfNames)(REFIID, LPOLESTR *rgszNames, UINT, LCID, DISPID *rgDispId);
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID, LCID, WORD, DISPPARAMS* pDispParams, 
					  VARIANT* pVarResult, EXCEPINFO*, UINT*);

private: // default JS event handlers
	void OnShow(DISPPARAMS*, VARIANT*);
	void OnClose(DISPPARAMS*, VARIANT*);
	void OnSetTitle(DISPPARAMS*, VARIANT*);
	void OnDragWindow(DISPPARAMS*, VARIANT*);
	void OnResizeWindow(DISPPARAMS*, VARIANT*);
	void OnMinimizeWindow(DISPPARAMS*, VARIANT*);
	void OnToggleMaximizeNormalWindow(DISPPARAMS*, VARIANT*);
	void OnSetOpacity(DISPPARAMS* params, VARIANT*);
	bool isMaximized;

private: // window message handlers
	int MessageLoop();
	BEGIN_MSG_MAP(HtmlControl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)		
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_SIZING, OnSizing)
	END_MSG_MAP()
	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnKeyDown(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnSizing(UINT, WPARAM, LPARAM, BOOL&);

public: // visual effects
	static const int CutSize = 0;
private:	
	void RemoveFrame();
	void FitToHost();

public: // js calls 
	typedef void (HtmlControl::*CallbackFunction)(DISPPARAMS* params, VARIANT* retval);
	typedef const char* (HtmlControl::*SimpleCallbackFunction)(int argc, const char** argv);
protected:
	// simple callback is defined as char* func(int argc, char* argv[])
	void ConnectHandler(BSTR name, CallbackFunction func, bool simple = false);
private:
	DISPID NameId(BSTR name) const;
	HRESULT Invoke(DISPID id, DISPPARAMS* params, VARIANT* retval);
	char* InPlaceWToA(BSTR bstr) const;
	struct JsCallHandler 
	{ 
		DISPID id; 
		union { CallbackFunction normal; SimpleCallbackFunction simple; } callback;
		bool simple;
	};
	static const DISPID BASE_DIPSPID = 100000;
	typedef std::map<std::wstring, JsCallHandler> JsHandlers;
	JsHandlers jsHandlers;
};