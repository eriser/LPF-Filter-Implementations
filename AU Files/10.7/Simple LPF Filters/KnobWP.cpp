#include "KnobWP.h"
#include "vstgui4/vstgui/lib/cbitmap.h"
#include <cmath>

/* ------------------------------------------------------
     CKnobWP
     Custom VSTGUI Object by Will Pirkle
     Created with RackAFX(TM) Plugin Development Software
     www.willpirkle.com
 -------------------------------------------------------*/

namespace VSTGUI {

CKnobWP::CKnobWP (const CRect& size, CControlListener* listener, int32_t tag, int32_t subPixmaps, CCoord heightOfOneImage, CBitmap* background, const CPoint &offset, bool bSwitchKnob)
: CAnimKnob (size, listener, tag, subPixmaps, heightOfOneImage, background, offset)
, m_bSwitchKnob(bSwitchKnob)
{
	m_fMaxValue = 1.0;
}
CKnobWP::~CKnobWP(void)
{

}

void CKnobWP::draw(CDrawContext* pContext)
{
	if(getDrawBackground ())
	{
		CPoint where (0, 0);

		// --- mormalize to the switch for clicky behaviour
		if(m_bSwitchKnob)
		{
			value *= m_fMaxValue;
			value = (int)value;
			value /= m_fMaxValue;
		}

		if(value >= 0.f && heightOfOneImage > 0.)
		{
			CCoord tmp = heightOfOneImage * (getNumSubPixmaps () - 1);
			if (bInverseBitmap)
				where.v = floor ((1. - value) * tmp);
			else
				where.v = floor (value * tmp);
			where.v -= (int32_t)where.v % (int32_t)heightOfOneImage;
		}

		// --- draw it
		getDrawBackground()->draw(pContext, getViewSize(), where);

	}

	setDirty (false);
}


CMouseEventResult CKnobWP::onMouseMoved (CPoint& where, const CButtonState& buttons)
{
	CMouseEventResult res = CKnob::onMouseMoved(where, buttons);
	return res;
}

void CKnobWP::valueChanged()
{
	CControl::valueChanged();
}

}