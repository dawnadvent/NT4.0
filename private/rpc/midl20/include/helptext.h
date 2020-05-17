/***
 *** This file should be included only in the command analyser source
 *** Do not use tabs! only spaces...
 ***/

const char *HelpArray[] = {
 "                       -MIDL COMPILER OPTIONS-"
,"                                -MODE-"
,"/ms_ext            Microsoft extensions to the IDL language (default)"
,"/c_ext             Allow Microsoft C extensions in the IDL file (default)"
,"/osf               OSF mode - disables /ms_ext and /c_ext options"
,"/app_config        Allow selected ACF attributes in the IDL file"
,"/mktyplib203       MKTYPLIB Version 2.03 compatiblity mode"
,""
,"                               -INPUT-"
,"/acf filename      Specify the attribute configuration file"
,"/I directory-list  Specify one or more directories for include path"
,"/no_def_idir       Ignore the current and the INCLUDE directories"
,""
,"                       -OUTPUT FILE GENERATION-"
,"/client none       Do not generate client files"
,"/client stub       Generate client stub file only"
,"/out directory     Specify destination directory for output files"
,"/server none       Generate no server files"
,"/server stub       Generate server stub file only"
,"/syntax_check      Check syntax only; do not generate output files"
,"/Zs                Check syntax only; do not generate output files"
,"/old               Generate old format type libraries"
,"/new               Generate new format type libraries"
,""
,"                         -OUTPUT FILE NAMES-"
,"/cstub filename    Specify client stub file name"
,"/dlldata filename  Specify dlldata file name"
,"/h filename        Specify header file name"
,"/header filename   Specify header file name"
,"/iid filename      Specify interface UUID file name"
,"/proxy filename    Specify proxy file name"
,"/sstub filename    Specify server stub file name"
,"/tlb filename      Specify type library file name"
,""
,"                -C COMPILER AND PREPROCESSOR OPTIONS-"
,"/cpp_cmd cmd_line  Specify name of C preprocessor"
,"/cpp_opt options   Specify additional C preprocessor options"
,"/D name[=def]      Pass #define name, optional value to C preprocessor"
,"/no_cpp            Turn off the C preprocessing option"
,"/nocpp             Turn off the C preprocessing option"
,"/U name            Remove any previous definition (undefine)"
,""
,"                            -ENVIRONMENT-"
,"/char signed       C compiler default char type is signed"
,"/char unsigned     C compiler default char type is unsigned"
,"/char ascii7       Char values limited to 0-127"
,"/dos               Target environment is MS-DOS client"
,"/env dos           Target environment is MS-DOS client"
,"/env mac           Target environment is Apple Macintosh"
,"/env powermac      Target environment is Apple PowerMac"
,"/env win16         Target environment is Microsoft Windows 16-bit (Win 3.x)"
,"/env win32         Target environment is Microsoft Windows 32-bit (NT)"
,"/mac               Target environment is Apple Macintosh"
,"/ms_union          Use Midl 1.0 non-DCE wire layout for non-encapsulated unions"
,"/oldnames          Do not mangle version number into names"
,"/powermac          Target environment is Apple PowerMac"
,"/rpcss             Automatically activate rpc_sm_enable_allocate"
,"/use_epv           Generate server side application calls via entry-pt vector"
,"/no_default_epv    Do not generate a default entry-point vector"
,"/prefix client str Add \"str\" prefix to client-side entry points"
,"/prefix server str Add \"str\" prefix to server-side manager routines"
,"/prefix switch str Add \"str\" prefix to switch routine prototypes"
,"/prefix all str    Add \"str\" prefix to all routines"
,"/win16             Target environment is Microsoft Windows 16-bit (Win 3.x)"
,"/win32             Target environment is Microsoft Windows 32-bit (NT)"
,""
,"                     -ERROR AND WARNING MESSAGES-"
,"/error none        Turn off all error checking options"
,"/error allocation  Check for out of memory errors"
,"/error bounds_check   Check size vs transmission length specification"
,"/error enum        Check enum values to be in allowable range"
,"/error ref         Check ref pointers to be non-null"
,"/error stub_data   Emit additional check for server side stub data validity"
,"/no_warn           Suppress compiler warning messages"
,""
,"                            -OPTIMIZATION-"
,"/align {1|2|4|8}   Designate packing level of structures"
,"/pack {1|2|4|8}    Designate packing level of structures"
,"/Zp{1|2|4|8}       Designate packing level of structures"
,"/Oi                Generate fully interpreted stubs"
,"/Oic               Generate fully interpreted stubs for standard interfaces and"
,"                   stubless proxies for object interfaces as of NT 3.51 release"
,"/Oicf              Generate fully interpreted stubs with extensions and"
,"                   stubless proxies for object interfaces as of NT 4.0 release"
,"/Os                Generate inline stubs"
,"/hookole           Generate HookOle debug info for local object interfaces"
,""
,"                           -MISCELLANEOUS-"
,"@response_file     Accept input from a response file"
,"/?                 Display a list of MIDL compiler switches"
,"/confirm           Display options without compiling MIDL source"
,"/help              Display a list of MIDL compiler switches"
,"/nologo            Supress displaying of the banner lines"
,"/o filename        Redirects output from screen to a file"
,"/W{0|1|2|3|4}      Specify warning level 0-4 (default = 1)"
,"/WX                Report warnings at specified /W level as errors"
};
