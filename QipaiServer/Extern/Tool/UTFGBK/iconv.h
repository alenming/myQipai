

#ifndef _LIBICONV_H
#define _LIBICONV_H

#define _LIBICONV_VERSION 0x0109    /* version number: (major<<8) + minor */
extern int _libiconv_version;       /* Likewise */

#undef iconv_t
#define iconv_t libiconv_t
typedef void* iconv_t;

/* Get size_t declaration. */
#include <stddef.h>

/* Get errno declaration and values. */
#include <errno.h>

#ifndef EILSEQ
#define EILSEQ 84
#endif


#ifdef __cplusplus
extern "C" {
#endif

#ifndef LIBICONV_PLUG
#define iconv_open libiconv_open
#endif
extern iconv_t iconv_open (const char* tocode, const char* fromcode);


#ifndef LIBICONV_PLUG
#define iconv libiconv
#endif
extern size_t iconv (iconv_t cd, const char* * inbuf, size_t *inbytesleft, char* * outbuf, size_t *outbytesleft);

/* Frees resources allocated for conversion descriptor `cd'. */
#ifndef LIBICONV_PLUG
#define iconv_close libiconv_close
#endif
extern int iconv_close (iconv_t cd);


#ifndef LIBICONV_PLUG

#define iconvctl libiconvctl
extern int iconvctl (iconv_t cd, int request, void* argument);

/* Requests for iconvctl. */
#define ICONV_TRIVIALP            0  /* int *argument */
#define ICONV_GET_TRANSLITERATE   1  /* int *argument */
#define ICONV_SET_TRANSLITERATE   2  /* const int *argument */
#define ICONV_GET_DISCARD_ILSEQ   3  /* int *argument */
#define ICONV_SET_DISCARD_ILSEQ   4  /* const int *argument */

/* Listing of locale independent encodings. */
#define iconvlist libiconvlist
extern void iconvlist (int (*do_one) (unsigned int namescount,
                                      const char * const * names,
                                      void* data),
                       void* data);

extern void libiconv_set_relocation_prefix (const char *orig_prefix,
                        const char *curr_prefix);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _LIBICONV_H */
