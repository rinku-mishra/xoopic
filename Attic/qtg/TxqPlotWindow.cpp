////////////////////////////////////////////////////////
//
//  TxqPlotWindow.cpp:  Class that creates a plot window
//		 Displays title, axis, and labels
//		 Plots the OOPIC data
//               Inherits TxqPlotBorder
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: TxqPlotWindow.cpp 1017 2000-01-10 21:25:29Z kgl $
//
///////////////////////////////////////////////////////

// Includes

#include "TxqPlotWindow.h"
#include "math.h"


TxqPlotWindow::TxqPlotWindow(QWidget *q, char *Label, WindowType myWindow) : TxqPlotBorder( q ) 
{

// Set class variables

	thisLabel = Label;
	thisWindow = myWindow;

// Debug print information
#ifdef OUTPUT
  	cout << "Entered TxqPlotWindow " << endl;
  	cout << "Window:"  << thisLabel << endl;
  	cout << "ulxc "     << thisWindow->ulxc << endl;
  	cout << "ulyc "     << thisWindow->ulyc << endl;
  	cout << "xSize "    << thisWindow->xSize << endl;
  	cout << "ySize "    << thisWindow->ySize << endl;
  	cout << "openFlag " << thisWindow->openFlag << endl;
  	cout << "Y_Label "  << thisLabel << endl;
  	cout << "x_min "    << thisWindow->label->X_Min << endl;
  	cout << "x_max "    << thisWindow->label->X_Max << endl;
  	cout << "y_min "    << thisWindow->label->Y_Min << endl;
  	cout << "y_min "    << thisWindow->label->Y_Max << endl;
#endif
 
  	PlotListen l;

// Connect mouse listener to click on image
	QObject::connect( this, SIGNAL( clicked(double, double) ),
                    &l, SLOT( listen(double, double) ) );

// Set geometry, and Caption and Icon text to match xoopic
        setPalette( QPalette( Qt::lightGray ) );
	// setGeometry(650, 150, 425, 325);
	setGeometry(200, 20, 425, 325);
	setCaption(thisLabel);
	setIconText(thisLabel);
 
// Change the labels on the axes
// Currently, only X-Label is implemented with TxqPlotBorder methods
  	setTitle( thisLabel );
  	setXTitle( thisWindow->label->X_Label );
  	setYTitle( " ");
 
// Set Data range
  	setXRange(  thisWindow->label->X_Min, thisWindow->label->X_Max );
  	setYRange(  thisWindow->label->Y_Min, thisWindow->label->Y_Max );
 
// Return if not open
  	if (!thisWindow->openFlag) return;
 
  	show();



}

TxqPlotWindow::~TxqPlotWindow() 
{
	 cout << "Deleting " << thisWindow->label->Y_Label << endl;
	// debug( "Deleting TxqPlotWindow\n");
  	// delete this;
}

