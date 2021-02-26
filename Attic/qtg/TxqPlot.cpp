////////////////////////////////////////////////////////
//
//  TxqPlot.C:  Plot class.
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS:  $Id: TxqPlot.cpp 907 1999-02-01 19:48:13Z kgl $
//
///////////////////////////////////////////////////////


// Class definitions
#include "TxqPlot.h"

// Qt includes
#include <qpen.h>
#include <qbrush.h>
#include <qpainter.h>

/*!
  \class TxqPlot TxqPlot.h 
  \brief The TxqPlot widget is the base class for plotting.

  The TxqPlot widget implements a scientific plot.  It is capable
of plotting both pixel and line style data.

*/

/*!
  \fn  virtual void TxqPlot::AppendData(double x, double y)
  This function adds the values \a x and \a y to the set of 
points for the TxqPlot.  The function call forwards to AppendPointData.
*/


/*!
  
  The constructor for TxqPlot takes a parent widget \a w and a
name \a nm and passes them on to the base class QWidget.  
Autoranging defaults to being turned on and the number of colors 
that the pixmap can cycle through is set to six.  The background
color of the plot is also set to black.

*/
  

TxqPlot::TxqPlot(QWidget *w, const char *nm) :
  QWidget(w, nm),
  xAutoRange( TRUE ), yAutoRange( TRUE ),
  pixmap( size() ), 
  current_color( 0 ),
  ncolors( 9 )
{


/* Setup the colors according to the color map from xoopic
   found in xgrafixint.h (theColorNames)
   *theColorNames[]={  "cyan", "wheat", "green", "yellow", "orange", 
                         "pink", "lightblue", "brown", "red"};

   Use the Qt hardwired color names if available
 */ 

  colors[0] = QColor("cyan");
  colors[1] = QColor(255, 165, 79);  // tan
  colors[2] = QColor("green");
  colors[3] = QColor("yellow");
  colors[4] = QColor(255, 165, 0);   // orange
  colors[5] = QColor(255, 105, 180); // pink
  colors[6] = QColor("blue");
  colors[7] = QColor(244, 164,  96); // brown
  colors[8] = QColor("red");

// These colors come up as black with Qt for some unknown reason
  // colors[1] = QColor(245, 222, 179);   //wheat
  // colors[1] = QColor(238, 216, 174  ); //wheat1
  // colors[5] = QColor(255, 192, 203); // pink
  // colors[7] = QColor(165, 42,  42);    // brown

  setBackgroundColor( "black" );
  pixmap.fill( "black" );

}

/*!
  BumpColor() cycles to the next pen color for any drawing done to the 
pixmap.  Currently the cycle goes through six colors: blue, red, green, yellow, cyan
and magenta.  
  
*/

void TxqPlot::BumpColor()
{
  current_color++;
  current_color %= ncolors;
} 

/*!
  SetColor(int i) sets the pen color according to the input color number
  
*/

void TxqPlot::SetColor(int i)
{
  current_color = i;
  current_color %= ncolors;
} 


/*!
  A TxqPlot responds to the closeEvent by deleteing itself.  Thus
a TxqPlot must be created on the heap with a new() call.

*/

void TxqPlot::closeEvent( QCloseEvent * )
{
  // delete this;
  hide();
}

/*!
  TxqPlot reimplements mousePressEvent so that the clicked(double x, 
double y) signal is emmited with the plot coordinates of the mouseEvent()

To do something interesting, you can connect to the clicked() signal.

*/

void TxqPlot::mousePressEvent(QMouseEvent *mouseEvent)
{
  // Convert in the plot coordinates and emit the signal
  double x = PixelToX( mouseEvent->pos() );
  double y = PixelToY( mouseEvent->pos() );
  
  emit clicked( x, y );
}

/*!
  Destroys the plot.  
*/

TxqPlot::~TxqPlot()
{
  debug( "Deleted plot" );
}

/*!
  A TxqPlot responds to the resizeEvent by blanking out the pixmap

*/

void TxqPlot::resizeEvent( QResizeEvent *re) {
  pixmap.resize( re->size() );
  pixmap.fill( "black" );
}

/*!
  The paintEvent will completely redraw the plot.  First the 
pixmap is drawn and then all of the points are drawn with lines
connecting the points.  
*/

void TxqPlot::paintEvent(QPaintEvent *)
{

  QPainter p;
  QPen pen("white");

  p.begin( this );
  p.setPen( pen );


  // Draw the pixmap
  bitBlt( this, 0, 0, &pixmap );
  

  // Loop over all of the points that have been Appended to the plot
  int size = x_.Size();
  if (size > 0) {
    p.moveTo( XtoPixel( x_[0] ), YtoPixel( y_[0] ) ) ;
  }
  for (int i=1; i<size; i++) {
    p.lineTo( XtoPixel(x_[i]), YtoPixel(y_[i]) );
  }

  p.end();

}

/*!
  This function adds data to the pixmap of the plot.  Values
should be in plot coordinates.   The values are drawn both to 
the screen and the pixmap.  The new_data() signal is emmited.
*/
void TxqPlot::AppendPixData(double xval, double yval)
{
  QPainter p;
  
  // Draw point to pixmap
  p.begin( &pixmap );
  p.setPen( colors[current_color] );  // Change this so bump color will work
  p.drawPoint( XtoPixel(xval), YtoPixel(yval) );
  p.end();
  
  // Draw point to the screen
  p.begin( this );
  p.setPen( colors[current_color] );  // Change this so bump color will work
  p.drawPoint( XtoPixel(xval), YtoPixel(yval) );
  p.end();
  
  emit new_data();
}

