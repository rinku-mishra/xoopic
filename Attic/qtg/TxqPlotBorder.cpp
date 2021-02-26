///////////////////////////////////////////////////////////
//
//  TxqPlotBorder.C:  A decoration of the Plot class to 
//          tick and label plot axes.
//
//  Copyright 1998 Tech-X Corporation
//
//  RCS:   $Id: TxqPlotBorder.cpp 881 1999-01-19 21:48:25Z kgl $
//
////////////////////////////////////////////////////////////



#include <math.h>

#include <qpainter.h>
#include <qstring.h>

#include "TxqPlotBorder.h"


/*!
  \class TxqPlotBorder TxqPlotBorder.h
  \brief The TxqPlotBorder is a widget that adds labels, axes, tick marks
and titles to a TxqPlot.

  TxqPlotBorder uses a decorator type Design Pattern.  Most of the 
methods for a TxqPlot are provided by the TxqPlotBorder as forwarded
calls to the TxqPlot member.

*/

/*!
  The TxqPlotBorder constructor takes a QWidget \a w and a name
\a nm and uses those to build the QWidget base class.  All other
options must be set from member methods.  
*/

TxqPlotBorder::TxqPlotBorder(QWidget *w, const char *nm) :
  QWidget( w, nm )
  
{

  // Create a layout.
  topLayout = new QVBoxLayout( this, 15 );
  CHECK_PTR( topLayout );

// Title is not available for oopic compatiability
// Title is the window caption

/*
  Create the labels, and add to the plot
  title = new QLabel( "Plot Title", this );
  CHECK_PTR( title );
  title->setAlignment( AlignCenter );
  title->setMinimumSize( title->sizeHint() );
*/

  xtitle = new QLabel( "X Axis Label", this );
  CHECK_PTR( xtitle );
  xtitle->setAlignment( AlignCenter );
  xtitle->setMinimumSize( xtitle->sizeHint() );

  xval = new QWidget( this );
  CHECK_PTR( this );
  xval->setMinimumHeight( 10 );

  yval = new QWidget( this );
  CHECK_PTR( yval );
  yval->setMinimumWidth( 60 );

// YTitle is not available for oopic compatiability
/*
  ytitle = new QWidget( this );
  ytitle->setMinimumWidth( 15 );
  CHECK_PTR( ytitle );
*/

  plot = new TxqPlot( this );
  CHECK_PTR( plot );
  connect( plot, SIGNAL( clicked(double, double) ), 
	   SIGNAL( clicked(double, double) ) );

  // Make a layout that manages the Y label and 
  // the plot together on the same row
  QBoxLayout *py = new QHBoxLayout( 10 );
  CHECK_PTR( py );
  
  

  // Add them to the layout 
  // Removed the title, and ytitle for oopic compatiblity
  // topLayout->addWidget( title );
  topLayout->addLayout( py, 10 );
    // py->addWidget( ytitle );
    py->addWidget( yval );
    py->addWidget( plot, 10 );
  topLayout->addWidget( xval );
  topLayout->addWidget( xtitle );

  topLayout->activate();

  // Make the QRect for X and Y labels
  QRect xyrect();

}

/*!
  The paintEvent repaints the entire TxqPlotBorder.  Currently this
consists of drawing a red border around the embedded plot, placing
tick marks on the TxqPlot and then labeling the tick marks, the axes
and titling the TxqPlorBorder.
   There are many possible improvements here.  Drawing directly
onto the plot is troublesome, becuase the TxqPlot updates
itself after the TxqPlotBorder when both require redraws.  Thus
most of the time the tick marks are not evident.  Also, there
should be more methods available for setting colors and sizes.

*/

// Updates the windows within
void TxqPlotBorder::paintEvent( QPaintEvent * )
{
  // plotAxes();
}

