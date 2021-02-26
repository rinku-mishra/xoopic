/* pbmplus.h - header file for PBM, PGM, PPM, and PNM
**
** Copyright (C) 1988, 1989, 1991 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#ifndef _PBMPLUS_H_
#define _PBMPLUS_H_

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>

#if defined(USG) || defined(SVR4)
#ifndef SYSV
#define SYSV
#endif
#endif
#if ! ( defined(BSD) || defined(SYSV) || defined(MSDOS) )
/* CONFIGURE: If your system is >= 4.2BSD, set the BSD option; if you're a
** System V site, set the SYSV option; and if you're IBM-compatible, set
** MSDOS.  If your compiler is ANSI C, you're probably better off setting
** SYSV - all it affects is string handling.
*/
#define BSD
/* #define SYSV */
/* #define MSDOS */
#endif

/* CONFIGURE: If you want to enable writing "raw" files, set this option.
** "Raw" files are smaller, and much faster to read and write, but you
** must have a filesystem that allows all 256 ASCII characters to be read
** and written.  You will no longer be able to mail P?M files without 
** using uuencode or the equivalent, or running the files through pnmnoraw.
** Note that reading "raw" files works whether writing is enabled or not.
*/
#define PBMPLUS_RAWBITS

/* CONFIGURE: PGM can store gray values as either bytes or shorts.  For most
** applications, bytes will be big enough, and the memory savings can be
** substantial.  However, if you need more than 8 bits of grayscale resolution,
** then define this symbol.
*/
/* #define PGM_BIGGRAYS */

/* CONFIGURE: Normally, PPM handles a pixel as a struct of three grays.
** If grays are stored in bytes, that's 24 bits per color pixel; if
** grays are stored as shorts, that's 48 bits per color pixel.  PPM
** can also be configured to pack the three grays into a single longword,
** 10 bits each, 30 bits per pixel.
**
** If you have configured PGM with the PGM_BIGGRAYS option, AND you don't
** need more than 10 bits for each color component, AND you care more about
** memory use than speed, then this option might be a win.  Under these
** circumstances it will make some of the programs use 1.5 times less space,
** but all of the programs will run about 1.4 times slower.
**
** If you are not using PGM_BIGGRAYS, then this option is useless -- it
** doesn't save any space, but it still slows things down.
*/
/* #define PPM_PACKCOLORS */

/* CONFIGURE: uncomment this to enable debugging checks. */
/* #define DEBUG */

#ifdef SYSV

#include <string.h>
#define index(s,c) strchr(s,c)
#define rindex(s,c) strrchr(s,c)
#define srandom(s) srand(s)
#define random rand
#define bzero(dst,len) memset(dst,0,len)
#define bcopy(src,dst,len) memcpy(dst,src,len)
#define bcmp memcmp
extern void srand();
extern int rand();

#else /*SYSV*/

#include <strings.h>
extern void srandom();
extern long random();

#endif /*SYSV*/

extern int atoi();
extern void exit();
extern long time();
extern int write();

/* CONFIGURE: On some systems, malloc.h doesn't declare these, so we have
** to do it.  On other systems, for example HP/UX, it declares them
** incompatibly.  And some systems, for example Dynix, don't have a
** malloc.h at all.  A sad situation.  If you have compilation problems
** that point here, feel free to tweak or remove these declarations.
*/
//#include <malloc/malloc.h>

/* CONFIGURE: Some systems don't have vfprintf(), which we need for the
** error-reporting routines.  If you compile and get a link error about
** this routine, uncomment the first define, which gives you a vfprintf
** that uses the theoretically non-portable but fairly common routine
** _doprnt().  If you then get a link error about _doprnt, or
** message-printing doesn't look like it's working, try the second
** define instead.
*/
/* #define NEED_VFPRINTF1 */
/* #define NEED_VFPRINTF2 */

/* End of configurable definitions. */


#undef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#undef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#undef abs
/* #define abs(a) ((a) >= 0 ? (a) : -(a)) */
#undef odd
#define odd(n) ((n) & 1)


