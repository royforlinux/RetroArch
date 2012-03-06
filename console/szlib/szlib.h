/* szlib.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995-2002 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* This source as presented is a modified version of original zlib for use with SSNES,
 * and must not be confused with the original software. */

#ifndef _SZLIB_H
#define _SZLIB_H

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "zconf.h"

/* constants */
#define ZLIB_VERSION "1.1.4"

#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1 /* will be removed, use Z_SYNC_FLUSH instead */
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4
/* Allowed flush values; see deflate() below for details */

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)

/* Return codes for the compression/decompression functions. Negative
 * values are errors, positive values are used for special but normal events.
 */

#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)
/* compression levels */

#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_DEFAULT_STRATEGY    0

#define Z_BINARY   0
#define Z_ASCII    1
#define Z_UNKNOWN  2
/* Possible values of the data_type field */

#define Z_DEFLATED   8
/* The deflate compression method (the only one supported in this version) */

#define Z_NULL  0  /* for initializing zalloc, zfree, opaque */

#define zlib_version zlibVersion()
/* for compatibility with versions < 1.0.2 */

typedef voidpf (*alloc_func) (voidpf opaque, unsigned int items, unsigned int size);
typedef void   (*free_func)  (voidpf opaque, voidpf address);

typedef struct z_stream_s {
    Bytef    *next_in;  /* next input byte */
    unsigned int     avail_in;  /* number of bytes available at next_in */
    unsigned long    total_in;  /* total nb of input bytes read so far */

    Bytef    *next_out; /* next output byte should be put there */
    unsigned int     avail_out; /* remaining free space at next_out */
    unsigned long    total_out; /* total nb of bytes output so far */

    char     *msg;      /* last error message, NULL if no error */
    struct internal_state *state; /* not visible by applications */

    alloc_func zalloc;  /* used to allocate the internal state */
    free_func  zfree;   /* used to free the internal state */
    voidpf     opaque;  /* private data object passed to zalloc and zfree */

    int     data_type;  /* best guess about the data type: ascii or binary */
    unsigned long   adler;      /* adler32 value of the uncompressed data */
    unsigned long   reserved;   /* reserved for future use */
} z_stream;

typedef z_stream *z_streamp;

typedef unsigned char  uch;
typedef unsigned char uchf;
typedef unsigned short ush;
typedef unsigned short ushf;
typedef unsigned long  ulg;

extern const char *z_errmsg[10]; /* indexed by 2-zlib_error */
/* (size given to avoid silly warnings with Visual C++) */

#define ERR_MSG(err) z_errmsg[Z_NEED_DICT-(err)]

#define ERR_RETURN(strm,err) \
  return (strm->msg = (char*)ERR_MSG(err), (err))
/* To be used only when the state is known to be valid */

        /* common constants */

#ifndef DEF_WBITS
#  define DEF_WBITS MAX_WBITS
#endif
/* default windowBits for decompression. MAX_WBITS is for compression only */

#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
/* default memLevel */

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

        /* target dependencies */

#ifdef WIN32 /* Window 95 & Windows NT */
#  define OS_CODE  0x0b
#endif

#if defined(ATARI) || defined(atarist)
#  define OS_CODE  0x05
#endif

#if defined(MACOS) || defined(TARGET_OS_MAC)
#  define OS_CODE  0x07
#  if defined(__MWERKS__) && __dest_os != __be_os && __dest_os != __win32_os
#    include <unix.h> /* for fdopen */
#  else
#    ifndef fdopen
#      define fdopen(fd,mode) NULL /* No fdopen() */
#    endif
#  endif
#endif

#ifdef __50SERIES /* Prime/PRIMOS */
#  define OS_CODE  0x0F
#endif

#ifdef TOPS20
#  define OS_CODE  0x0a
#endif

#if defined(_BEOS_) || defined(RISCOS)
#  define fdopen(fd,mode) NULL /* No fdopen() */
#endif

#if (defined(_MSC_VER) && (_MSC_VER > 600))
#  define fdopen(fd,type)  _fdopen(fd,type)
#endif

/* Common defaults */

#ifndef OS_CODE
#  define OS_CODE  0x03  /* assume Unix */
#endif

/* functions */

#ifdef HAVE_STRERROR
   extern char *strerror (int);
