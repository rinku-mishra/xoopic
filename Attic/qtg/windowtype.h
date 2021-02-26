////////////////////////////////////////////////////////
//
//  windowtype.h: the header to define the 
//                xgrafix window type 
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: windowtype.h 916 1999-02-09 19:22:13Z kgl $
//
///////////////////////////////////////////////////////
 
#ifndef WINDOWTYPE_H
#define WINDOWTYPE_H

#define    MAX_BUFFER_SIZE                 512


typedef struct mesh_struct {
  SCALAR           x;
  SCALAR           y;
  SCALAR           z;
}               MeshType;

typedef struct window_struct *WindowType;

struct window_struct {
  int             ulxc;
  int             ulyc;
  int             lrxc;
  int             lryc;
  int             state;
  DataType        data;
  LabelType       label;
  SCALAR          theta;
  SCALAR          phi;
  char            eps_file_name[MAX_BUFFER_SIZE];
  char            plot_title[MAX_BUFFER_SIZE];

/*
  GC              xwingc;
  Pixmap          pixmap_buffer;
  void            (*paint_function) ();
  void            (*print_function) ();
  void            (*ascii_print_function) ();
  void            (*xgrafix_print_function) ();
  Tk_Window       tkwin;
*/
  MeshType      **theMesh;
  double          c1,d1,c2,d2;
  int             mstart, mend, nstart, nend, xSize, ySize;
  StructType     *structures;
  // int            *openFlag;
  int            openFlag;
  // char            type;
  int		 type;
};

#endif