/* Definitions to make PBMPLUS work with either ANSI C or C Classic. */

/* Initialization. */

void pm_init ( int* argcP, char* argv[] );


/* Variable-sized arrays definitions. */

char** pm_allocarray ( int cols, int rows, int size );
char* pm_allocrow ( int cols, int size );
void pm_freearray ( char** its, int rows );
void pm_freerow ( char* itrow );


/* Case-insensitive keyword matcher. */

int pm_keymatch ( char* str, char* keyword, int minchars );


/* Log base two hacks. */

int pm_maxvaltobits ( int maxval );
int pm_bitstomaxval ( int bits );


/* Error handling definitions. */

void pm_message ( char*, ... );
void pm_error ( char*, ... );			/* doesn't return */
void pm_perror ( char* reason );			/* doesn't return */
void pm_usage ( char* usage );			/* doesn't return */


/* File open/close that handles "-" as stdin and checks errors. */

FILE* pm_openr ( char* name );
FILE* pm_openw ( char* name );
void pm_close ( FILE* f );


/* Endian I/O. */

int pm_readbigshort ( FILE* in, short* sP );
int pm_writebigshort( FILE* out, short s );
int pm_readbiglong ( FILE* in, long* lP );
int pm_writebiglong ( FILE* out, long l );
int pm_readlittleshort ( FILE* in, short* sP );
int pm_writelittleshort ( FILE* out, short s );
int pm_readlittlelong ( FILE* in, long* lP );
int pm_writelittlelong ( FILE* out, long l );


#endif /*_PBMPLUS_H_*/

/************************************************************************/
/************************************************************************/

/* pbm.h - header file for libpbm portable bitmap library
*/

#ifndef _PBM_H_
#define _PBM_H_

typedef unsigned char bit;
#define PBM_WHITE 0
#define PBM_BLACK 1


/* Magic constants. */

#define PBM_MAGIC1 'P'
#define PBM_MAGIC2 '1'
#define RPBM_MAGIC2 '4'
#define PBM_FORMAT (PBM_MAGIC1 * 256 + PBM_MAGIC2)
#define RPBM_FORMAT (PBM_MAGIC1 * 256 + RPBM_MAGIC2)
#define PBM_TYPE PBM_FORMAT


/* Macro for turning a format number into a type number. */

#define PBM_FORMAT_TYPE(f) ((f) == PBM_FORMAT || (f) == RPBM_FORMAT ? PBM_TYPE : -1)


/* Declarations of routines. */

void pbm_init ( int* argcP, char* argv[] );

#define pbm_allocarray( cols, rows ) ((bit**) pm_allocarray( cols, rows, sizeof(bit) ))
#define pbm_allocrow( cols ) ((bit*) pm_allocrow( cols, sizeof(bit) ))
#define pbm_freearray( bits, rows ) pm_freearray( (char**) bits, rows )
#define pbm_freerow( bitrow ) pm_freerow( (char*) bitrow )

bit** pbm_readpbm ( FILE* file, int* colsP, int* rowsP );
void pbm_readpbminit ( FILE* file, int* colsP, int* rowsP, int* formatP );
void pbm_readpbmrow ( FILE* file, bit* bitrow, int cols, int format );

void pbm_writepbm ( FILE* file, bit** bits, int cols, int rows, int forceplain );
void pbm_writepbminit ( FILE* file, int cols, int rows, int forceplain );
void pbm_writepbmrow ( FILE* file, bit* bitrow, int cols, int forceplain );

#endif /*_PBM_H_*/

/************************************************************************/
/************************************************************************/

/* pgm.h - header file for libpgm portable graymap library
*/

#ifndef _PGM_H_
#define _PGM_H_

#ifdef PGM_BIGGRAYS
typedef unsigned short gray;
#define PGM_MAXMAXVAL 65535
#else /*PGM_BIGGRAYS*/
typedef unsigned char gray;
#define PGM_MAXMAXVAL 255
#endif /*PGM_BIGGRAYS*/


