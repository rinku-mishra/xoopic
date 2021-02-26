/*
====================================================================

DUMP.H

Defines commonly used dump functions
0.99 (KC, 11-9-97)  Replacing often used codes with funcitons


====================================================================
*/
#ifndef	__DUMP_H
#define	__DUMP_H

#include <stdio.h>
#include "ovector.h"
#include "misc.h"

#include <oops.h>
#include "xgio.h"

void XGVector2Write(Vector2& vec, FILE *DMPFile);
void XGVector3Write(Vector3& vec, FILE *DMPFile);
void XGVector2Read(Vector2& vec, FILE *DMPFile);
void XGVector3Read(Vector3& vec, FILE *DMPFile);

#endif	//	ifndef __DUMP_H


