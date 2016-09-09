
#ifndef __csliderwp__
#define __csliderwp__

#include "vstgui4/vstgui/lib/controls/cslider.h"

namespace VSTGUI {

/* ------------------------------------------------------
     CVerticalSliderWP, CHorizontalSliderWP
     Custom VSTGUI Objects by Will Pirkle
     Created with RackAFX(TM) Plugin Development Software
     www.willpirkle.com
 -------------------------------------------------------*/

class CVerticalSliderWP : public CVerticalSlider
{
public:
	CVerticalSliderWP (const CRect& size, CControlListener* listener, int32_t tag, int32_t iMinPos, int32_t iMaxPos, CBitmap* handle, CBitmap* background, const CPoint& offset = CPoint (0, 0), const int32_t style = kBottom);

	// overrides
	virtual void draw (CDrawContext*) VSTGUI_OVERRIDE_VMETHOD;

	void setSwitchMax(float f){m_fMaxValue = f;}
	bool isSwitchSlider(){return m_bSwitchSlider;}
	void setSwitchSlider(bool b){m_bSwitchSlider = b;}

	CLASS_METHODS(CVerticalSliderWP, CControl)

protected:
	~CVerticalSliderWP ();
	bool m_bSwitchSlider;
	float m_fMaxValue;

};


class CHorizontalSliderWP : public CHorizontalSlider
{
public:
	CHorizontalSliderWP (const CRect& size, CControlListener* listener, int32_t tag, int32_t iMinPos, int32_t iMaxPos, CBitmap* handle, CBitmap* background, const CPoint& offset = CPoint (0, 0), const int32_t style = kLeft);

	// overrides
	virtual void draw (CDrawContext*) VSTGUI_OVERRIDE_VMETHOD;

	void setSwitchMax(float f){m_fMaxValue = f;}
	void setSwitchSlider(bool b){m_bSwitchSlider = b;}
	bool isSwitchSlider(){return m_bSwitchSlider;}

	CLASS_METHODS(CHorizontalSliderWP, CControl)

protected:
	~CHorizontalSliderWP ();
	bool m_bSwitchSlider;
	float m_fMaxValue;
};

} // namespace

#endif