/* Magic constants. */

#define PGM_MAGIC1 'P'
#define PGM_MAGIC2 '2'
#define RPGM_MAGIC2 '5'
#define PGM_FORMAT (PGM_MAGIC1 * 256 + PGM_MAGIC2)
#define RPGM_FORMAT (PGM_MAGIC1 * 256 + RPGM_MAGIC2)
#define PGM_TYPE PGM_FORMAT


/* Macro for turning a format number into a type number. */

#define PGM_FORMAT_TYPE(f) ((f) == PGM_FORMAT || (f) == RPGM_FORMAT ? PGM_TYPE : PBM_FORMAT_TYPE(f))


/* Declarations of routines. */

void pgm_init ( int* argcP, char* argv[] );

#define pgm_allocarray( cols, rows ) ((gray**) pm_allocarray( cols, rows, sizeof(gray) ))
#define pgm_allocrow( cols ) ((gray*) pm_allocrow( cols, sizeof(gray) ))
#define pgm_freearray( grays, rows ) pm_freearray( (char**) grays, rows )
#define pgm_freerow( grayrow ) pm_freerow( (char*) grayrow )

gray** pgm_readpgm ( FILE* file, int* colsP, int* rowsP, gray* maxvalP );
void pgm_readpgminit ( FILE* file, int* colsP, int* rowsP, gray* maxvalP, int* formatP );
void pgm_readpgmrow ( FILE* file, gray* grayrow, int cols, gray maxval, int format );

void pgm_writepgm ( FILE* file, gray** grays, int cols, int rows, gray maxval, int forceplain );
void pgm_writepgminit ( FILE* file, int cols, int rows, gray maxval, int forceplain );
void pgm_writepgmrow ( FILE* file, gray* grayrow, int cols, gray maxval, int forceplain );

extern gray pgm_pbmmaxval;
/* This is the maxval used when a PGM program reads a PBM file.  Normally
** it is 1; however, for some programs, a larger value gives better results
*/

#endif /*_PGM_H_*/

/**********************************************************************/
/**********************************************************************/

/* ppm.h - header file for libppm portable pixmap library
*/

#ifndef _PPM_H_
#define _PPM_H_

typedef gray pixval;

#ifdef PPM_PACKCOLORS

#define PPM_MAXMAXVAL 1023
typedef unsigned long pixel;
#define PPM_GETR(p) (((p) & 0x3ff00000) >> 20)
#define PPM_GETG(p) (((p) & 0xffc00) >> 10)
#define PPM_GETB(p) ((p) & 0x3ff)
#define PPM_ASSIGN(p,red,grn,blu) (p) = ((pixel) (red) << 20) | ((pixel) (grn) << 10) | (pixel) (blu)
#define PPM_EQUAL(p,q) ((p) == (q))

#else /*PPM_PACKCOLORS*/

#define PPM_MAXMAXVAL PGM_MAXMAXVAL
typedef struct
    {
    pixval r, g, b;
    } pixel;
#define PPM_GETR(p) ((p).r)
#define PPM_GETG(p) ((p).g)
#define PPM_GETB(p) ((p).b)
#define PPM_ASSIGN(p,red,grn,blu) do { (p).r = (red); (p).g = (grn); (p).b = (blu); } while ( 0 )
#define PPM_EQUAL(p,q) ( (p).r == (q).r && (p).g == (q).g && (p).b == (q).b )

#endif /*PPM_PACKCOLORS*/


/* Magic constants. */

#define PPM_MAGIC1 'P'
#define PPM_MAGIC2 '3'
#define RPPM_MAGIC2 '6'
#define PPM_FORMAT (PPM_MAGIC1 * 256 + PPM_MAGIC2)
#define RPPM_FORMAT (PPM_MAGIC1 * 256 + RPPM_MAGIC2)
#define PPM_TYPE PPM_FORMAT


/* Macro for turning a format number into a type number. */