void TxqPlotBorder::plotAxes( ) {

  QPainter p;

  // Set up the pen
  QPen framePen( "red" );

  // Add a border
  p.begin( plot );
  p.setPen( framePen );
  // p.drawRect( plot->rect() );

 
  // Vectors for adding 
  TxqList<double> xticks, yticks;

  getXTicks( xticks );
  getYTicks( yticks );


  QPainter val( xval );
  val.setPen( framePen );

  // Loop over and place ticks at each X position
  int i;
  for (i = 0; i < xticks.Size(); i++) {
    double pos = xticks[i];
    
    p.moveTo( XtoPixel(pos), plot->height() );
    p.lineTo( XtoPixel(pos), plot->height()-5 ); 
    p.moveTo( XtoPixel(pos), 0 );
    p.lineTo( XtoPixel(pos), 5 );

    // label this tick mark iwth its value
    xyrect.setRect( plot->x() + XtoPixel(pos) - xval->x() - 20, 0, 
		 40, xval->height() );
    QString num;
    num.setNum( pos );

    // Make sure there is enough room to print this value
    xyrect = val.boundingRect( xyrect, AlignCenter, num.data() );
    int diff = xval->width() - ( xyrect.x() + xyrect.width() );
    if (diff < 0) {
      xyrect.moveBy( diff, 0 );
    }
    
    val.drawText( xyrect, AlignCenter, num.data() );
  }

  val.end();

  val.begin( yval );
  val.setPen( framePen );

  // Loop over and place ticks at each position on the Y axis
  for (i = 0; i < yticks.Size(); i++) {
    double pos = yticks[i];

    p.moveTo( 0, YtoPixel(pos) );
    p.lineTo( 5, YtoPixel(pos) );
    p.moveTo( plot->width(), YtoPixel(pos) );
    p.lineTo( plot->width()-5, YtoPixel(pos) );

    // label this tick mark iwth its value
    xyrect.setRect( 0, YtoPixel(pos) - 20, 
		 yval->width(), 40 );
    QString num;
    num.setNum( pos );

    // Make sure there is enough room to print this value
    xyrect = val.boundingRect( xyrect, AlignCenter, num.data() );
    int diff = yval->height() - ( xyrect.y() + xyrect.height() );
    if (diff < 0) {
      xyrect.moveBy( 0, diff );
    }
    if (xyrect.y() < 0) {
      xyrect.moveBy( 0, -xyrect.y() );
    }

    val.drawText( xyrect, AlignCenter, num.data() );
  }

  val.end();
  p.end();
  


  // Label the y axis
  /** Removed for oopic compatibility
  p.begin( ytitle );
  p.rotate( -90 );

  //  p.drawText( p.xFormDev( ytitle->rect() ), 
  //	      AlignCenter, ylabel.String() );
  p.drawText( p.xFormDev( ytitle->rect() ), 
	      AlignCenter, ylabel );

  //  QRect br = p.boundingRect( p.xFormDev( ytitle->rect() ), 
  //			     AlignCenter, ylabel.String() );
  QRect br = p.boundingRect( p.xFormDev( ytitle->rect() ), 
			     AlignCenter, ylabel );
  ytitle->setMinimumSize( p.xForm(br).size() );

  p.end();
  */
  
}

/*!
  The plotYlabel() will plot the Y min and max values on the
  y axis and the x tick marks.
  Needed since the oopic code can auto-scale Y

*/

void TxqPlotBorder::plotYlabel() {
  QPainter val( yval );
  QPen framePen( "red" );
  int ii;
  double ymin = plot->Ymin();
  double ymax = plot->Ymax();
  double pos;

  val.setPen( framePen );
 
  // Erase the previous min/max labels
  yval->erase();

  // Loop over and place ticks at min and max on the Y axis
  for (ii = 0; ii < 2; ii++ ) {
    if ( ii == 0) { 
     pos=ymin;
    }
    else {
     pos=ymax;
   }
 
    // label this tick mark iwth its value
  
    xyrect.setRect( 0, YtoPixel(pos) - 20,
                 yval->width(), 40 );
    QString num;
    num.sprintf( "%.3g", pos);
 
    // Make sure there is enough room to print this value
    xyrect = val.boundingRect( xyrect, AlignCenter, num.data() );
    int diff = yval->height() - ( xyrect.y() + xyrect.height() );
    if (diff < 0) {
      xyrect.moveBy( 0, diff );
    }
    if (xyrect.y() < 0) {
      xyrect.moveBy( 0, -xyrect.y() );
    }
 
    // val.drawText( xyrect, AlignCenter, num.data() );
    val.drawText( xyrect, AlignRight, num.data() );
  }
 
  val.end();
 }