void TxqPlotWindow::plotPoints() 
{
        double y;
        int i,n;
        int il;
	double m, b, m1, b1;
        char buf[512];
        DataType data_Ptr, ptr;
	XPoint *X_Points;
	LabelType  label_Ptr;
        int computeX, computeY;


#ifdef OUTPUT
  	cout << "x_min   " << thisWindow->label->X_Min << endl;
  	cout << "x_max   " << thisWindow->label->X_Max << endl;
  	cout << "y_min   " << thisWindow->label->Y_Min << endl;
  	cout << "y_min   " << thisWindow->label->Y_Max << endl;
#endif

	RescaleTwoDWindow(thisWindow);
        label_Ptr = thisWindow->label;

// set data min/max on graph
// Set Data range
	setXRange(  thisWindow->label->X_Min, thisWindow->label->X_Max );
	setYRange(  thisWindow->label->Y_Min, thisWindow->label->Y_Max );

/**
        computeX = 0;
        computeY = 0;
        if ( (thisWindow->label->X_Min == 0.0 ) && (thisWindow->label->X_Max == 0.0) ) computeX = 1;
        if ( (thisWindow->label->Y_Min == 0.0 ) && (thisWindow->label->Y_Max == 0.0) ) computeY = 1;
*/
/**
  This is needed since some species propagate 0 values
 */
        if ( (thisWindow->label->X_Min == 0.0 ) && (thisWindow->label->X_Max == 0.0) ) 
	  setXRange(  -1.0, 1.0);
        if ( (thisWindow->label->Y_Min == 0.0 ) && (thisWindow->label->Y_Max == 0.0) ) 
	  setYRange(  -1.0, 1.0);


	data_Ptr = thisWindow->data;
          clearPixmap();
        for (ptr = data_Ptr; ptr != NULL; ptr = ptr->next) {
	  n = *(ptr->npoints);
          int color = ptr->color;
          SetColor(color);
#ifdef OUTPUT
          cout << endl << "Line color " << color << endl;
	  cout << "number of data points " << n << endl;
#endif
          if (!(X_Points = (XPoint *) malloc(n * sizeof(XPoint)))) putchar(7);

          if (Is_X_Log(thisWindow))
          for (i = 0; i < n; i++) {cout << "log " << endl; X_Points[i].x = 0; }
/*
           X_Points[i].x = m1 * log10(max(label_Ptr->X_Scale * CurveX(ptr,i),
                                       DBL_MIN)) + b1;
*/
          else
           for (i = 0; i < n; i++)
              X_Points[i].x =  CurveX(ptr,i); 
              // X_Points[i].x = m1 * CurveX(ptr,i) + b1;
 
          if (Is_Y_Log(thisWindow))
            for (i = 0; i < n; i++) {cout << "log " << endl; X_Points[i].y = 0; }
/**
            for (i = 0; i < n; i++)
              X_Points[i].y = m * log10(max(label_Ptr->Y_Scale * CurveY(ptr,i),
                                      DBL_MIN)) + b;
*/
          else
           for (i = 0; i < n; i++)
            X_Points[i].y =  CurveY(ptr,i); 
            // X_Points[i].y = m * CurveY(ptr,i) + b;
 
          // clearPixmap();

/**
          if (computeX) {
           for (i = 0; i < n; i++) {
            if (X_Points[i].x < thisWindow->label->X_Min ) thisWindow->label->X_Min = X_Points[i].x;
	    if (X_Points[i].x > thisWindow->label->X_Max ) thisWindow->label->X_Max = X_Points[i].x;
           }
	   setXRange(  thisWindow->label->X_Min, thisWindow->label->X_Max );
          }
          if (computeY) {
           for (i = 0; i < n; i++) {
            if (X_Points[i].y < thisWindow->label->Y_Min ) thisWindow->label->Y_Min = X_Points[i].y;
	    if (X_Points[i].y > thisWindow->label->Y_Max ) thisWindow->label->Y_Max = X_Points[i].y;
           }
	   setYRange(  thisWindow->label->Y_Min, thisWindow->label->Y_Max );
          }
*/

	  plotStructure();
          PlotPixData( X_Points, n);
          if ( label_Ptr->Y_Auto_Rescale  && label_Ptr->X_Auto_Rescale ) {
	    plotYlabel();
	    plotXlabel();
          }
          else if ( label_Ptr->Y_Auto_Rescale ) {
	    plotYlabel();
            plotXticks();
          }
	  else if ( label_Ptr->X_Auto_Rescale ) {
	    plotXlabel();
            plotYticks();
          }
          else {
            plotAxes( );
          }

/**
    if (Is_Scatter(ptr))
      XDrawPoints(theDisplay, thisWindow->pixmap_buffer, thisWindow->xwingc, X_Points, n, CoordModeOrigin);
    else
      XDrawLines(theDisplay, thisWindow->pixmap_buffer, thisWindow->xwingc, X_Points, n, CoordModeOrigin);
*/
       
          free(X_Points);
	 }
     DrawPixmap();
}