#  define zstrerror(errnum) strerror(errnum)
#else
#  define zstrerror(errnum) ""
#endif

typedef unsigned long ( *check_func) (unsigned long check, const Bytef *buf,
				       unsigned int len);
voidpf zcalloc (voidpf opaque, unsigned items, unsigned size);
void   zcfree  (voidpf opaque, voidpf ptr);

#define ZALLOC(strm, items, size) \
           (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))

/* Huffman code lookup table entry--this entry is four bytes for machines
   that have 16-bit pointers (e.g. PC's in the small or medium model). */

typedef struct inflate_huft_s inflate_huft;

struct inflate_huft_s {
  union {
    struct {
      Byte Exop;        /* number of extra bits or operation */
      Byte Bits;        /* number of bits in this code or subcode */
    } what;
    unsigned int pad;           /* pad structure to a power of 2 (4 bytes for */
  } word;               /*  16-bit, 8 bytes for 32-bit int's) */
  unsigned int base;            /* literal, length base, distance base,
                           or table offset */
};

/* Maximum size of dynamic tree.  The maximum found in a long but non-
   exhaustive search was 1004 huft structures (850 for length/literals
   and 154 for distances, the latter actually the result of an
   exhaustive search).  The actual maximum is not known, but the
   value below is more than safe. */
#define MANY 1440

extern int inflate_trees_bits (
    unsigned int *,                    /* 19 code lengths */
    unsigned int *,                    /* bits tree desired/actual depth */
    inflate_huft **,       /* bits tree result */
    inflate_huft *,             /* space for trees */
    z_streamp);                /* for messages */

extern int inflate_trees_dynamic (
    unsigned int,                       /* number of literal/length codes */
    unsigned int,                       /* number of distance codes */
    unsigned int *,                    /* that many (total) code lengths */
    unsigned int *,                    /* literal desired/actual bit depth */
    unsigned int *,                    /* distance desired/actual bit depth */
    inflate_huft **,       /* literal/length tree result */
    inflate_huft **,       /* distance tree result */
    inflate_huft *,             /* space for trees */
    z_streamp);                /* for messages */

extern int inflate_trees_fixed (
    unsigned int *,                    /* literal desired/actual bit depth */
    unsigned int *,                    /* distance desired/actual bit depth */
    inflate_huft **,       /* literal/length tree result */
    inflate_huft **,       /* distance tree result */
    z_streamp);                /* for memory allocation */

struct inflate_blocks_state;
typedef struct inflate_blocks_state inflate_blocks_statef;

extern inflate_blocks_statef * inflate_blocks_new (
    z_streamp z,
    check_func c,               /* check function */
    unsigned int w);                   /* window size */

extern int inflate_blocks (
    inflate_blocks_statef *,
    z_streamp ,
    int);                      /* initial return code */

extern void inflate_blocks_reset (
    inflate_blocks_statef *,
    z_streamp ,
    unsigned long *);                  /* check value on output */

extern int inflate_blocks_free (
    inflate_blocks_statef *,
    z_streamp);

struct inflate_codes_state;
typedef struct inflate_codes_state inflate_codes_statef;

extern inflate_codes_statef *inflate_codes_new (
    unsigned int, unsigned int,
    inflate_huft *, inflate_huft *,
    z_streamp );

extern int inflate_codes (
    inflate_blocks_statef *,
    z_streamp ,
    int);

typedef enum {
      TYPE,     /* get type bits (3, including end bit) */
      LENS,     /* get lengths for stored */
      STORED,   /* processing stored block */
      TABLE,    /* get table lengths */
      BTREE,    /* get bit lengths tree for a dynamic block */
      DTREE,    /* get length, distance trees for a dynamic block */
      CODES,    /* processing fixed or dynamic block */
      DRY,      /* output remaining window bytes */
      DONE,     /* finished last block, done */
      BAD}      /* got a data error--stuck here */
inflate_block_mode;

/* inflate blocks semi-private state */
struct inflate_blocks_state {

  /* mode */
  inflate_block_mode  mode;     /* current inflate_block mode */