void TxqPlotBorder::plotXticks() {
// plot x-ticks and labels the same as plotAxes

  QPainter p;
  QPen framePen( "red" );
  QPainter val( xval );
 
  // Add a border
  p.begin( plot );
  p.setPen( framePen );
  // p.drawRect( plot->rect() );

  // Vectors for adding
  TxqList<double> xticks;
 
  getXTicks( xticks );
 
  // val.begin( xval );
  val.setPen( framePen );
 
  // Loop over and place ticks at each X position
  int i;
  for (i = 0; i < xticks.Size(); i++) {
    double pos = xticks[i];
 
    p.moveTo( XtoPixel(pos), plot->height() );
    p.lineTo( XtoPixel(pos), plot->height()-5 );
    p.moveTo( XtoPixel(pos), 0 );
    p.lineTo( XtoPixel(pos), 5 );
 
    // label this tick mark iwth its value
    QRect xyrect( plot->x() + XtoPixel(pos) - xval->x() - 20, 0,
                 40, xval->height() );
    QString num;
    num.setNum( pos );
 
    // Make sure there is enough room to print this value
    xyrect = val.boundingRect( xyrect, AlignCenter, num.data() );
    int diff = xval->width() - ( xyrect.x() + xyrect.width() );
    if (diff < 0) {
      xyrect.moveBy( diff, 0 );
    }
 
    val.drawText( xyrect, AlignCenter, num.data() );
  }
 
  val.end();
  p.end();


}

/*!
  The plotXlabel() will plot the X min and max values on the
  x axis and the y tick marks.
  Needed since the oopic code can auto-scale X
 
*/
 
void TxqPlotBorder::plotXlabel() {
  QPainter val( xval );
  QPen framePen( "red" );
  int ii;
  double xmin = plot->Xmin();
  double xmax = plot->Xmax();
  double pos;
 
  val.setPen( framePen );
 
  // Erase the previous min/max labels
  xval->erase();
 
  // Loop over and place ticks at min and max on the Y axis
  for (ii = 0; ii < 2; ii++ ) {
    if ( ii == 0) {
     pos=xmin;
    }
    else {
     pos=xmax;
   }
 
    // label this tick mark iwth its value
 
    xyrect.setRect( plot->x() + XtoPixel(pos) - xval->x() - 20, 0,
                 40, xval->height() );

    QString num;
    num.sprintf( "%.3g", pos);
 
    // Make sure there is enough room to print this value
    xyrect = val.boundingRect( xyrect, AlignCenter, num.data() );
    int diff = xval->width() - ( xyrect.x() + xyrect.width() );
    if (diff < 0) {
      xyrect.moveBy(  diff , 0);
    }
 
    val.drawText( xyrect, AlignCenter, num.data() );
  }
 
  val.end();
 }

void TxqPlotBorder::plotYticks() {
 
// plot y-ticks and labels the same as plotAxes
  QPainter p;
  QPen framePen( "red" );
  QPainter val( yval );
  int ii;
 
  // Add a border
  p.begin( plot );
  p.setPen( framePen );
 
  // Vectors for adding
  TxqList<double> yticks;
 
  getYTicks( yticks );

  // val.begin( yval );
  val.setPen( framePen );
 
  // Loop over and place ticks at each position on the Y axis
  for (ii = 0; ii < yticks.Size(); ii++) {
    double pos = yticks[ii];
 
    p.moveTo( 0, YtoPixel(pos) );
    p.lineTo( 5, YtoPixel(pos) );
    p.moveTo( plot->width(), YtoPixel(pos) );
    p.lineTo( plot->width()-5, YtoPixel(pos) );
 
    // label this tick mark iwth its value
    xyrect.setRect( 0, YtoPixel(pos) - 20,
                 yval->width(), 40 );
    QString num;
    num.setNum( pos );
    // Make sure there is enough room to print this value
    xyrect = val.boundingRect( xyrect, AlignCenter, num.data() );
    int diff = yval->height() - ( xyrect.y() + xyrect.height() );
    if (diff < 0) {
      xyrect.moveBy( 0, diff );
    }
    if (xyrect.y() < 0) {
      xyrect.moveBy( 0, -xyrect.y() );
    }
 
    val.drawText( xyrect, AlignCenter, num.data() );
  }
 
  val.end();
  p.end();

  }
 


