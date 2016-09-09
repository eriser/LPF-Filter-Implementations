#pragma once
#include "vstgui4/vstgui/lib/controls/cbuttons.h"

namespace VSTGUI {

enum {mouseUpAndDown, mouseUp, mouseDown};

class CKickButtonWP : public CKickButton
{
public:
	CKickButtonWP(const CRect& size, CControlListener* listener, int32_t tag, CBitmap* background, const CPoint& offset = CPoint (0, 0));
	virtual CMouseEventResult onMouseDown (CPoint& where, const CButtonState& buttons) VSTGUI_OVERRIDE_VMETHOD;
	virtual CMouseEventResult onMouseUp (CPoint& where, const CButtonState& buttons) VSTGUI_OVERRIDE_VMETHOD;

	void setMouseMode(unsigned int uMode){uMouseBehavior = uMode;}

private:
	float   fEntryState;
	unsigned int uMouseBehavior;
};

}
