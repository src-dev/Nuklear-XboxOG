/*
    -----------------------------------------------------------------------------
    Floating-point to integer conversion shims

    These functions provide stub implementations of legacy MSVC helpers
    (_ftol, _ftol2, _ftol2_sse, _ftol2_sse_excpt) that older x86 binaries
    may reference but modern CRTs no longer supply.

    Each function simply casts a double to a long, offering a minimal
    compatibility layer for projects using a custom or reduced runtime.
    -----------------------------------------------------------------------------
*/

extern "C" {

    long __cdecl _ftol(double x)
    {
        return (long)x;
    }

    long __cdecl _ftol2(double x)
    {
        return (long)x;
    }

    long __cdecl _ftol2_sse(double x)
    {
        return (long)x;
    }

    long __cdecl _ftol2_sse_excpt(double x)
    {
        return (long)x;
    }
}