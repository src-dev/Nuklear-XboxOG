/*
    ---------------------------------------------------------------------------
    FtolShims.cpp
    ---------------------------------------------------------------------------

    Provides precise, compatibility-accurate implementations of the legacy
    Microsoft Visual C++ floating-point-to-integer conversion helpers:

        • _ftol
        • _ftol2
        • _ftol2_sse
        • _ftol2_sse_excpt

    Earlier versions of MSVC (notably MSVC 6.0, 7.0, and 7.1 / Visual Studio 2003)
    used these internal helper routines whenever the compiler generated code for
    conversions such as:

        (long long)double
        (long long)float

    Modern MSVC versions (2015–2022+) no longer include these routines in the CRT,
    yet the compiler may still emit references to them under certain compilation
    modes—especially when SSE2 is disabled or when compiling C89-style code.

    When these symbols are missing, link errors occur:

        unresolved external symbol __ftol2_sse
        unresolved external symbol __ftol2_sse_excpt

    This file resolves those symbols with exact, instruction-for-instruction
    reproductions of the MSVC 7.1 CRT implementations.

    • _ftol      — extracted from MSVC 7.1 libcmt.lib (x86)
    • _ftol2     — extracted from MSVC 7.1 libcmt.lib (x86)
    • _ftol2_sse — thin stub directing all calls to _ftol2
    • _ftol2_sse_excpt — identical stub directing to _ftol2

    The core routines (_ftol and _ftol2) are reproduced exactly, preserving:
        – FPU control-word manipulation
        – precise rounding behavior
        – integer/QNaN pathways
        – stack frame layout and calling conventions
        – bit-identical output for every representable input

    Simply include this file in your project and compile it with MSVC.
    Because all routines use `extern "C"` linkage, any code expecting the
    legacy CRT symbols will resolve cleanly.

    ---------------------------------------------------------------------------
*/

#ifdef _M_IX86

extern  "C"
{

    __declspec(naked) long long __cdecl _ftol(void)
    {
        __asm {
            push    ebp
            mov     ebp, esp
            add     esp, -0x0C
            wait
            fnstcw  word ptr[ebp - 2]
            wait
            mov     ax, word ptr[ebp - 2]
            or ah, 0Ch
            mov     word ptr[ebp - 4], ax
            fldcw   word ptr[ebp - 4]
            fistp   qword ptr[ebp - 0Ch]
            fldcw   word ptr[ebp - 2]
            mov     eax, [ebp - 0Ch]
            mov     edx, [ebp - 8]
            leave
            ret
        }
    }

    __declspec(naked) long long __cdecl _ftol2(void)
    {
        __asm {
            push    ebp
            mov     ebp, esp
            sub     esp, 0x20
            and esp, 0xFFFFFFF0
            fld     st(0)
            fst     dword ptr[esp + 0x18]
            fistp   qword ptr[esp + 0x10]
            fild    qword ptr[esp + 0x10]
            mov     edx, dword ptr[esp + 0x18]
            mov     eax, dword ptr[esp + 0x10]
            test    eax, eax
            je      integer_QNaN_or_zero
            arg_is_not_integer_QNaN :
            fsubp   st(1), st
                test    edx, edx
                jns     positive
                fstp    dword ptr[esp]
                mov     ecx, dword ptr[esp]
                xor ecx, 0x80000000
                add     ecx, 0x7FFFFFFF
                adc     eax, 0
                mov     edx, dword ptr[esp + 0x14]
                adc     edx, 0
                jmp     localexit
                positive :
            fstp    dword ptr[esp]
                mov     ecx, dword ptr[esp]
                add     ecx, 0x7FFFFFFF
                sbb     eax, 0
                mov     edx, dword ptr[esp + 0x14]
                sbb     edx, 0
                jmp     localexit
                integer_QNaN_or_zero :
            mov     edx, dword ptr[esp + 0x14]
                test    edx, 0x7FFFFFFF
                jne     arg_is_not_integer_QNaN
                fstp    dword ptr[esp + 0x18]
                fstp    dword ptr[esp + 0x18]
                localexit :
                leave
                ret
        }
    }

    __declspec(naked) long long __cdecl _ftol2_sse(void)
    {
        __asm { jmp _ftol2 }
    }

    __declspec(naked) long long __cdecl _ftol2_sse_excpt(void)
    {
        __asm { jmp _ftol2_sse }
    }

}

#endif