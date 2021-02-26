/*
====================================================================

spacetimefunc.h

	This class contains parameters for space- and time-dependent operation of
   boundaries.  It can be used in any place where space and time parameterization
	is needed.

	xtFlag meanings (should coincide with input.txt):
	0 = f(t) from envelope function
	1 = f(t) from string function
	2 = f(x) from string function
	3 = f1(x)f2(t) from string function (decoupled)
	4 = f(x,t) from string function (coupled)

1.00  (JohnV 01-08-00) Creation from timefunc.h

==================================================================== */
#ifndef SpaceTimeFunction_H
#define SpaceTimeFunction_H
#include "misc.h"
#include "math.h"
#include "eval.h"
#include "ostring.h"
extern Evaluator *adv_eval;

class SpaceTimeFunction
{
  int xtFlag; /* activates spacial/temporal dependence */
  Scalar A,C;   /*  Usually AC and then DC values respectively */
  Scalar w, phase;  /*parameters for the AC component usually */
  Scalar trise, tfall, tpulse, tdelay;  /* parameters for the envelope function */
  Scalar a0,a1; /* more parameters for the time dependent function */
  Scalar local_time; // These can now be Scalar -- RT, 2003/12/09
  Scalar local_space;
  Evaluator *evaluator;
  ostring Func;

  Scalar Envelope(Scalar t) 
    {
      if(t<tdelay)
        return a0;
      if(t<trise+tdelay) 
        return (a1-a0)*(t-tdelay)/trise + a0;
      if(t<trise+tpulse+tdelay)
        return a1;
      if(t<trise+tpulse+tfall+tdelay)
        return a1 - (a1-a0)*(t - trise - tpulse- tdelay)/tfall;
      return a0;
    };

  Scalar DerivEnvelope(Scalar t) 
    {
      if(t<tdelay)
        return 0;
      if(t<trise+tdelay) 
        return (a1-a0)/trise;
      if(t<trise+tpulse+tdelay)
        return 0.0;
      if(t<trise+tpulse+tfall+tdelay)
        return - (a1-a0)/tfall;
      return 0;
    };
	
  Scalar IntTimeValue(Scalar t)
    {
      Scalar iw;//, itrise;
      if (w)
        iw=1/(w);
      else
        iw=0;
				
      if(t<tdelay)
        return (a0+C)*t;
      if(t<trise+tdelay) 
        return a0*C + C*(a1+a0)*t*t/(2*trise) + 
          cos(w * t + phase)*(-a0*A*iw - t*A*(a1-a0)*iw/trise) + 
          sin(w * t + phase)*A*(a1-a0)*iw*iw/trise;
      if(t<trise+tpulse+tdelay)
        return a1*(C*t-A*cos(w * t + phase)*iw);
      if(t<trise+tpulse+tfall+tdelay)
        {
          Scalar a = a1+(trise+tpulse)/tfall;
          Scalar m = -(a1-a0)/tfall;
          Scalar Coeff1 = m*A*iw*iw;
          Scalar Coeff2 = a*A*iw + Coeff1;
          return a*C*t + C*m*t*t/2 + Coeff1*sin(w * t + phase) -  
            Coeff2*cos(w * t + phase); 
        }
      return a0*(C*t-A*cos(w * t + phase)*iw);
    };
	
  Scalar DerivSinusoid(Scalar t) 
    {
      return A * w*sin(w * t + phase);
    };


  Scalar Sinusoid(Scalar t) 
    {
      return C + A * sin(w * t + phase);
    };

  Scalar CoSinusoid(Scalar t) 
    {
      return C + A * cos(w * t + phase);
    };

 public:

  SpaceTimeFunction(Scalar _AA, Scalar _CC, Scalar _ff, Scalar _pp, Scalar _tr,
                    Scalar _tf, Scalar _tp, Scalar _td, Scalar _a0, Scalar _a1, 
                    ostring F, int _xtFlag=0) 
    {
      A = _AA; C = _CC; w = _ff*TWOPI; phase = _pp; trise = _tr;
      tfall = _tf; tpulse = _tp; tdelay = _td;
      a0 = _a0; a1 = _a1; xtFlag = _xtFlag;

      Func = "0";
      if (xtFlag > 0) {
        Func=F+ (ostring)'\n';
        evaluator = new Evaluator(adv_eval);
        evaluator->add_indirect_variable("t",&local_time);
        if (xtFlag > 1) evaluator->add_indirect_variable("x",&local_space);
      };
    };

  Scalar SpaceTimeValue(Scalar x, Scalar t) {
    switch(xtFlag) {
    case 0:
      return Envelope(t) * Sinusoid(t);
    case 1:
    case 2:
    case 3:
    case 4:
      {
        local_time = t;
        local_space = x;
        return evaluator->Evaluate(Func.c_str());
      }
    default:
      return 0;
    };
  };

  Scalar TimeValue(Scalar t) {
    switch(xtFlag) {
    case 0:
      return Envelope(t) * Sinusoid(t);
    case 1:
      {
        local_time = t;
        return evaluator->Evaluate(Func.c_str());
      }
    default:
      return 0;
    };
  };

  Scalar TimeValueDeriv(Scalar t) {
    switch(xtFlag) {
    case 0:
      return Sinusoid(t)*DerivEnvelope(t) + Envelope(t)*DerivSinusoid(t);
    default:
      return 0;
    };
  };
  Scalar TimeValueInt(Scalar t) {
    switch(xtFlag){
    case 0:
      return IntTimeValue(t);
    default:
      return 0;
    };
  };
  Scalar TimeValue2(Scalar t) {
    switch(xtFlag) {
    case 0:
      return Envelope(t) * CoSinusoid(t);
    default:
      return 0;
    };
  };

  Scalar TanhEnvelope(Scalar t)
    {
      if(t<tdelay)
        return 0;
      if(t<trise+tdelay)
        return 0.5*(tanh(14*(t-tdelay)/trise-6)+1);
      if(t<trise+tdelay+tpulse)
        return 1;
      if(t<trise+tdelay+tpulse+tfall)
        return 0.5*(tanh(-14*(t-tdelay-trise-tpulse)/tfall+8)
                    +1);
	return 0;/* this should never happen */
    };


  Scalar TimeValue3(Scalar t) {
      return TanhEnvelope(t);
  };

  int get_xtFlag() {return xtFlag;};
};

#endif
