### vadimg: this AWK script constructs an APIMON-style API table
###         from the input generated by winapi.awk.
BEGIN {
}
{
    if ($2 == "HWND") {
        szRet = "T_HWND";
    } else if ($2 == "LPSTR" || $2 == "LPCSTR") {
        szRet = "T_LPSTR";
    } else if ($2 == "LPWSTR" || $2 == "LPCWSTR") {
        szRet = "T_LPWSTR";
    } else if ($2 == "HACCEL") {
        szRet = "T_HACCEL";
    } else if ($2 == "HANDLE") {
        szRet = "T_HANDLE";
    } else if ($2 == "HBITMAP") {
        szRet = "T_HBITMAP";
    } else if ($2 == "HBRUSH") {
        szRet = "T_HBRUSH";
    } else if ($2 == "HCURSOR") {
        szRet = "T_HCURSOR";
    } else if ($2 == "HDC") {
        szRet = "T_HDC";
    } else if ($2 == "HDCLPPOINT") {
        szRet = "T_HDCLPPOINT";
    } else if ($2 == "HDESK") {
        szRet = "T_HDESK";
    } else if ($2 == "HDWP") {
        szRet = "T_HDWP";
    } else if ($2 == "HENHMETAFILE") {
        szRet = "T_HENHMETAFILE";
    } else if ($2 == "HFONT") {
        szRet = "T_HFONT";
    } else if ($2 == "HGDIOBJ") {
        szRet = "T_HGDIOBJ";
    } else if ($2 == "HGLOBAL") {
        szRet = "T_HGLOBAL";
    } else if ($2 == "HGLRC") {
        szRet = "T_HGLRC";
    } else if ($2 == "HHOOK") {
        szRet = "T_HHOOK";
    } else if ($2 == "HICON") {
        szRet = "T_HICON";
    } else if ($2 == "HINSTANCE") {
        szRet = "T_HINSTANCE";
    } else if ($2 == "HKL") {
        szRet = "T_HKL";
    } else if ($2 == "HMENU") {
        szRet = "T_HMENU";
    } else if ($2 == "HMETAFILE") {
        szRet = "T_HMETAFILE";
    } else if ($2 == "HPALETTE") {
        szRet = "T_HPALETTE";
    } else if ($2 == "HPEN") {
        szRet = "T_HPEN";
    } else if ($2 == "HRGN") {
        szRet = "T_HRGN";
    } else if ($2 == "HWINSTA") {
        szRet = "T_HWINSTA";
    } else if ($2 == "HWND") {
        szRet = "T_HWND";
    } else {
        szRet = "T_DWORD";
    }

    printf "{ \"%s\", %s", $1, szRet;

    nArgs = NF - 2;
    if (nArgs > 8) {
        nArgs = 8;
    }

    if (nArgs > 0) {
        printf ", %d, {", nArgs;

        for (i = 3; i <= 10 && i <= NF; i++) {
            szSpace = ", ";
            szToken = "";
            if (i == 3) {
                szSpace = "";
            }
                
            if ($i == "HWND") {
                szToken = "T_HWND";
            } else if ($i == "LPSTR" || $i == "LPCSTR") {
                szToken = "T_LPSTR";
            } else if ($i == "LPWSTR" || $i == "LPCWSTR") {
                szToken = "T_LPWSTR";
            } else if ($i == "HACCEL") {
                szToken = "T_HACCEL";
            } else if ($i == "HANDLE") {
                szToken = "T_HANDLE";
            } else if ($i == "HBITMAP") {
                szToken = "T_HBITMAP";
            } else if ($i == "HBRUSH") {
                szToken = "T_HBRUSH";
            } else if ($i == "HCURSOR") {
                szToken = "T_HCURSOR";
            } else if ($i == "HDC") {
                szToken = "T_HDC";
            } else if ($i == "HDCLPPOINT") {
                szToken = "T_HDCLPPOINT";
            } else if ($i == "HDESK") {
                szToken = "T_HDESK";
            } else if ($i == "HDWP") {
                szToken = "T_HDWP";
            } else if ($i == "HENHMETAFILE") {
                szToken = "T_HENHMETAFILE";
            } else if ($i == "HFONT") {
                szToken = "T_HFONT";
            } else if ($i == "HGDIOBJ") {
                szToken = "T_HGDIOBJ";
            } else if ($i == "HGLOBAL") {
                szToken = "T_HGLOBAL";
            } else if ($i == "HGLRC") {
                szToken = "T_HGLRC";
            } else if ($i == "HHOOK") {
                szToken = "T_HHOOK";
            } else if ($i == "HICON") {
                szToken = "T_HICON";
            } else if ($i == "HINSTANCE") {
                szToken = "T_HINSTANCE";
            } else if ($i == "HKL") {
                szToken = "T_HKL";
            } else if ($i == "HMENU") {
                szToken = "T_HMENU";
            } else if ($i == "HMETAFILE") {
                szToken = "T_HMETAFILE";
            } else if ($i == "HPALETTE") {
                szToken = "T_HPALETTE";
            } else if ($i == "HPEN") {
                szToken = "T_HPEN";
            } else if ($i == "HRGN") {
                szToken = "T_HRGN";
            } else if ($i == "HWINSTA") {
                szToken = "T_HWINSTA";
            } else if ($i == "HWND") {
                szToken = "T_HWND";
            } else {
                szToken = "T_DWORD";
            }

            printf "%s%s", szSpace, szToken;
        }
        
        printf "} },\n"
    }
###     printf ", 0, },\n";
}
