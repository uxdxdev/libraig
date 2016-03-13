
#ifndef raig_EXPORT_H
#define raig_EXPORT_H

#ifdef RAIG_STATIC_DEFINE
#  define raig_EXPORT
#  define RAIG_NO_EXPORT
#else
#  ifndef raig_EXPORT
#    ifdef raig_EXPORTS
        /* We are building this library */
#      define raig_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define raig_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef RAIG_NO_EXPORT
#    define RAIG_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef RAIG_DEPRECATED
#  define RAIG_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef RAIG_DEPRECATED_EXPORT
#  define RAIG_DEPRECATED_EXPORT raig_EXPORT RAIG_DEPRECATED
#endif

#ifndef RAIG_DEPRECATED_NO_EXPORT
#  define RAIG_DEPRECATED_NO_EXPORT RAIG_NO_EXPORT RAIG_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define RAIG_NO_DEPRECATED
#endif

#endif