/*!
  This sets the label for the X axis to \a t.
*/

void TxqPlotBorder::setXTitle( const char *t )
{
  xtitle->setText( t );
}

/*!
  This sets the label for the Y axis to \a t.
*/
void TxqPlotBorder::setYTitle( const char *t )
{
// removed for oopic compatibility
//  ylabel = t;
}
  
/*!
  This sets the title of the TxqPlotBorder to \a t.  The title
is displayed within the TxqPlotBorder window and above the TxqPlot.
*/
void TxqPlotBorder::setTitle( const char *t )
{
// removed for oopic compatibility
//  title->setText( t );
}

/*!
  The TxqPlotBorder destructor does nothing.  The TxqPlot is 
destructed through the QWidget virtual destructor.
*/
TxqPlotBorder::~TxqPlotBorder()
{
  // The plot needs to be deleted
  debug( "TxqPlotBorder deleted" );

  // The plot will be deleted through the QWidget virtual destructor
}

/*!
  A TxqPlotBorder responds to the closeEvent by hiding itself.  
*/

void TxqPlotBorder::closeEvent( QCloseEvent * )
{
  debug( "TxqPlotBorder closed" );

  // Call the widget destructor
  // delete this;

  hide();
}

/*!  
  TxqPlotBorder calls getXTicks to find the positions for tick marks.
getXTicks fills \a ticks, a List of doubles with values for each position.
The default implementation tries to place three to five tick marks at even
values within the plot range for each axis.  All tick marks are labeled with
their value.  
  The user can override this virtual function for any class derived from
TxqPlotBorder to specify tick marks.

  Still to be added are major and minor tick marks, variable tick lengths,
and control of which ticks are labeled. 

*/



void TxqPlotBorder::getXTicks( TxqList<double> &ticks )
{
  // Go and put tick marks and values on each of the axes
  double xmin = plot->Xmin();
  double xmax = plot->Xmax();
  double diff = fabs( xmax - xmin );
  double ld = log10( diff );
  int scale =(int) ceil( ld );
  
  double normalizedDiff = pow( 10, ld - scale );  // .1 -> 1

  // From the normalized difference, 
  //determine what gap to place tick marks at
  double anti = 2.;  // Average number of tick intervals
  double ts;
  if (normalizedDiff/anti >= .05) ts = .05;
  if (normalizedDiff/anti >= .1) ts = .1;
  if (normalizedDiff/anti >= .2) ts = .2;
  if (normalizedDiff/anti >= .4) ts = .4;
  
  double gap = ts * pow( 10., scale );

  // Loop over and append ticks to the end of the List.
  for (double pos = gap*ceil(xmin/gap); pos<=xmax; pos += gap) {
    // Keep away from meaningless numbers near zero
    if ( fabs(pos) < 1e-6 * pow(10.,scale) ) pos = 0; 
    ticks.Append( pos );
  }
}

/*!  
  TxqPlotBorder calls getYTicks to find the positions for tick marks.
getXTicks fills \a ticks, a List of doubles with values for each position.
The default implementation tries to place three to five tick marks at even
values within the plot range for each axis.  All tick marks are labeled with
their value.  
  The user can override this virtual function for any class derived from
TxqPlotBorder to specify tick marks.

  Still to be added are major and minor tick marks, variable tick lengths,
and control of which ticks are labeled. 

*/

void TxqPlotBorder::getYTicks( TxqList<double> &ticks )
{
  // Go and put tick marks and values on each of the axes
  double ymin = plot->Ymin();
  double ymax = plot->Ymax();
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

  // Loop over and append ticks to the end of the List.
  // use pos<=gap*ceil(ymax/gap); to get last y tick mark
  // for (double pos = gap*ceil(ymin/gap); pos<=ymax; pos += gap) {

  for (double pos = gap*ceil(ymin/gap); pos<=gap*ceil(ymax/gap); pos += gap) {
    // Keep away from meaningless numbers near zero
    if (fabs(pos) < 1e-6 * pow(10., scale) ) pos = 0; 
    ticks.Append( pos );
  }
}
  
  