/*!
  This function adds data to the pixmap of the plot.  Values
should be in plot coordinates.   The values are drawn to
the pixmap only.  The new_data() signal is emmited.
*/
void TxqPlot::PlotPixData(XPoint *xpoints, int n)
{
  QPainter p;
  int i;
  double xval, yval;
 

  // Draw point to pixmap

  p.begin( &pixmap );
  p.setPen( colors[current_color] );  // Change this so bump color will work
  for (i=0; i<n; i++) 
   p.drawPoint( XtoPixel(xpoints[i].x), YtoPixel(xpoints[i].y) );
  p.end();
 
}

/*!
  This function bit blt's the pixmap ont the canvas
 */

void TxqPlot::DrawPixmap()
{
  // Draw pixmap
  bitBlt( this, 0, 0, &pixmap );
 
  emit new_data();
}


/*!
  This function adds point style data to the plot.  Values \a xval and
\a yval should
be in plot coordinates.  Point style data is stored with the plot,
and is redrawn if the plot is resized.  The signal new_data() is
emitted.
*/

void TxqPlot::AppendPointData(double xval, double yval)
{

  // Add to the next element of the vector
  x_.Append( xval );
  y_.Append( yval );
  next++;

  // If autoRanging is turned on, check if mins/maxes should be adjusted
  if (xAutoRange) {
    if (xval > xmax) xmax = xval;
    if (xval < xmin) xmin = xval;
  }
  if (yAutoRange) {
    if (yval > ymax) ymax = yval;
    if (yval < ymin) ymin = yval;
  }

  // Message out the slot that we have updated.
  emit new_data();
  update();
}

/*!
  This method takes a screen pixel \a p and  returns the 
plot x coordinate for that point.
*/
double TxqPlot::PixelToX( const QPoint &p)
{
  // Calculate the x data value of the given pixel
  return xmin + p.x()*(xmax-xmin)/width();
}

/*!
  This method takes a screen pixel \a p and  returns the 
plot y coordinate for that point.
*/
double TxqPlot::PixelToY( const QPoint &p)
{
  // Calculate the y data value of the given pixel
  // recall that y reads from top to bottom
  return ymax - p.y()*(ymax-ymin)/height();
}

/*!
  This method returns the screen pixel point for the plot 
coordinates \a x and \a y
*/
QPoint TxqPlot::XYtoPixel( double x, double y )
{
  return QPoint( XtoPixel(x), YtoPixel(y) );
}

/*! 
  This method returns the x pixel on the plot for the 
plot coordinate \a xval.  Larger valued x pixels are to the right
on the screen.
*/
short TxqPlot::XtoPixel( double xval )
{
  int pix =  int( width() * (xval - xmin)/(xmax - xmin) );
  if (pix > 0x7fff) pix = 0x7fff;  // Keep large values from wrapping negative
  return pix;
}

/*! 
  This method returns the y pixel on the plot for the 
plot coordinate \a yval.  Larger valued y pixels are lower on the screen.
*/
short TxqPlot::YtoPixel( double yval )
{
  int pix =  int( height() * (ymax - yval)/(ymax - ymin) );
  if (pix > 0x7fff) pix = 0x7fff;  // Keep large values from wrapping negative
  return pix;
}

/*!
  This sets the automatic ranging flags for each of the plot axes.
When \a xAutoRangeSet or \a yAutoRangeSet are true, then the plot
will size itself so that all of the data in the given dimension will
be shown.  Axes will change as needed during the paintEvent().
*/
void TxqPlot::setAutoRange(bool xAutoRangeSet, bool yAutoRangeSet)
{
  xAutoRange = xAutoRangeSet;
  yAutoRange = yAutoRangeSet;
}


/*!
  This method explicitly sets the range covered by the X axis and
turns off auto ranging for the X axis.
*/
void TxqPlot::setXRange( double Xmin, double Xmax)
{
  xmin = Xmin;
  xmax = Xmax;
  xAutoRange = FALSE;
}

/*!
  This method explicitly sets the range covered by the Y axis and
turns off auto ranging for the Y axis.
*/
void TxqPlot::setYRange( double min, double max)
{
  ymin = min;
  ymax = max;
  yAutoRange = FALSE;
}

/*! This method clears the pixmap to black
*/
void TxqPlot::clearPixmap() 
{
  pixmap.fill( "black" );

}

/*! This method fills a Rectangle
*/
void TxqPlot::fillRectangle(double x1, double y1, double x2, double y2, int color) 
{
  QPainter p;

  QPen pen(colors[color]);
  QBrush brush(colors[color]);

  QPoint p1( XtoPixel( x1 ), YtoPixel( y1 ) );
  QPoint p2( XtoPixel( x2 ), YtoPixel( y2 ) );
  QRect  r(p1, p2);


  p.begin( &pixmap );
  p.setPen( pen );
  p.setBrush( brush );  
  p.drawRect( r );
  p.end();
  p.begin( this );
  p.setPen( pen );
  p.setBrush( brush );  
  p.drawRect( r );
  p.end();
  emit new_data();
}

/*! This method draws a line from x1, y1, to x2, y2 with int color
*/
void TxqPlot::drawLine(double x1, double y1, double x2, double y2, int color)
{
  QPainter p;
 
  QPen pen(colors[color]);
  QBrush brush(colors[color]);
 
  QPoint p1( XtoPixel( x1 ), YtoPixel( y1 ) );
  QPoint p2( XtoPixel( x2 ), YtoPixel( y2 ) );
 
  p.begin( &pixmap );
   p.setPen( pen );
   p.setBrush( brush );
   p.drawLine( p1, p2 );
  p.end();
  p.begin( this );
   p.setPen( pen );
   p.setBrush( brush );
   p.drawLine( p1, p2 );
  p.end();

  emit new_data();
}