  /* mode dependent information */
  union {
    unsigned int left;          /* if STORED, bytes left to copy */
    struct {
      unsigned int table;               /* table lengths (14 bits) */
      unsigned int index;               /* index into blens (or border) */
      unsigned int *blens;             /* bit lengths of codes */
      unsigned int bb;                  /* bit length tree depth */
      inflate_huft *tb;         /* bit length decoding tree */
    } trees;            /* if DTREE, decoding info for trees */
    struct {
      inflate_codes_statef 
         *codes;
    } decode;           /* if CODES, current state */
  } sub;                /* submode */
  unsigned int last;            /* true if this block is the last block */

  /* mode independent information */
  unsigned int bitk;            /* bits in bit buffer */
  unsigned long bitb;           /* bit buffer */
  inflate_huft *hufts;  /* single malloc for tree space */
  Bytef *window;        /* sliding window */
  Bytef *end;           /* one byte after sliding window */
  Bytef *read;          /* window read pointer */
  Bytef *write;         /* window write pointer */
  check_func checkfn;   /* check function */
  unsigned long check;          /* check on output */

};


/* defines for inflate input/output */
/*   update pointers and return */
#define UPDBITS {s->bitb=b;s->bitk=k;}
#define UPDIN {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;}
#define UPDOUT {s->write=q;}
#define UPDATE {UPDBITS UPDIN UPDOUT}
#define LEAVE {UPDATE return inflate_flush(s,z,r);}
/*   get bytes and bits */
#define LOADIN {p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;}
#define NEEDBYTE {if(n)r=Z_OK;else LEAVE}
#define NEXTBYTE (n--,*p++)
#define NEEDBITS(j) {while(k<(j)){NEEDBYTE;b|=((unsigned long)NEXTBYTE)<<k;k+=8;}}
#define DUMPBITS(j) {b>>=(j);k-=(j);}
/*   output bytes */
#define WAVAIL (unsigned int)(q<s->read?s->read-q-1:s->end-q)
#define LOADOUT {q=s->write;m=(unsigned int)WAVAIL;}
#define WRAP {if(q==s->end&&s->read!=s->window){q=s->window;m=(unsigned int)WAVAIL;}}
#define FLUSH {UPDOUT r=inflate_flush(s,z,r); LOADOUT}
#define NEEDOUT {if(m==0){WRAP if(m==0){FLUSH WRAP if(m==0) LEAVE}}r=Z_OK;}
#define OUTBYTE(a) {*q++=(Byte)(a);m--;}
/*   load local pointers */
#define LOAD {LOADIN LOADOUT}

/* masks for lower bits (size given to avoid silly warnings with Visual C++) */
extern unsigned int	inflate_mask[17];

/* copy as much as possible from the sliding window to the output area */
extern int	inflate_flush (inflate_blocks_statef *, z_streamp , int);

// utility functions

extern voidp	gzopen  (const char *path, const char *mode);
extern voidp	gzdopen  (int fd, const char *mode);
extern int	gzread  (voidp file, voidp buf, unsigned len);
extern int	gzwrite (voidp file, const voidp buf, unsigned len);
extern int	gzprintf (voidp file, const char *format, ...);
extern int	gzputs (voidp file, const char *s);
extern char *	gzgets (voidp file, char *buf, int len);
extern int	gzputc (voidp file, int c);
extern int	gzgetc (voidp file);
extern int	gzflush (voidp file, int flush);
extern z_off_t	gzseek (voidp file, z_off_t offset, int whence);
extern int	gzrewind (voidp file);
extern z_off_t	gztell (voidp file);
extern int	gzeof (voidp file);
extern int	gzclose (voidp file);

extern const char *  gzerror (voidp file, int *errnum);

/* checksum functions */

extern unsigned long	adler32 (unsigned long adler, const Bytef *buf, unsigned int len);
extern unsigned long	crc32   (unsigned long crc, const Bytef *buf, unsigned int len);

/* various hacks, don't look :) */

extern int	inflateInit_ (z_streamp strm, const char * version, int stream_size);
extern int	inflateInit2_ (z_streamp strm, int  windowBits, const char *version, int stream_size);

#define inflateInit(strm) \
        inflateInit_((strm),                ZLIB_VERSION, sizeof(z_stream))
#define inflateInit2(strm, windowBits) \
        inflateInit2_((strm), (windowBits), ZLIB_VERSION, sizeof(z_stream))

#endif /* _SZLIB_H */
