///////////////////////////////////////////////////////////
//
//  TxqPlotBorder.h:  A decoration of the Plot class to 
//          tick and label plot axes.
//
//  Copyright 1998 Tech-X Corporation
//
//  RCS:   $Id: TxqPlotBorder.h 908 1999-02-02 18:53:23Z kgl $
//
////////////////////////////////////////////////////////////


#ifndef TXQPLOTBORDER_H
#define TXQPLOTBORDER_H 


// Txstd includes
#include <TxqList.h>

// Qt includes
#include <qlabel.h>
#include <qlayout.h>
#include <qevent.h>

#include "TxqPlot.h"


/**
  -- 
  This class creates a Qt Plot widget used for scientific plotting
  and adds X and Y axis, labels, tick marks, and titles
 *
 *  Copyright 1998, 1999 by Tech-X Corporation
 *
 *  @author  Ryan McLean and Kelly G. Luetkemeyer
 *
 *  @version $Id: TxqPlotBorder.h 908 1999-02-02 18:53:23Z kgl $
 */

class TxqPlotBorder : public QWidget
{
  Q_OBJECT

public:

  /** The TxqPlotBorder constructor takes a QWidget w and a name
      nm and uses those to build the QWidget base class.  All other
      options must be set from member methods. */
  TxqPlotBorder(QWidget *parent=0, const char *nm=0);

  /** Operations forwarded to the plot 
      This method is forwarded onto the TxqPlot member, plot. 
       See TxqPlot for details. */
  void AppendData(double x, double y) { plot->AppendData( x, y ); }

  /** This method is forwarded onto the TxqPlot member, plot. 
       See TxqPlot for details. */
  void setAutoRange(bool x, bool y) { plot->setAutoRange( x, y ); }

  /** This method is forwarded onto the TxqPlot member, plot. 
       See TxqPlot for details. */
  void setXRange(double x, double y) { plot->setXRange( x, y ); }

  /** This method is forwarded onto the TxqPlot member, plot. 
       See TxqPlot for details. */
  void setYRange(double x, double y) { plot->setYRange( x, y ); }

  /** This method is forwarded onto the TxqPlot member, plot. 
       See TxqPlot for details. */
  void AppendPixData(double x, double y) { plot->AppendPixData( x, y ); }

  /** This method is forwarded onto the TxqPlot member, plot. 
       See TxqPlot for details. */
  void AppendPointData(double x, double y) { plot->AppendPointData( x, y ); }

  /** This method is forwarded onto the TxqPlot member, plot. 
       See TxqPlot for details. */
  void BumpColor() { plot->BumpColor(); }

  /** This method is forwarded onto the TxqPlot member, plot. 
       See TxqPlot for details. */
  void SetColor(int i) { plot->SetColor(i); }

  /** This method is forwarded onto the TxqPlot member, plot. 
       See TxqPlot for details. */
  void clearPixmap() { plot->clearPixmap(); }

  /** This method is forwarded onto the TxqPlot member, plot. 
       See TxqPlot for details. */
  void PlotPixData(XPoint *xpoints, int n) { plot->PlotPixData( xpoints, n); }

  /** This method is forwarded onto the TxqPlot member, plot. 
       See TxqPlot for details. */
  void DrawPixmap() { plot->DrawPixmap( ); }

  /** This method is forwarded onto the TxqPlot member, plot. 
       See TxqPlot for details. */
  void fillRectangle(double x1, double y1, double x2, double y2, int color) 
    { plot->fillRectangle( x1, y1, x2, y2, color); }

  /** This method is forwarded onto the TxqPlot member, plot. 
       See TxqPlot for details. */
  void drawLine(double x1, double y1, double x2, double y2, int color) 
    { plot->drawLine( x1, y1, x2, y2, color); }

  // Operations implemented by the border

  /** This sets the title of the TxqPlotBorder to t.  
      The title is displayed within the TxqPlotBorder window and above the TxqPlot. */
  void setTitle(const char *);

  /** This sets the label for the X axis to  t. */
  void setXTitle(const char *);

  /** This sets the label for the Y axis to  t. */
  void setYTitle(const char *);

  /** This method plots both Axes */
  void plotAxes( );
 
  /** The plotYlabel() will plot the Y min and max values on the
  y axis and the x tick marks. */
  void plotYlabel( );

  /** The plotXlabel() will plot the X min and max values on the
  x axis and the y tick marks */
  void plotXlabel( );

  /** Plot only the X labels and ticks, allows for auto scaling the Y-axis */
  void plotXticks( );

  /** Plot only the Y labesl and ticks, allows for auto scaling the X-axis */
  void plotYticks( );

  /** TxqPlotBorder calls getXTicks to find the positions for tick marks.
      getXTicks fills  ticks, a List of doubles with values for each position.
      The default implementation tries to place three to five tick marks at even
      values within the plot range for each axis.  All tick marks are labeled with their value.
      The user can override this virtual function for any class derived from
      TxqPlotBorder to specify tick marks. */

  virtual void getXTicks(TxqList<double> &);

  /** TxqPlotBorder calls getYTicks to find the positions for tick marks.
      getXTicks fills \a ticks, a List of doubles with values for each position.
      The default implementation tries to place three to five tick marks at even
      values within the plot range for each axis.  All tick marks are labeled with their value.
      The user can override this virtual function for any class derived from
      TxqPlotBorder to specify tick marks. */

  virtual void getYTicks(TxqList<double> &);

signals:

  /**  This signal is connected to the TxqPlot signal clicked and
       is emitted whenever there is a mouse press in the plot portion of the
       TxqPlotBorder.  See  TxqPlot  for details.  */
  void clicked( double, double );

protected:

  /* This requires that TxqPlotBorder be made with a new call, which
     is required since closing a TxqPlotBorder also deletes it. */
  virtual ~TxqPlotBorder();

  /** This method is forwarded onto the TxqPlot member, plot. 
    See TxqPlot for details. */
  double PixelToX( const QPoint & p ) { return plot->PixelToX(p); }

  /** This method is forwarded onto the TxqPlot member, plot. 
    See TxqPlot for details. */
  double PixelToY( const QPoint & p ) { return plot->PixelToY(p); }

  /** This method is forwarded onto the TxqPlot member, plot. 
    See TxqPlot for details. */
  QPoint XYtoPixel( double x, double y ) { return plot->XYtoPixel(x,y); }

  /** This method is forwarded onto the TxqPlot member, plot. 
    See TxqPlot for details. */
  short XtoPixel( double x ) { return plot->XtoPixel( x ); }

  /** This method is forwarded onto the TxqPlot member, plot. 
    See TxqPlot for details. */
  short YtoPixel( double y ) { return plot->YtoPixel( y ); }

  /** The paintEvent repaints the entire TxqPlotBorder.  Currently this
      consists of drawing a red border around the embedded plot, placing
      tick marks on the TxqPlot and then labeling the tick marks, the axes
      and titling the TxqPlotBorder. */
  void paintEvent( QPaintEvent * );
  
  /** hide the window when closing */
  void closeEvent( QCloseEvent * );

  /** the TxqPlot */
  TxqPlot *plot;

private:
  
  /** the TxqPlot */
  // TxqPlot *plot;

  /** the X axes title */
  QLabel *xtitle;

  /** the  Y axes title */
  QWidget *ytitle;

  /** the main widget title */
  QLabel *title;

  /** Place to put x and y value labels */
  QWidget *xval;  
  /** Place to put x and y value labels */
  QWidget *yval;  

  /** Used for the axes labeling */
  QRect xyrect;

  const char* ylabel;

  /** the layout */
  QBoxLayout *topLayout;

};


#endif