#define PPM_FORMAT_TYPE(f) ((f) == PPM_FORMAT || (f) == RPPM_FORMAT ? PPM_TYPE : PGM_FORMAT_TYPE(f))


/* Declarations of routines. */

void ppm_init ( int* argcP, char* argv[] );

#define ppm_allocarray( cols, rows ) ((pixel**) pm_allocarray( cols, rows, sizeof(pixel) ))
#define ppm_allocrow( cols ) ((pixel*) pm_allocrow( cols, sizeof(pixel) ))
#define ppm_freearray( pixels, rows ) pm_freearray( (char**) pixels, rows )
#define ppm_freerow( pixelrow ) pm_freerow( (char*) pixelrow )

pixel** ppm_readppm ( char *stream, int* colsP, int* rowsP, pixval* maxvalP );
void ppm_readppminit ( char *stream, int* colsP, int* rowsP, pixval* maxvalP, int* formatP );
void ppm_readppmrow ( char *stream, pixel* pixelrow, int cols, pixval maxval, int format );

void ppm_writeppm ( FILE* file, pixel** pixels, int cols, int rows, pixval maxval, int forceplain );
char* ppm_writeppminit ( char *stream, int cols, int rows, pixval maxval, int forceplain );
void ppm_writeppmrow ( char *stream, pixel* pixelrow, int cols, pixval maxval, int forceplain );

pixel ppm_parsecolor ( char* colorname, pixval maxval );
char* ppm_colorname ( pixel* colorP, pixval maxval, int hexok );

extern pixval ppm_pbmmaxval;
/* This is the maxval used when a PPM program reads a PBM file.  Normally
** it is 1; however, for some programs, a larger value gives better results
*/


/* Color scaling macro -- to make writing ppmtowhatever easier. */

#define PPM_DEPTH(newp,p,oldmaxval,newmaxval) \
    PPM_ASSIGN( (newp), \
	( (int) PPM_GETR(p) * (newmaxval) + (oldmaxval) / 2 ) / (oldmaxval), \
	( (int) PPM_GETG(p) * (newmaxval) + (oldmaxval) / 2 ) / (oldmaxval), \
	( (int) PPM_GETB(p) * (newmaxval) + (oldmaxval) / 2 ) / (oldmaxval) )


/* Luminance macro. */

#define PPM_LUMIN(p) ( 0.299 * PPM_GETR(p) + 0.587 * PPM_GETG(p) + 0.114 * PPM_GETB(p) )

#endif /*_PPM_H_*/

/*************************************************************************/
/*************************************************************************/

/* ppmcmap.h - header file for colormap routines in libppm
*/

/* Color histogram stuff. */

typedef struct colorhist_item* colorhist_vector;
struct colorhist_item
    {
    pixel color;
    int value;
    };

typedef struct colorhist_list_item* colorhist_list;
struct colorhist_list_item
    {
    struct colorhist_item ch;
    colorhist_list next;
    };

colorhist_vector ppm_computecolorhist ( pixel** pixels, int cols, int rows, int maxcolors, int* colorsP );
/* Returns a colorhist *colorsP long (with space allocated for maxcolors. */

void ppm_addtocolorhist ( colorhist_vector chv, int* colorsP, int maxcolors, pixel* colorP, int value, int position );

void ppm_freecolorhist ( colorhist_vector chv );


/* Color hash table stuff. */

typedef colorhist_list* colorhash_table;

colorhash_table ppm_computecolorhash ( pixel** pixels, int cols, int rows, int maxcolors, int* colorsP );

int
ppm_lookupcolor ( colorhash_table cht, pixel* colorP );

colorhist_vector ppm_colorhashtocolorhist ( colorhash_table cht, int maxcolors );
colorhash_table ppm_colorhisttocolorhash ( colorhist_vector chv, int colors );

int ppm_addtocolorhash ( colorhash_table cht, pixel* colorP, int value );
/* Returns -1 on failure. */

colorhash_table ppm_alloccolorhash ( void );

void ppm_freecolorhash ( colorhash_table cht );
