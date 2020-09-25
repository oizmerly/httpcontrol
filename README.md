This is an old project ~ from 2005. Not sure wheter the code still workls.

# Introduction
A simple, yet powerful, lightweight, flexible and cool looking user interface controls. The main idea is to use the Internet Explorer engine to display the user interface. Thus it is possible to use the reach functionality provided by the combination of HTML, CSS and JavaScript. The HTML document's JavaScript may call C++ functions and vice versa. The attached example demonstrates a simple HTML based dialog with interaction between the document's JavaScript and C++ backend.

This library may be used as a "black-box" and doesn't require any specific knowledge about Windows programming. Yet, in order to understand the details, some COM background would be very helpful.

# Using the Code
The simplest dialog may be displayed by the next code:

```
HtmlControl dlg("hello_world.html");
dlg.DoModal();
```
`hello_world.html` is a file that is placed into the resource and the compiler inserts it into the application executable. Resource file may be created manually, for example:

```
main.html       HTML "main.html"
HtmlControl.js  JS   "HtmlControl.js"
```
This resource includes two files: main.html and `HtmlControl.js`. For the sake of simplicity, I usually give the resource item name identical to the file name. `HtmlControl.js` may be accessed from the document using:

```
< script src="JS/HtmlControl.js"> </script>
```

In order to call C++ functions from within the HTML document JavaScript, you need to inherit a class from HtmlControl and declare handlers for JavaScript calls.

```
class TestDialog : public HtmlControl
{
public:
    TestDialog() : HtmlControl(L"test.html") {
        CONNECT_JS_CALL_SIMPLE_HANDLER(Foo, TestDialog::OnFoo);
    }
    const char* OnFoo(int argc, const char* argv[]) { return "ok"; }
};
```

In other words, TestDialog declares a JavaScript call handler TestDialog::OnFoo. This handler is called from JavaScript using:

```
var retval = external.Foo("param1", "param2", "param3", ...)
```

A C++ handler receives an array of arguments argv (array size is argc). For the sake of simplicity, I defined `CONNECT_JS_CALL_SIMPLE_HANDLER` macro that translates all parameters and return values into strings `(char*). CONNECT_JS_CALL_HANDLER` provides you with full control on the arguments sent from the JavaScript.

# Background
Now, let's try to understand some nontrivial implementation details.

Host Window and Initialization
Basically, there are no limits when you choose the host window implementation. It may be a simple Win32 window, MFC or WTL based window. I decided to create the host window manually using ATL. This reduces the application footprint and removes unnecessary dependencies to other libraries.

In HtmlControl::DoModal, a borderless and titlebarless window is created and a window message loop is initiated. All initialization is performed when the message loop receives a WM_CREATE window message. On this message, the browser control is created and it is navigated to the control HTML page. Also HtmlControl is registered as an external dispatcher (SetExternalDispatch).

# JavaScript - C++ Communication
The communication between the HTML document JavaScript and C++ backend is based on COM technology. The callee is a COM object that must implement the IDispatch interface. This interface has two methods used for communication:

```
HRESULT GetIDsOfNames(REFIID riid,
                      LPOLESTR *rgszNames,
                      UINT cNames,
                      LCID lcid,
                      DISPID *rgDispId);
HRESULT Invoke(DISPID dispIdMember,
               REFIID riid,
               LCID lcid,
               WORD wFlags,
               DISPPARAMS *pDispParams,
               VARIANT *pVarResult,
               EXCEPINFO *pExcepInfo,
               UINT *puArgErr);
```

When a caller calls some function, the callee instance should return the unique id for this function. GetIDsOfNames does this. It receives the function name gszNames (as you probably noticed, it may receive several names) and returns its id rgDispId. Immediately after, Invoke is called with dispIdMember parameter equal to id returned by GetIDsOfNames. Along with function id, Invoke receives all parameters pDispParams and may return some value via pVarResult.

In the provided source code, you may find a simple yet flexible implementation of this technology. Also, for the sake of simplicity, I wrote several macros that hide the details of implementation.

# Keyboard Events
There is a subtle issue with WM_KEYDOWN and WM_KEYUP messages. The host window eats them up, thus the HTML control doesn't receive keyboard events. The solution is straightforward - in the message loop, these messages are redirected to the window of HTML control.
