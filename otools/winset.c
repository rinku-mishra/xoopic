#ifndef NOX

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <precision.h>

#ifndef TXQ_GUI
#include <xgrafix.h>
#else
extern void XGSet2D(const char *type, const char *xlabel, const char *ylabel, const char *wlabel,
                    const char *state, int ulx, int uly, Scalar xscale,
                    Scalar yscale, int xautorescale, 
                    int yautorescale, Scalar xmin, Scalar xmax,
                    Scalar ymin, Scalar ymax);
#endif


void WinSet2D(const char *PlotType, const char *X_Label, const char *Y_Label, const char *W_Label, const char *State,
             int ulx, int uly, Scalar X_Scale, Scalar Y_Scale,
             int X_Auto_Rescale, int Y_Auto_Rescale, Scalar X_Min,
             Scalar X_Max, Scalar Y_Min, Scalar Y_Max) {
#ifdef TXQ_GUI
  XGSet2D(PlotType, X_Label, Y_Label, W_Label, State,
             ulx, uly, X_Scale, Y_Scale,
             X_Auto_Rescale, Y_Auto_Rescale, X_Min,
             X_Max, Y_Min, Y_Max);
#else
  XGSet2D(PlotType, X_Label, W_Label, State,
             ulx, uly, X_Scale, Y_Scale,
             X_Auto_Rescale, Y_Auto_Rescale, X_Min,
             X_Max, Y_Min, Y_Max);
#endif
}

#endif  // NOX

