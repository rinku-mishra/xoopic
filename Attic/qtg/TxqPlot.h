////////////////////////////////////////////////////////
//
//  TxqPlot.h:  Plot widget class.
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: TxqPlot.h 907 1999-02-01 19:48:13Z kgl $
//
///////////////////////////////////////////////////////

#ifndef TXQPLOT_H
#define TXQPLOT_H

#include <TxqList.h>

#include <qwidget.h>
#include <qpushbt.h>
#include <qobject.h>
#include <qpixmap.h>

#include "xpoint.h"

#define COLORS 9


// This is just a helper class that can listen
// to the clicked() signal from TxqPlot

/**
  -- 
    This class is a demonstration class that can be used to connect
    the clicked signal of a TxqPlot to a slot that will print the emitted coordinates
 *
 *  Copyright 1998, 1999 by Tech-X Corporation
 *
 *  @author  Kelly G. Luetkemeyer and Ryan McLean
 *
 *  @version $Id: TxqPlot.h 907 1999-02-01 19:48:13Z kgl $
 */

class PlotListen : public QObject  
{
  Q_OBJECT
public slots:
 
  /**  The listen method prints the message 
"Heard the point ( x,  y)", where  x and  y are sent from the signal connected to this slot.
  */
  void listen(double x, double y) {
    debug ( "Heard the point ( %lg, %lg ) ", x, y );
  }
};

  

/**
  -- 
  This class creates a Qt Plot widget used for scientific plotting
 *
 *  Copyright 1998, 1999 by Tech-X Corporation
 *
 *  @author  Ryan McLean and Kelly G. Luetkemeyer
 *
 *  @version $Id: TxqPlot.h 907 1999-02-01 19:48:13Z kgl $
 */

class TxqPlot : public QWidget
{
  Q_OBJECT

public:

  /**
  The constructor for TxqPlot takes a parent widget w and a
  name  nm and passes them on to the base class QWidget.
  Autoranging defaults to being turned on and the number of colors
  that the pixmap can cycle through is set to six.  The background
  color of the plot is also set to black.
  */

  TxqPlot(QWidget *w=0, const char *nm=0);
  virtual void AppendData(double x, double y) {AppendPointData(x,y);}

  /**
  This sets the automatic ranging flags for each of the plot axes.
  When xAutoRangeSet or yAutoRangeSet are true, then the plot
  will size itself so that all of the data in the given dimension will
  be shown.  Axes will change as needed during the paintEvent().
  */

  void setAutoRange(bool xAR, bool yAR);

  /** These set the ranges and turn off autoRanging on axes for which they are set */
  void setXRange( double xmin, double xmax );

  /** These set the ranges and turn off autoRanging on axes for which they are set */
  void setYRange( double ymin, double ymax );
  
  /** Returns the value for the left edge of the plot. */
  double Xmin() { return xmin;}

  /** Returns the value for the right edge of the plot. */
  double Xmax() { return xmax;}

  /**  Returns the value for the bottom edge of the plot. */
  double Ymin() { return ymin;}

  /** Returns the value for the top edge of the plot. */
  double Ymax() { return ymax;}

  /** This method takes a screen pixel p and  returns the plot x coordinate for that point. */
  double PixelToX( const QPoint & );

  /** This method takes a screen pixel p and  returns the plot y coordinate for that point. */
  double PixelToY( const QPoint & );

  /** This method returns the screen pixel point for the plot coordinates x and  y */
  QPoint XYtoPixel( double x, double y );

  /** This method returns the x pixel on the plot for the
      plot coordinate xval.  Larger valued x pixels are to the right on the screen. */
  short XtoPixel( double x );

  /** This method returns the y pixel on the plot for the
      plot coordinate  yval.  Larger valued y pixels are lower on the screen. */
  short YtoPixel( double y );


  /** This method adds data to the pixmap of the plot.  Values should be in plot coordinates.   
      The values are drawn both to the screen and the pixmap.  The new_data() signal is emmited. */
  void AppendPixData(double x, double y);  

  /** This function adds point style data to the plot.  
      Values xval and  yval should be in plot coordinates.  
      Point style data is stored with the plot, and is redrawn if the plot is resized.  
      The signal new_data() is emitted. */
  void AppendPointData(double x, double y);  

  /** This function adds data to the pixmap of the plot.  
      Values should be in plot coordinates.   The values are drawn to the pixmap only.  
      The new_data() signal is emmited. */
  void PlotPixData(XPoint *xpoint, int n);

  /** This method bit blt's the pixmap to the screen */
  void DrawPixmap();

  /** This method fills a Rectangle */
  void fillRectangle(double x1, double y1, double x2, double y2, int color);

  /** This method draws a line */
  void drawLine(double x1, double y1, double x2, double y2, int color);

  /** Switches pen to a new color */
  void BumpColor();   

  /** Switches pen to new color based in index */
  void SetColor(int i);    

  /** clears the pixmap */
  void clearPixmap();       

signals:

  /** The clicked signal is emitted whenever the mouse is pressed in the
  plot widget.  The signal has the values xval and  yval of the  
  location of the mouse in plot coordinates.  */

  void clicked(double xval, double yval);

  /**  The signal new_data() is emitted whenever data is added to the plot,
  either via the AppendData, AppendPointData or AppendPixData method.  This
   is useful for linking updates to the plot to other events.  */

  void new_data();

protected:

  /** This requires that TxqPlot be made with a new call, which
       is required since closing a TxqPlot also deletes it. */
  virtual ~TxqPlot();

  /** The paint event */
  void paintEvent( QPaintEvent * );

  /** the Resize event */
  void resizeEvent( QResizeEvent * );
  
  /** the close event */
  void closeEvent( QCloseEvent * );


private:

  /** List data used for AppendPointData */
  TxqList< double > x_;

  /** List data used for AppendPointData */
  TxqList< double > y_;
  int next;

  /** Pixmap used for AppendPixData */
  QPixmap pixmap;

  /** xAuto range flag */
  bool xAutoRange;

  /** yAuto range flag */
  bool yAutoRange;

  /** mouse event pressed method */
  void mousePressEvent(QMouseEvent *);

  /** Ranges for the plot */
  double xmin, xmax, ymin, ymax;
  
  /** Color list */
  const int ncolors;

  /** color list */
  QColor colors[COLORS];

  /** index to current color */
  int current_color;

};



#endif