void TxqPlotWindow::plotStructure() 
{
	
  StructType     *structures;
  int n;
  XPoint         *StructPoints;


  /**********************************************/
  /* Plotting the structure(s) into the window. */
 
#ifdef OUTPUT
  cout << "Entered plotStructure" << endl;
#endif
//   thisWindow = theWindowArray[windowNumber];
  structures = thisWindow->structures;
  while(structures) {
    n = structures->numberPoints;

#ifdef OUTPUT
    cout << "Number of structure points " << n <<endl;
#endif
 
    if (!(StructPoints = (XPoint *) malloc(n * sizeof(XPoint))))
      putchar(7);
 
    if (Is_X_Log(thisWindow)) { cout << "Log in structures " << endl;
/**
      for (i = 0; i < n; i++)
        StructPoints[i].x = m3 * log10(max(structures->x[i], DBL_MIN)) + b1;
*/
    } else {
      for (int i = 0; i < n; i++)
        StructPoints[i].x =  structures->x[i] ;
        // StructPoints[i].x = m3 * structures->x[i] + b1;
    }
 
    if (Is_Y_Log(thisWindow)) { cout << "Log in structures " << endl;
/**
      for (i = 0; i < n; i++)
        StructPoints[i].y = m2 * log10(max(structures->y[i], DBL_MIN)) + b;
*/
    } else {
      for (int i = 0; i < n; i++)
        StructPoints[i].y =  structures->y[i] ;
        // StructPoints[i].y = m2 * structures->y[i] + b;
    }
 
/*
    if (structures->fillFlag == FILLED) {
      XSetForeground(theDisplay, thisWindow->xwingc,
                     Color[structures->fillColor]);
      XFillPolygon(theDisplay, thisWindow->pixmap_buffer, thisWindow->xwingc,
                   StructPoints, n, Complex, CoordModeOrigin);
    }
*/
 
    double x1, y1, x2, y2;

    if (n==2) {
     x1 = StructPoints[0].x;
     y1 = StructPoints[0].y;
     x2 = StructPoints[1].x;
     y2 = StructPoints[1].y;
     int color = structures->lineColor;
     // cout << "Drawing line from " << x1 << " " << y1 <<" " << x2 <<" " << y2 << endl;
     // cout << "Color is " << color << endl;
     drawLine(x1,y1,x2,y2,color);
    }

     
/* this is a kludge ... need to learn more about structure */
    if (n==5) {
     int color = structures->fillColor;
     // cout << "Structure color " << color << endl;
     x1 = StructPoints[0].x;
     y1 = StructPoints[0].y;
     x2 = StructPoints[0].x;
     y2 = StructPoints[0].y;

#ifdef OUTPUT
     cout << "Structure x,y" << endl;
#endif
     for (int i = 0; i < n; i++) {
#ifdef OUTPUT
    	cout << StructPoints[i].x << " " << StructPoints[i].y << endl;
#endif
	// AppendPointData(StructPoints[i].x, StructPoints[i].y);
        if (StructPoints[i].x < x1 ) x1 = StructPoints[i].x;
        if (StructPoints[i].x > x2 ) x2 = StructPoints[i].x;
        if (StructPoints[i].y > y1 ) y1 = StructPoints[i].y;
        if (StructPoints[i].y < y2 ) y2 = StructPoints[i].y;
     }
     fillRectangle(x1,y1,x2,y2,color);
    }

/*
    XSetForeground(theDisplay, thisWindow->xwingc,
                   Color[structures->lineColor]);
    XDrawLines(theDisplay, thisWindow->pixmap_buffer, thisWindow->xwingc,
               StructPoints, n, CoordModeOrigin);
*/
 
    free(StructPoints);
    structures = structures->next;
  }

}
/*!
  A TxqPlotWindow responds to the closeEvent by hiding itself.
*/
 
void TxqPlotWindow::closeEvent( QCloseEvent * )
{
  cout << "Closing " << thisWindow->label->Y_Label << endl;
  thisWindow->openFlag = 0;
  hide();
}

void TxqPlotWindow::getYTicks( TxqList<double> &ticks )
{
   double ymin = plot->Ymin();
   double ymax = plot->Ymax();
   double pos;
  double diff = fabs( ymax - ymin );
  double ld = log10( diff );
  int scale =(int) ceil( ld );
 
  double normalizedDiff = pow( 10., ld - scale );  // .1 -> 1
 
  // From the normalized difference,
  //determine what gap to place tick marks at
  double anti = 2.;  // Average number of tick intervals
  double ts;
  if (normalizedDiff/anti >= .05) ts = .05;
  if (normalizedDiff/anti >= .1) ts = .1;
  if (normalizedDiff/anti >= .2) ts = .2;
  if (normalizedDiff/anti >= .4) ts = .4;
 
  double gap = ts * pow( 10., scale );
  // double gap = 1.0 * pow( 10., scale );


   double pos1 =gap*ceil(ymin/gap);
   double pos2 =gap*ceil(ymax/gap);
   pos=(((pos2-pos1)/2.0));

   ticks.Append(pos1);
   ticks.Append(pos);
   ticks.Append(pos2);

}
