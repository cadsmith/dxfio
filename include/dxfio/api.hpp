#ifndef DXFIO_API_HPP
#define DXFIO_API_HPP

// DXFIO_API marks what dxfio exports. CMake defines DXFIO_STATIC when it builds dxfio as a static library,
// and dxfio_EXPORTS while it builds dxfio as a shared one.
#ifdef DXFIO_STATIC
// A static library exports nothing, so there is nothing to mark.
#  define DXFIO_API
#else
#  if defined(_WIN32) || defined(__CYGWIN__)
#    ifdef dxfio_EXPORTS
// The DLL being built exports what is marked.
#      define DXFIO_API __declspec(dllexport)
#    else
// A program using the DLL imports what is marked.
#      define DXFIO_API __declspec(dllimport)
#    endif
#  else
#    if defined(__GNUC__) && __GNUC__ >= 4
// The shared library hides its symbols but those marked.
#      define DXFIO_API __attribute__((visibility("default")))
#    else
// A compiler with no symbol visibility marks nothing.
#      define DXFIO_API
#    endif
#  endif
#endif

#endif /* DXFIO_API_HPP */
