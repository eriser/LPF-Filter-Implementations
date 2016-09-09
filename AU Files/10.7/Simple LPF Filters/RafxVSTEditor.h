
#ifndef __CRafxVSTEditor__
#define __CRafxVSTEditor__

#include "WPXMLParser.h"
#include "plugin.h"
#include "synthfunctions.h"
#include "VuMeterWP.h"
#include "XYPadWP.h"
#include "rafx2vstguimessages.h"
#include "vstgui4/vstgui/vstgui.h"
#include "RAFXSwitchView.h"

#include <sstream>
#import <CoreAudio/CoreAudio.h>
#import <AudioUnit/AudioUnit.h>
#import <AudioToolbox/AudioToolbox.h>

using namespace std;
using namespace pugi;

namespace VSTGUI {

const UINT ASSIGNBUTTON_1 = 32768;
const UINT ASSIGNBUTTON_2 = 32769;
const UINT ASSIGNBUTTON_3 = 32770;
const UINT ALPHA_WHEEL    = 32771;
const UINT LCD_KNOB		  = 32772;
const UINT JOYSTICK_X	  = 32773;
const UINT JOYSTICK_Y	  = 32774;
const UINT JOYSTICK 	  = 32775;
const UINT TRACKPAD 	  = 32776;
const UINT LCD_TITLE 	  = 32777;
const UINT LCD_COUNT 	  = 32778;
const UINT TAB_CTRL_0 	  = 65536;
const UINT TAB_CTRL_1	  = 65537;
const UINT TAB_CTRL_2	  = 65538;
const UINT TAB_CTRL_3     = 65539;
const UINT TAB_CTRL_4     = 65540;
const UINT TAB_CTRL_5     = 65541;
const UINT TAB_CTRL_6     = 65542;
const UINT TAB_CTRL_7     = 65543;
const UINT TAB_CTRL_8     = 65544;
const UINT TAB_CTRL_9     = 65545;
const UINT TAB_CTRL_10    = 65546;
const UINT TAB_CTRL_11    = 65547;
const UINT TAB_CTRL_12    = 65548;
const UINT TAB_CTRL_13    = 65549;
const UINT TAB_CTRL_14    = 65550;
const UINT TAB_CTRL_15    = 65551;
const UINT TAB_CTRL_16    = 65552;
const UINT TAB_CTRL_17    = 65553;
const UINT TAB_CTRL_18    = 65554;
const UINT TAB_CTRL_19    = 65555;
const UINT TAB_CTRL_20    = 65556;
const UINT TAB_CTRL_21    = 65557;
const UINT TAB_CTRL_22    = 65558;
const UINT TAB_CTRL_23    = 65559;
const UINT TAB_CTRL_24    = 65560;
const UINT TAB_CTRL_25    = 65561;
const UINT TAB_CTRL_26    = 65562;
const UINT TAB_CTRL_27    = 65563;
const UINT TAB_CTRL_28    = 65564;
const UINT TAB_CTRL_29    = 65565;
const UINT TAB_CTRL_30    = 65566;
const UINT TAB_CTRL_31    = 65567;

// --- custom RAFX Attributes
/*const UINT RAFX_XML_NODE = 32768;
const UINT RAFX_TEMPLATE_TYPE = 32769;
const UINT RAFX_CONTROL_TYPE = 32770; */

// --- NODE
const UINT RAFX_XML_NODE = 'RXML';

// --- m_uPlugInEx[]
const UINT RAFX_VERSION = 0;
const UINT VSTGUI_VERSION = 1;
const UINT KNOB_MODE = 2;

typedef struct
{
    void* pWindow;
    float width;
    float height;
    AudioUnit au;
} VIEW_STRUCT;

// --- RAFX v6.6
const unsigned int kHostChoice = 3;

/* ------------------------------------------------------
 KeyboardHooker
 Custom VSTGUI Object by Will Pirkle
 Created with RackAFX(TM) Plugin Development Software
 www.willpirkle.com
 -------------------------------------------------------*/
class KeyboardHooker : public IKeyboardHook
{
    //< should return 1 if no further key down processing should apply, otherwise -1
    virtual int32_t onKeyDown (const VstKeyCode& code, CFrame* frame)
    {
        VstKeyCode key(code);

        // --- do keydown processing based on code.character
        //     return 1 if handled, -1 if not handled
        return -1;
    }

    //< should return 1 if no further key up processing should apply, otherwise -1
    virtual int32_t onKeyUp (const VstKeyCode& code, CFrame* frame)
    {
        VstKeyCode key(code);

        // --- do keyup processing based on code.character
        //     return 1 if handled, -1 if not handled
        return -1;
    }
};


/* ------------------------------------------------------
     CRafxVSTEditor
     Custom VSTGUI Object by Will Pirkle
     Created with RackAFX(TM) Plugin Development Software
     www.willpirkle.com
 -------------------------------------------------------*/

class CRafxVSTEditor : public CControlListener, public VSTGUI::VSTGUIEditorInterface, public VSTGUI::CBaseObject
{
public:
	CRafxVSTEditor();
    ~CRafxVSTEditor();
	CWPXMLParser m_XMLParser;
    void* m_pRafxFrame;
  	KeyboardHooker m_KeyHooker;
    CVSTGUITimer* timer;
   	CMessageResult notify(CBaseObject* sender, const char* message);

    // --- AU Only
    void getSize(float& width, float& height);
    bool m_bInitialized;
    float getCookedValue(int nIndex, float fNormalizedValue);

    // --- End AU Only

	bool bitmapExistsInResources(const char* bitmapname);

    // --- open/close
	bool open(void* pHwnd, CPlugIn* pPlugIn, char* pXMLFile, AudioUnit inAU);
    void close();
	bool bClosing;
	virtual void idle();

	// --- lin/circ
	UINT m_uKnobAction;
	virtual int32_t getKnobMode() const;

	// --- from CControlListener
	void valueChanged(CControl* pControl);
   	virtual int32_t controlModifierClicked (VSTGUI::CControl* pControl, VSTGUI::CButtonState button);// { return 0; }	///< return 1 if you want the control to not handle it, otherwise 0
	virtual void controlBeginEdit (VSTGUI::CControl* pControl);// {}
	virtual void controlEndEdit (VSTGUI::CControl* pControl);// {}
	virtual void controlTagWillChange (VSTGUI::CControl* pControl);// {}
	virtual void controlTagDidChange (VSTGUI::CControl* pControl);// {}
#if DEBUG
	virtual char controlModifierClicked (VSTGUI::CControl* pControl, long button);// { return 0; }
#endif


	// -- WP RAFX
	void initControls(bool bSetListener = true);
	CBitmap* loadBitmap(const CResourceDescription& desc);
	CNinePartTiledBitmap* loadTiledBitmap(const CResourceDescription& desc, CCoord left, CCoord top, CCoord right, CCoord bottom);

	bool createSubView(CViewContainer* pParentView, pugi::xml_node viewNode, bool bFrameSubView); // framesubview = not part of anothe vc part of frame vc
	bool deleteSubView(CViewContainer* pParentView, pugi::xml_node viewNode, pugi::xml_node parentNode); // framesubview = not part of anothe vc part of frame vc

	bool m_bOKtoStore;

	CTextLabel* createTextLabel(pugi::xml_node node, bool bAddingNew = true, bool bStandAlone = false);
	void parseTextLabel(CTextLabel* pLabel, pugi::xml_node node);
	CTextEdit* createTextEdit(pugi::xml_node node, bool bAddingNew = true, bool bStandAlone = false);
	CFontDesc* createFontDesc(pugi::xml_node node);
	CFontDesc* createFontDescFromFontNode(pugi::xml_node node);
	CAnimKnob* createAnimKnob(pugi::xml_node node, CBitmap* bitmap, bool bAddingNew = true, bool bStandAlone = false);
	CSlider* createSlider(pugi::xml_node node, CBitmap* handleBitmap, CBitmap* grooveBitmap, bool bAddingNew = true, bool bStandAlone = false);
	CVerticalSwitch* createVerticalSwitch(pugi::xml_node node, CBitmap* backgroundBitmap, bool bAddingNew = true, bool bStandAlone = false);
    CHorizontalSwitch* createHorizontalSwitch(pugi::xml_node node, CBitmap* backgroundBitmap, bool bAddingNew = true);
    COnOffButton* createOnOffButton(pugi::xml_node node, CBitmap* bitmap, bool bAddingNew = true, bool bStandAlone = false);
	CVuMeter* createMeter(pugi::xml_node node, CBitmap* onBitmap, CBitmap* offBitmap, bool bAddingNew = true, bool bStandAlone = false);
	CKickButton* createKickButton(pugi::xml_node node, CBitmap* backgroundBitmap, bool bAddingNew = true, bool bStandAlone = false);
	COptionMenu* createOptionMenu(pugi::xml_node node, CBitmap* backgroundBitmap, bool bAddingNew = true, bool bStandAlone = false);
	CXYPad* createXYPad(pugi::xml_node node, CBitmap* backgroundBitmap, bool bAddingNew = true, bool bStandAlone = false);
	CViewContainer* createViewContainer(pugi::xml_node node, CBitmap* backgroundBitmap, bool bAddingNew = true, bool bStandAlone = false);
	CView* createView(pugi::xml_node node, CBitmap* backgroundBitmap, bool bAddingNew = true, bool bStandAlone = false);
	CViewContainer* createUISwitchView(pugi::xml_node node, CBitmap* backgroundBitmap, bool bAddingNew = true, bool bStandAlone = false);
    CViewContainer* createUISwitchViewSubView(CViewContainer* pParentView, pugi::xml_node viewNode);

	CTextLabel* m_pLCDControlNameLabel;
	CTextLabel* m_pLCDControlIndexCountLabel;

	void setPlugIn(CPlugIn* pPlugIn){m_pPlugIn = pPlugIn;}
	void setControlMap(UINT* pControlMap){m_pControlMap = pControlMap;}
	void setLCDControlMap(UINT* pLCDControlMap){m_pLCDControlMap = pLCDControlMap;}
	void setLCDControlCount(int nCount){m_nLCDControlCount = nCount;}

	// --------------------------------------------------------------------------------------------------
	const char* getBitmapName(int nIndex);
	inline bool isBitmapTiled(const char* bitmapName, int& left, int& top, int& right, int& bottom)
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;

		if(m_XMLParser.isBitmapTiled(bitmapName))
		{
			const char_t* offsets = m_XMLParser.getBitmapAttribute(bitmapName, "nineparttiled-offsets");
			CCoord Dleft, Dtop, Dright, Dbottom = 0;
			getTiledOffsets(offsets, Dleft, Dtop, Dright, Dbottom);
			left = (int)Dleft;
			right = (int)Dright;
			top = (int)Dtop;
			bottom = (int)Dbottom;
			return true;
		}
		return false;
	}
	const char* getCurrentBackBitmapName();
	const char* getCurrentBackColorName();
	const char* getRAFXBitmapType(const char* bitmapName);
	bool setRAFXBitmapType(const char* bitmapName, const char* type);

    bool getBitmapsWithRAFXType(std::vector<std::string>* pStringList, const char* type, bool bAppend, bool bIncludeNoTypes);
	bool getColors(std::vector<std::string>* pNameList, std::vector<std::string>* pRGBAList);
	bool getFonts(std::vector<std::string>* pFontNameList);
	bool getTags(std::vector<std::string>* pControlTagList, UINT uControlType, bool bClearList = true);
	bool getJoystickTags(std::vector<std::string>* pControlTagList);

	// for radio buttons...
	const char* getEnumString(const char* sTag);
    inline bool setCurrentBackBitmap(const char* bitmap, CFrame* pFrame = NULL)
	{
		if(pFrame == NULL)
			pFrame = frame;

		if(strcmp(bitmap, "none") == 0)
		{
			pFrame->setBackground(NULL);
			pFrame->invalid();
			pFrame->idle();

			int32_t outSize;
			pugi::xml_node node;
			if(pFrame->getAttribute(RAFX_XML_NODE, sizeof(pugi::xml_node*), &node, outSize))
				m_XMLParser.setAttribute(node, "bitmap", "");

			return true;
		}

		CNinePartTiledBitmap* backgroundTiledBMP;
		CBitmap* backgroundBMP;

		// --- lookup the bitmap from the file
		if(m_XMLParser.hasBitmap(bitmap))
		{
			// --- get the bitmap file name (png)
			const char_t* bitmapFilename = m_XMLParser.getBitmapAttribute(bitmap, "path");

			if(m_XMLParser.isBitmapTiled(bitmap))
			{
				const char_t* offsets = m_XMLParser.getBitmapAttribute(bitmap, "nineparttiled-offsets");
				CCoord left, top, right, bottom = 0;
				getTiledOffsets(offsets, left, top, right, bottom);

				backgroundTiledBMP = loadTiledBitmap(bitmapFilename, left, top, right, bottom);
				pFrame->setBackground(backgroundTiledBMP);
				if(backgroundTiledBMP)
					backgroundTiledBMP->forget();
			}
			else
			{
				backgroundBMP = loadBitmap(bitmapFilename);
				pFrame->setBackground(backgroundBMP);
				if(backgroundBMP)
					backgroundBMP->forget();
			}

			int32_t outSize;
			pugi::xml_node node;
			if(pFrame->getAttribute(RAFX_XML_NODE, sizeof(pugi::xml_node*), &node, outSize))
				m_XMLParser.setAttribute(node, "bitmap", bitmap);

			return true;
		}

		return false;
	}

	inline bool setCurrentBackColor(const char* color, COLORREF cr, CFrame* pFrame = NULL)
	{
		if(pFrame == NULL)
			pFrame = frame;

		string str(color);

		CColor backCColor;
		CColor builtInColor;

		// --- add if not existing
		checkAddCColor(color, cr);

		if(isBuiltInColor(str, builtInColor))
			backCColor = builtInColor;
		else if(!m_XMLParser.hasColor(color))
			return false;
		else
			backCColor = getCColor(color);

		// --- set on frame
        //	CColor backCColor = getCColor(color);
		pFrame->setBackgroundColor(backCColor);

		// --- save
		int32_t outSize;
		pugi::xml_node node;
		if(pFrame->getAttribute(RAFX_XML_NODE, sizeof(pugi::xml_node*), &node, outSize))
			m_XMLParser.setAttribute(node, "background-color", color);

		return true;
	}

	// --- helpers for the massive VC get/set shit
	inline void getStringAttribute(pugi::xml_node node, const char* attributeName, std::string* destination)
	{
		const char* att =  m_XMLParser.getAttribute(node, attributeName);
		if(att)
		{
			string s(att);
			*destination = s;
		}
	}
	inline void setStringAttribute(pugi::xml_node node, const char* attributeName, std::string value)
	{
		m_XMLParser.setAttribute(node, attributeName, value.c_str());
	}
	inline void setAttribute(pugi::xml_node node, const char* attributeName, const char* attributeValue)
	{
		m_XMLParser.setAttribute(node, attributeName, attributeValue);
	}
	inline void getIntAttribute(pugi::xml_node node, const char* attributeName, int* destination)
	{
		const char* att =  m_XMLParser.getAttribute(node, attributeName);
		if(att)
		{
			string s(att);
			*destination = atoi(s.c_str());
		}
	}
	inline void setIntAttribute(pugi::xml_node node, const char* attributeName, int value)
	{
		char* text =  new char[33];
		itoa(value, text, 10);
		const char* cctext(text);
		m_XMLParser.setAttribute(node, attributeName, cctext);
		delete []  text;
	}
	inline void getBoolAttribute(pugi::xml_node node, const char* attributeName, bool* destination, bool bReverseLogic = false)
	{
		const char* att =  m_XMLParser.getAttribute(node, attributeName);
		if(att)
		{
			if(bReverseLogic)
			{
				*destination = true;
				if(strcmp(att, "true") == 0)
					*destination = false;
			}
			else
			{
				*destination = false;
				if(strcmp(att, "true") == 0)
					*destination = true;
			}
		}
	}
	inline void setBoolAttribute(pugi::xml_node node, const char* attributeName, bool value)
	{
		const char* pAtt = value ? "true" : "false";
		m_XMLParser.setAttribute(node, attributeName, pAtt);
	}

	// --- FONT
	inline CFontDesc* createFontDesc(const char* font, int nWeight,
									 bool bBold = false, bool bItalic = false,
									 bool bStrike = false, bool bUnder = false)
	{
		const char_t* fontname = m_XMLParser.getFontAttribute(font, "font-name");
		string str(fontname);
		CFontDesc* fontDesc = NULL;
		if(strlen(fontname) <= 0)
		{
			const CFontRef builtInFont = getBuiltInFont(font);
			if(builtInFont)
			{
				fontDesc = builtInFont;
			}
		}
		else
			fontDesc = new CFontDesc(fontname);

		//
		const char_t* fontSize = intToString(nWeight);
		if(strlen(fontSize) > 0)
		{
			string ccoord(fontSize);
			const CCoord fontsize = ::atof(ccoord.c_str());
			fontDesc->setSize(fontsize);
		}

		// --- bold
		if(bBold)
			fontDesc->setStyle(kBoldFace);

		// --- ital
		if(bItalic)
			fontDesc->setStyle(fontDesc->getStyle() | kItalicFace);

		if(bStrike)
			fontDesc->setStyle(fontDesc->getStyle() | kStrikethroughFace);

		if(bUnder)
			fontDesc->setStyle(fontDesc->getStyle() | kUnderlineFace);

		return fontDesc;
	}

	// --- true if not VC
	inline bool isNotVC(string sClass)
	{
		if(sClass != "CViewContainer" &&
           sClass != "CSplitView" &&
           sClass != "CLayeredViewContainer" &&
           sClass != "CRowColumnView" &&
           sClass != "CRowColumnView" &&
           sClass != "CScrollView" &&
           sClass != "CShadowViewContainer" &&
           sClass != "UIViewSwitchContainer")
			return true;

		return false;
	}

	// --- currently only supporting this, may add more later
	/*inline bool isSupportedVC(string sClass)
	{
		if(sClass == "CViewContainer")
			return true;

		return false;
	}*/

	inline const CRect getRectFromNode(pugi::xml_node viewNode)
	{
		// --- make rect
		const char_t* origin = m_XMLParser.getAttribute(viewNode, "origin");
		string sOrigin = string(origin);

		int x, y = 0;
		if(!getXYFromString(sOrigin, x, y))
		{
			const CRect rect(0,0,0,0);
			return rect;
		}

		const char_t* size = m_XMLParser.getAttribute(viewNode, "size");
		string sSize = string(size);

		int w, h = 0;
		if(!getXYFromString(sSize, w, h))
		{
			const CRect rect(0,0,0,0);
			return rect;
		}
		const CRect rect(x, y, x+w, y+h);
		return rect;
	}
	inline bool getXYFromString(string str, int& x, int& y)
	{
		if(str.length() <= 0)
			return false;

		int nComma = str.find_first_of(',');
		if(nComma <= 0)
			return false;

		string sX = str.substr(0, nComma);
		string sY = str.substr(nComma+1);

		x = atoi(sX.c_str());
		y = atoi(sY.c_str());
		return true;
	}
	inline string getStringFromCPoint(CPoint point)
	{
		string sX;
		string sY;
		string str;

		ostringstream strs;
		strs << point.x;
		sX.assign(strs.str());

		ostringstream strs2;
		strs2 << point.y;
		sY.assign(strs2.str());

		str.assign(sX);
		str.append(", ");
		str.append(sY);
		return str;
	}
	inline const CPoint getCPointFromString(string offset)
	{
		int x, y = 0;
		getXYFromString(offset, x, y);
		return CPoint(x,y);
	}
	inline bool getTiledOffsets(string str, CCoord& left, CCoord& top, CCoord& right, CCoord& bottom)
	{
		left = 0; top = 0; right = 0; bottom = 0;
		if(str.length() <= 0)
			return false;

		int nComma = str.find_first_of(',');
		if(nComma <= 0)
			return false;

		string sLeft = str.substr(0, nComma);
		string sRem1 = str.substr(nComma+1);

		nComma = sRem1.find_first_of(',');
		if(nComma <= 0)
			return false;

		string sTop = sRem1.substr(0, nComma);
		string sRem2 = sRem1.substr(nComma+1);

		nComma = sRem2.find_first_of(',');
		if(nComma <= 0)
			return false;

		string sRight = sRem2.substr(0, nComma);
		string sBottom = sRem2.substr(nComma+1);

		left = atof(sLeft.c_str());
		top = atof(sTop.c_str());
		right = atof(sRight.c_str());
		bottom = atof(sBottom.c_str());

		return true;
	}
	inline bool getRGBAFromString(string str, int& r, int& g, int& b, int& a)
	{
		if(str.length() <= 0)
			return false;

		string strAlpha = str.substr(7,2);
		stringstream ss1(strAlpha);
		ss1 >> hex >> a;

		string strR = str.substr(1,2);
		string strG = str.substr(3,2);
		string strB = str.substr(5,2);

		stringstream ss2(strR);
		ss2 >> hex >> r;
		stringstream ss3(strG);
		ss3 >> hex >> g;
		stringstream ss4(strB);
		ss4 >> hex >> b;
		return true;
	}
	inline bool isBuiltInColor(string str, CColor& builtInColor)
	{
		char c = str.at(0);
		if(c == '~')
		{
			if(strcmp(str.c_str(), "~ TransparentCColor") == 0)
			{
				builtInColor = kTransparentCColor; return true;
			}
			if(strcmp(str.c_str(), "~ BlackCColor") == 0)
			{
				builtInColor = kBlackCColor; return true;
			}
			if(strcmp(str.c_str(), "~ WhiteCColor") == 0)
			{
				builtInColor = kWhiteCColor; return true;
			}
			if(strcmp(str.c_str(), "~ GreyCColor") == 0)
			{
				builtInColor = kGreyCColor; return true;
			}
			if(strcmp(str.c_str(), "~ RedCColor") == 0)
			{
				builtInColor = kRedCColor; return true;
			}
			if(strcmp(str.c_str(), "~ GreenCColor") == 0)
			{
				builtInColor = kGreenCColor; return true;
			}
			if(strcmp(str.c_str(), "~ BlueCColor") == 0)
			{
				builtInColor = kBlueCColor; return true;
			}
			if(strcmp(str.c_str(), "~ YellowCColor") == 0)
			{
				builtInColor = kYellowCColor; return true;
			}
			if(strcmp(str.c_str(), "~ CyanCColor") == 0)
			{
				builtInColor = kCyanCColor; return true;
			}
			if(strcmp(str.c_str(), "~ MagentaCColor") == 0)
			{
				builtInColor = kMagentaCColor; return true;
			}
		}
		return false;
	}
	inline const CFontRef getBuiltInFont(string str)
	{
		char c = str.at(0);
		if(c == '~')
		{
			if(strcmp(str.c_str(), "~ SystemFont") == 0)
				return kSystemFont;
			if(strcmp(str.c_str(), "~ NormalFontVeryBig") == 0)
				return kNormalFontVeryBig;
			if(strcmp(str.c_str(), "~ NormalFontBig") == 0)
				return kNormalFontBig;
			if(strcmp(str.c_str(), "~ NormalFont") == 0)
				return kNormalFont;
			if(strcmp(str.c_str(), "~ NormalFontSmall") == 0)
				return kNormalFontSmall;
			if(strcmp(str.c_str(), "~ NormalFontSmaller") == 0)
				return kNormalFontSmaller;
			if(strcmp(str.c_str(), "~ NormalFontVerySmall") == 0)
				return kNormalFontVerySmall;
			if(strcmp(str.c_str(), "~ SymbolFont") == 0)
				return kSymbolFont;
		}
		return NULL;
	}
	inline CColor getCColor(const char_t* colorname)
	{
		if(strlen(colorname) <= 0)
			return CColor(0, 0, 0, 1);

		string sColorName(colorname);
		CColor theCColor;
		if(isBuiltInColor(sColorName, theCColor))
			return theCColor;
		else
		{
			// do we have it?
			if(m_XMLParser.hasColor(colorname))
			{
				const char_t* rgba = m_XMLParser.getColorAttribute(colorname, "rgba");
				string sRGBA(rgba);
				int r, g, b, a = 255;
				getRGBAFromString(sRGBA, r, g, b, a);
				return CColor(r, g, b, a);
			}
		}
		// --- may be empty/black
		return theCColor;
	}
	inline string COLORREFtoRGBAstring(DWORD cr)
	{
		char cResultR[32];
		char cResultG[32];
		char cResultB[32];
		BYTE r = GetRValue(cr);	/* get R, G, and B out of DWORD */
		BYTE g = GetGValue(cr);
		BYTE b = GetBValue(cr);
		sprintf(cResultR, "%X", r);
		sprintf(cResultG, "%X", g);
		sprintf(cResultB, "%X", b);
		string sR(cResultR);
		string sG(cResultG);
		string sB(cResultB);
		if(sR.size() == 1) sR = "0" + sR;
		if(sG.size() == 1) sG = "0" + sG;
		if(sB.size() == 1) sB = "0" + sB;
		string sResult = "#";
		sResult.append(sR);
		sResult.append(sG);
		sResult.append(sB);
		sResult.append("ff");

		return sResult;
	}
	inline bool checkAddCColor(const char_t* colorname, COLORREF cr)
	{
		if(strlen(colorname) <= 0)
			return false;

		if(m_XMLParser.hasColor(colorname))
			return true;

		CColor color;
		if(isBuiltInColor(colorname, color))
			return true;

		// --- make the string
		string sRGBA = COLORREFtoRGBAstring(cr);

		// --- add color
		return m_XMLParser.addColor(colorname, sRGBA.c_str());
	}

	inline CBitmap* getLoadBitmap(pugi::xml_node viewNode, const char_t* bmName = "bitmap")
	{
		const char_t* bitmapname = m_XMLParser.getAttribute(viewNode, bmName);
		const char_t* bitmapFilename = m_XMLParser.getBitmapAttribute(bitmapname, "path");

        if(strlen(bitmapname) <= 0)return NULL;
        if(strlen(bitmapFilename) <= 0)return NULL;

		CNinePartTiledBitmap* backgroundTiledBMP = NULL;
		CBitmap* backgroundBMP = NULL;

		if(m_XMLParser.isBitmapTiled(bitmapname))
		{
			const char_t* offsets = m_XMLParser.getBitmapAttribute(bitmapname, "nineparttiled-offsets");
			CCoord left, top, right, bottom = 0;
			getTiledOffsets(offsets, left, top, right, bottom);

			backgroundTiledBMP = loadTiledBitmap(bitmapFilename, left, top, right, bottom);
			return backgroundTiledBMP;
		}
		else
		{
			backgroundBMP = loadBitmap(bitmapFilename);
			return backgroundBMP;
		}

		return NULL;
	}

   	inline CBitmap* getLoadBitmap(const char_t* bmName)
	{
		const char_t* bitmapFilename = m_XMLParser.getBitmapAttribute(bmName, "path");

		CNinePartTiledBitmap* backgroundTiledBMP = NULL;
		CBitmap* backgroundBMP = NULL;

		if(m_XMLParser.isBitmapTiled(bmName))
		{
			const char_t* offsets = m_XMLParser.getBitmapAttribute(bmName, "nineparttiled-offsets");
			CCoord left, top, right, bottom = 0;
			getTiledOffsets(offsets, left, top, right, bottom);

			backgroundTiledBMP = loadTiledBitmap(bitmapFilename, left, top, right, bottom);
			return backgroundTiledBMP;
		}
		else
		{
			backgroundBMP = loadBitmap(bitmapFilename);
			return backgroundBMP;
		}

		return NULL;
	}

	inline int getEnumStringIndex(char* enumString, const char* testString)
	{
		string sEnumStr(enumString);
		string sTestStr(testString);
		int index = 0;
		bool bWorking = true;
		while(bWorking)
		{
			int nComma = sEnumStr.find_first_of(',');
			if(nComma <= 0)
			{
				if(sEnumStr == sTestStr)
					return index;

				bWorking = false;
			}
			else
			{
				string sL = sEnumStr.substr(0, nComma);
				sEnumStr = sEnumStr.substr(nComma+1);

				if(sL == sTestStr)
					return index;

				index++;
			}
		}

		return -1;
	}

   	inline int getNumEnums(char* string)
	{
		if(strlen(string) <= 0)
			return 0;
		std::string sTemp(string);
		std::string s(string);
		s.erase(std::remove(s.begin(), s.end(), ','), s.end());
		int nLen = sTemp.size() - s.size();
		return nLen+1;
	}

	inline char* getEnumString(char* string, int index)
	{
		int nLen = strlen(string);
		char* copyString = new char[nLen+1];

		strncpy(copyString, string, nLen);
        copyString[nLen] = '\0';

		for(int i=0; i<index+1; i++)
		{
			char * comma = ",";

			int j = strcspn (copyString,comma);

			if(i==index)
			{
				char* pType = new char[j+1];
				strncpy (pType, copyString, j);
				pType[j] = '\0';
				delete [] copyString;

				// special support for 2-state switches
				// (new in RAFX 5.4.14)
				if(strcmp(pType, "SWITCH_OFF") == 0)
				{
					delete [] pType;
					return "OFF";
				}
				else if(strcmp(pType, "SWITCH_ON") == 0)
				{
					delete [] pType;
					return "ON";
				}

				return pType;
			}
			else // remove it
			{
				char* pch = strchr(copyString,',');

				if(!pch)
				{
					delete [] copyString;
					return NULL;
				}

				int nLen = strlen(copyString);
				memcpy (copyString,copyString+j+1,nLen-j);
			}
		}

		delete [] copyString;
		return NULL;
	}

    inline void broadcastControlChange(int nTag, float fPluginValue, float fControlValue, CControl* pControl, CUICtrl* pCtrl)
	{
		if(isOptionMenuControl(pControl))
			fControlValue = fPluginValue;
		else if(isVertSwitchControl(pControl) || isHorizSwitchControl(pControl))
		{
			fControlValue = pControl->getValueNormalized();
			fPluginValue = fControlValue;
		}

		// --- vertical/hroizontal switch thing to transmit integers to the UI switcher view
		int nSwitchIndex = -1;
		if(isOptionMenuControl(pControl))
		{
			COptionMenu* pOM = (COptionMenu*)pControl;
			nSwitchIndex = pOM->getCurrentIndex();
		}
		else if(isVertSwitchControl(pControl) || isHorizSwitchControl(pControl))
		{
			nSwitchIndex = (int)pControl->getValue();
        }

		// --- THIS NEEDS TO BE FIRST
		for(int i=0; i<m_nViewContainerCount; i++)
		{
			// --- switch view?
			if(isRAFXSwitchView(m_ppViewContainers[i]) && nSwitchIndex >= 0)
			{
				int nContTag = ((CRAFXSwitchView*)m_ppViewContainers[i])->getControllerTag();
				if(nContTag == nTag)
                    ((CRAFXSwitchView*)m_ppViewContainers[i])->setCurrentViewIndex(nSwitchIndex);
			}
		}

		if(!pCtrl) return;

		for(int i=0; i<m_nAnimKnobCount; i++)
		{
			if(m_ppKnobControls[i] == pControl)
				continue;

			if(m_ppKnobControls[i]->getTag() == nTag)
			{
				m_ppKnobControls[i]->setValue(fControlValue);
				m_ppKnobControls[i]->invalid();
			}
			else if(m_ppKnobControls[i]->getTag() == LCD_KNOB)
			{
				if(m_pLCDControlMap[m_nAlphaWheelIndex] == nTag)
				{
					m_ppKnobControls[i]->setValue(fControlValue);
					m_ppKnobControls[i]->invalid();

					CUICtrl* pCtrl = m_pPlugIn->m_UIControlList.getAt(m_pLCDControlMap[m_nAlphaWheelIndex]);

					// --- update the edit control
					updateTextEdits(LCD_KNOB, m_ppKnobControls[i], pCtrl);
				}
			}
		}
		for(int i=0; i<m_nSliderCount; i++)
		{
			if(m_ppSliderControls[i] == pControl)
				continue;

			if(m_ppSliderControls[i]->getTag() == nTag)
			{
				m_ppSliderControls[i]->setValue(fControlValue);
				m_ppSliderControls[i]->invalid();
			}
		}
		for(int i=0; i<m_nKickButtonCount; i++)
		{
			if(m_ppKickButtonControls[i] == pControl)
				continue;

			if(m_ppKickButtonControls[i]->getTag() == nTag)
			{
				m_ppKickButtonControls[i]->setValue(fControlValue);
				m_ppKickButtonControls[i]->invalid();
			}
		}
		for(int i=0; i<m_nOnOffButtonCount; i++)
		{
			if(m_ppOnOffButtonControls[i] == pControl)
				continue;

			if(m_ppOnOffButtonControls[i]->getTag() == nTag)
			{
				m_ppOnOffButtonControls[i]->setValue(fControlValue);
				m_ppOnOffButtonControls[i]->invalid();
			}
		}
		for(int i=0; i<m_nVerticalSwitchCount; i++)
		{
			if(m_ppVerticalSwitchControls[i] == pControl)
				continue;

			if(m_ppVerticalSwitchControls[i]->getTag() == nTag)
			{
				float fV = fPluginValue*(float)(m_ppVerticalSwitchControls[i]->getMax());
				m_ppVerticalSwitchControls[i]->setValue((int)fV);
				m_ppVerticalSwitchControls[i]->invalid();
			}
		}
        for(int i=0; i<m_nHorizontalSwitchCount; i++)
		{
			if(m_ppHorizontalSwitchControls[i] == pControl)
				continue;

			if(m_ppHorizontalSwitchControls[i]->getTag() == nTag)
			{
				float fV = fPluginValue*(float)(m_ppHorizontalSwitchControls[i]->getMax());
				m_ppHorizontalSwitchControls[i]->setValue((int)fV);
				m_ppHorizontalSwitchControls[i]->invalid();
			}
		}
		for(int i=0; i<m_nVuMeterCount; i++)
		{
			if(m_ppVuMeterControls[i] == pControl)
				continue;

			if(m_ppVuMeterControls[i]->getTag() == nTag)
			{
				m_ppVuMeterControls[i]->setValue(fControlValue);
				m_ppVuMeterControls[i]->invalid();
			}
		}
		// --- text edits
		updateTextEdits(nTag, pControl, pCtrl);

		// --- option menus
		for(int i=0; i<m_nOptionMenuCount; i++)
		{
			if(m_ppOptionMenuControls[i] == pControl)
				continue;

			if(m_ppOptionMenuControls[i]->getTag() == nTag)
			{
				float fV = fPluginValue*(float)(m_ppOptionMenuControls[i]->getNbEntries() - 1);
				m_ppOptionMenuControls[i]->setCurrent((int)fV);
				m_ppOptionMenuControls[i]->invalid();
			}
		}
		for(int i=0; i<m_nXYPadCount; i++)
		{
			if(m_ppXYPads[i] == pControl)
				continue;

			if(isCXYPadWP(m_ppXYPads[i]))
			{
				if(((CXYPadWP*)m_ppXYPads[i])->getTagX() == nTag || ((CXYPadWP*)m_ppXYPads[i])->getTagY() == nTag)
				{
					float x, y;
					m_ppXYPads[i]->calculateXY(m_ppXYPads[i]->getValue(), x, y);
					y = -1.0*y + 1.0;

					if(((CXYPadWP*)m_ppXYPads[i])->getTagX() == nTag)
						x = fControlValue;
					else
						y = fControlValue;

					float val = m_ppXYPads[i]->calculateValue(x, y);
					m_ppXYPads[i]->setValue(val);
					m_ppXYPads[i]->invalid();
				}
			}
		}
	}

	inline CAnimKnob* getAlphaWheelKnob()
	{
		for(int i=0; i<m_nAnimKnobCount; i++)
		{
			if(m_ppKnobControls[i]->getTag() == ALPHA_WHEEL)
				return m_ppKnobControls[i];
		}
		return NULL;
	}

	inline CAnimKnob* getLCDValueKnob()
	{
		for(int i=0; i<m_nAnimKnobCount; i++)
		{
			if(m_ppKnobControls[i]->getTag() == LCD_KNOB)
				return m_ppKnobControls[i];
		}
		return NULL;
	}

	inline CTextEdit* getLCDTextEdit()
	{
		for(int i=0; i<m_nTextEditCount; i++)
		{
			if(m_ppTextEditControls[i]->getTag() == LCD_KNOB)
				return m_ppTextEditControls[i];
		}
		return NULL;
	}

	inline CXYPad* findJoystickXYPad(CControl* pControl)
	{
		for(int i=0; i<m_nXYPadCount; i++)
		{
			if(m_ppXYPads[i] == (CXYPadWP*)pControl)
				return m_ppXYPads[i];
		}
		return NULL;
	}

    inline float calcLogParameter(float fNormalizedParam)
	{
		return (pow(10.f, fNormalizedParam) - 1.0)/9.0;
	}

	inline float calcLogPluginValue(float fPluginValue)
	{
		return log10(9.0*fPluginValue + 1.0);
	}

	inline float calcVoltOctaveParameter(CUICtrl* pCtrl)
	{
		float fRawValue = pCtrl->fUserDisplayDataLoLimit;
		if(fRawValue > 0)
		{
			double dOctaves = fastlog2(pCtrl->fUserDisplayDataHiLimit/pCtrl->fUserDisplayDataLoLimit);
			if(pCtrl->uUserDataType == intData)
				fRawValue = fastlog2(*(pCtrl->m_pUserCookedIntData)/pCtrl->fUserDisplayDataLoLimit)/dOctaves;
			else if(pCtrl->uUserDataType == floatData)
				fRawValue = fastlog2(*(pCtrl->m_pUserCookedFloatData)/pCtrl->fUserDisplayDataLoLimit)/dOctaves;
			else if(pCtrl->uUserDataType == doubleData)
				fRawValue = fastlog2(*(pCtrl->m_pUserCookedDoubleData)/pCtrl->fUserDisplayDataLoLimit)/dOctaves;
			else if(pCtrl->uUserDataType == UINTData)
				fRawValue = *(pCtrl->m_pUserCookedUINTData);
		}

		return fRawValue;
	}

	inline float calcVoltOctavePluginValue(float fPluginValue, CUICtrl* pCtrl)
	{
		if(pCtrl->uUserDataType == UINTData)
			return *(pCtrl->m_pUserCookedUINTData);

		double dOctaves = fastlog2(pCtrl->fUserDisplayDataHiLimit/pCtrl->fUserDisplayDataLoLimit);
		float fDisplay = pCtrl->fUserDisplayDataLoLimit*fastpow2(fPluginValue*dOctaves); //(m_fDisplayMax - m_fDisplayMin)*value + m_fDisplayMin; //m_fDisplayMin*fastpow2(value*dOctaves);
		float fDiff = pCtrl->fUserDisplayDataHiLimit - pCtrl->fUserDisplayDataLoLimit;
		return (fDisplay - pCtrl->fUserDisplayDataLoLimit)/fDiff;
	}

	// --- called when value changes in text edit; it needs to store the new stringindex value and update itself
	inline float updateEditControl(CControl* pControl, CUICtrl* pCtrl)
	{
		const char* p = ((CTextEdit*)pControl)->getText();

		float fValue = 0.0;
		switch(pCtrl->uUserDataType)
		{
			case floatData:
			{
				float f = atof(p);
				if(f > pCtrl->fUserDisplayDataHiLimit) f = pCtrl->fUserDisplayDataHiLimit;
				if(f < pCtrl->fUserDisplayDataLoLimit) f = pCtrl->fUserDisplayDataLoLimit;
				p = floatToString(f,2);
				fValue = calcSliderVariable(pCtrl->fUserDisplayDataLoLimit, pCtrl->fUserDisplayDataHiLimit, f);

                // --- RAFX v6.6
				if(pCtrl->bLogSlider)
					fValue = calcLogParameter(fValue);
				else if(pCtrl->bExpSlider)
				{
					fValue = calcVoltOctaveParameter(pCtrl);
				}
				else
					fValue = calcSliderVariable(pCtrl->fUserDisplayDataLoLimit, pCtrl->fUserDisplayDataHiLimit, f);

                pControl->setValue(fValue);
				((CTextEdit*)pControl)->setText(p);
				break;
			}
			case doubleData:
			{
				float f = atof(p);
				if(f > pCtrl->fUserDisplayDataHiLimit) f = pCtrl->fUserDisplayDataHiLimit;
				if(f < pCtrl->fUserDisplayDataLoLimit) f = pCtrl->fUserDisplayDataLoLimit;
				p = floatToString(f,2);
				fValue = calcSliderVariable(pCtrl->fUserDisplayDataLoLimit, pCtrl->fUserDisplayDataHiLimit, f);

                // --- RAFX v6.6
				if(pCtrl->bLogSlider)
					fValue = calcLogParameter(fValue);
				else if(pCtrl->bExpSlider)
				{
					fValue = calcVoltOctaveParameter(pCtrl);
				}
				else
					fValue = calcSliderVariable(pCtrl->fUserDisplayDataLoLimit, pCtrl->fUserDisplayDataHiLimit, f);

                pControl->setValue(fValue);
				((CTextEdit*)pControl)->setText(p);
				break;
			}
			case intData:
			{
				int f = atoi(p);
				if(f > pCtrl->fUserDisplayDataHiLimit) f = pCtrl->fUserDisplayDataHiLimit;
				if(f < pCtrl->fUserDisplayDataLoLimit) f = pCtrl->fUserDisplayDataLoLimit;
				p = intToString(f);
				fValue = calcSliderVariable(pCtrl->fUserDisplayDataLoLimit, pCtrl->fUserDisplayDataHiLimit, f);

                // --- RAFX v6.6
				if(pCtrl->bLogSlider)
					fValue = calcLogParameter(fValue);
				else if(pCtrl->bExpSlider)
				{
					fValue = calcVoltOctaveParameter(pCtrl);
				}
				else
					fValue = calcSliderVariable(pCtrl->fUserDisplayDataLoLimit, pCtrl->fUserDisplayDataHiLimit, f);

                pControl->setValue(fValue);
				((CTextEdit*)pControl)->setText(p);
				break;
			}
			case UINTData:
			{
				string str(p);
				string list(pCtrl->cEnumeratedList);
				if(list.find(str) == -1)
				{
					fValue = calcSliderVariable(pCtrl->fUserDisplayDataLoLimit, pCtrl->fUserDisplayDataHiLimit, *(pCtrl->m_pUserCookedUINTData));
					pControl->setValue(fValue);

					char* pEnum;
					pEnum = getEnumString(pCtrl->cEnumeratedList, (int)(*(pCtrl->m_pUserCookedUINTData)));
					if(pEnum)
						((CTextEdit*)pControl)->setText(pEnum);
				}
				else
				{
					int t = getEnumStringIndex(pCtrl->cEnumeratedList, p);
					if(t < 0)
					{
						// this should never happen...
						char* pEnum;
						pEnum = getEnumString(pCtrl->cEnumeratedList, 0);
						if(pEnum)
							((CTextEdit*)pControl)->setText(pEnum);
						fValue = 0.0;
					}
					else
					{
						fValue = calcSliderVariable(pCtrl->fUserDisplayDataLoLimit, pCtrl->fUserDisplayDataHiLimit, (float)t);
						pControl->setValue(fValue);
						((CTextEdit*)pControl)->setText(str.c_str());
					}
				}

				break;
			}
			default:
				break;
		}

		return fValue;
	}

	inline void updateTextEdits(int nTag, CControl* pControl, CUICtrl* pCtrl)
	{
		if(!pCtrl) return;

		for(int i=0; i<m_nTextEditCount; i++)
		{
			if(m_ppTextEditControls[i]->getTag() == nTag)
			{
				m_ppTextEditControls[i]->setValueNormalized(pControl->getValueNormalized());
				switch(pCtrl->uUserDataType)
				{
					case floatData:
					{
						m_ppTextEditControls[i]->setText(floatToString(*pCtrl->m_pUserCookedFloatData,2));
						break;
					}
					case doubleData:
					{
						m_ppTextEditControls[i]->setText(doubleToString(*pCtrl->m_pUserCookedDoubleData,2));
						break;
					}
					case intData:
					{
						m_ppTextEditControls[i]->setText(intToString(*pCtrl->m_pUserCookedIntData));
						break;
					}
					case UINTData:
					{
						char* pEnum;
						pEnum = getEnumString(pCtrl->cEnumeratedList, (int)(*(pCtrl->m_pUserCookedUINTData)));
						if(pEnum)
							m_ppTextEditControls[i]->setText(pEnum);
						break;
					}
					default:
						break;
				}
			}
		}
	}

	inline float updateLCDGuts(int nLCDControlIndex)
	{
		// --- set the parameter info
		// This does the name label and the 1/3 indexCount label
		CUICtrl* pCtrl = m_pPlugIn->m_UIControlList.getAt(m_pLCDControlMap[nLCDControlIndex]);
		if(pCtrl)
		{
			string sUnits(pCtrl->cUnits);
			sUnits = trim(sUnits);
			if(sUnits.length() > 0)
			{
				string p1 = " (";
				p1.append(sUnits);
				p1.append(")");

				// -- this is	Name (units)
				string sName(pCtrl->cControlName);
				string nameUnits = sName.append(p1);
				m_pLCDControlNameLabel->setText(nameUnits.c_str());
			}
			else
                // --- name only
				m_pLCDControlNameLabel->setText(pCtrl->cControlName);

			int nIndex = nLCDControlIndex + 1;
			char* cIndex = intToString(nIndex);
			string sIndex(cIndex);

			sIndex.append("/");
			char* cCount = intToString(m_nLCDControlCount);
			sIndex.append(cCount);
			m_pLCDControlIndexCountLabel->setText(sIndex.c_str());
			delete cCount;
			delete cIndex;

		}

		// --- this is the Edit control
		CTextEdit* pControl = getLCDTextEdit(); //->setValue(pControl->getValue());
		if(pControl)
		{
			int nPlugInControlIndex = m_pLCDControlMap[m_nAlphaWheelIndex];
			pControl->setValue(m_pPlugIn->getParameter(nPlugInControlIndex));
			CUICtrl* pCtrl = m_pPlugIn->m_UIControlList.getAt(nPlugInControlIndex);
			if(pCtrl)
			{
				switch(pCtrl->uUserDataType)
				{
					case floatData:
					{
						pControl->setText(floatToString(*pCtrl->m_pUserCookedFloatData,2));
						break;
					}
					case doubleData:
					{
						pControl->setText(doubleToString(*pCtrl->m_pUserCookedDoubleData,2));
						break;
					}
					case intData:
					{
						pControl->setText(intToString(*pCtrl->m_pUserCookedIntData));
						break;
					}
					case UINTData:
					{
						char* pEnum;
						pEnum = getEnumString(pCtrl->cEnumeratedList, (int)(*(pCtrl->m_pUserCookedUINTData)));
						if(pEnum)
							pControl->setText(pEnum);
						break;
					}
					default:
						break;
				}
				pControl->invalid();
			}
		}

		// --- update the knob
		int nPlugInControlIndex = m_pLCDControlMap[m_nAlphaWheelIndex];
        float fRawValue = m_pPlugIn->getParameter(nPlugInControlIndex);
        pCtrl = m_pPlugIn->m_UIControlList.getAt(nPlugInControlIndex);

        if(pCtrl)
        {
            if(pCtrl->bLogSlider && pCtrl->uUserDataType != UINTData)
            {
                fRawValue = calcLogParameter(fRawValue);
            }
            else if(pCtrl->bExpSlider && pCtrl->uUserDataType != UINTData)
            {
                if(pCtrl->fUserDisplayDataLoLimit > 0)
                {
                    fRawValue = calcVoltOctaveParameter(pCtrl);
                }
            }
        }

        getLCDValueKnob()->setValue(fRawValue);
		getLCDValueKnob()->invalid();

        return fRawValue;
	}

    inline bool isKnobControl(CControl* pControl)
	{
		CAnimKnob* control = dynamic_cast<CAnimKnob*>(pControl);
		if(control)
			return true;

		return false;
	}
	inline bool isKnobControl(CView* pControl)
	{
		CAnimKnob* control = dynamic_cast<CAnimKnob*>(pControl);
		if(control)
			return true;
		return false;
	}

	inline bool isSliderControl(CControl* pControl)
	{
		CSlider* control = dynamic_cast<CSlider*>(pControl);
		if(control)
			return true;

		return false;
	}
	inline bool isSliderControl(CView* pControl)
	{
		CSlider* control = dynamic_cast<CSlider*>(pControl);
		if(control)
			return true;
		return false;
	}

	inline bool isTextEditControl(CControl* pControl)
	{
		CTextEdit* control = dynamic_cast<CTextEdit*>(pControl);
		if(control)
			return true;

		return false;
	}
	inline bool isTextEditControl(CView* pControl)
	{
		CTextEdit* control = dynamic_cast<CTextEdit*>(pControl);
		if(control)
			return true;
		return false;
	}

	inline bool isOptionMenuControl(CControl* pControl)
	{
		COptionMenu* control = dynamic_cast<COptionMenu*>(pControl);
		if(control)
			return true;

		return false;
	}
	inline bool isOptionMenuControl(CView* pControl)
	{
		COptionMenu* control = dynamic_cast<COptionMenu*>(pControl);
		if(control)
			return true;
		return false;
	}

	inline bool isRadioButtonControl(CControl* pControl)
	{
		CVerticalSwitch* control = dynamic_cast<CVerticalSwitch*>(pControl);
		if(control)
			return true;

		return false;
	}
 	// LABELS
	inline bool isTextLabelControl(CControl* pControl)
	{
		CTextLabel* control = dynamic_cast<CTextLabel*>(pControl);
		if(control)
			return true;
		return false;
	}
	inline bool isTextLabelControl(CView* pControl)
	{
		CTextLabel* control = dynamic_cast<CTextLabel*>(pControl);
		if(control)
			return true;
		return false;
	}

	// KICKBUTTONS
	inline bool isKickButtonControl(CControl* pControl)
	{
		CKickButton* control = dynamic_cast<CKickButton*>(pControl);
		if(control)
			return true;
		return false;
	}
	inline bool isKickButtonControl(CView* pControl)
	{
		CKickButton* control = dynamic_cast<CKickButton*>(pControl);
		if(control)
			return true;
		return false;
	}

	// ONOFFBUTTONS
	inline bool isOnOffButtonControl(CControl* pControl)
	{
		COnOffButton* control = dynamic_cast<COnOffButton*>(pControl);
		if(control)
			return true;
		return false;
	}
	inline bool isOnOffButtonControl(CView* pControl)
	{
		COnOffButton* control = dynamic_cast<COnOffButton*>(pControl);
		if(control)
			return true;
		return false;
	}

	// VERTICAL SWICTHES
	inline bool isVertSwitchControl(CControl* pControl)
	{
		CVerticalSwitch* control = dynamic_cast<CVerticalSwitch*>(pControl);
		if(control)
			return true;
		return false;
	}
	inline bool isVertSwitchControl(CView* pControl)
	{
		CVerticalSwitch* control = dynamic_cast<CVerticalSwitch*>(pControl);
		if(control)
			return true;
		return false;
	}

	// VU METERS
	inline bool isVuMeterControl(CControl* pControl)
	{
		CVuMeter* control = dynamic_cast<CVuMeter*>(pControl);
		if(control)
			return true;
		return false;
	}
	inline bool isVuMeterControl(CView* pControl)
	{
		CVuMeter* control = dynamic_cast<CVuMeter*>(pControl);
		if(control)
			return true;
		return false;
	}

	// XYPADS
	inline bool isXYPadControl(CControl* pControl)
	{
		CXYPad* control = dynamic_cast<CXYPad*>(pControl);
		if(control)
			return true;
		return false;
	}
	inline bool isXYPadControl(CView* pControl)
	{
		CXYPad* control = dynamic_cast<CXYPad*>(pControl);
		if(control)
			return true;
		return false;
	}


	// VERT SW
	inline bool isHorizSwitchControl(CControl* pControl)
	{
		CHorizontalSwitch* control = dynamic_cast<CHorizontalSwitch*>(pControl);
		if(control)
			return true;
		return false;
	}
	inline bool isHorizSwitchControl(CView* pControl)
	{
		CHorizontalSwitch* control = dynamic_cast<CHorizontalSwitch*>(pControl);
		if(control)
			return true;
		return false;
	}

	// VC
	inline bool isViewContainer(CView* pControl)
	{
		CViewContainer* control = dynamic_cast<CViewContainer*>(pControl);
		if(control)
			return true;
		return false;
	}

/*    inline bool isCRAFXKnobControl(CControl* pControl)
	{
		CRAFXKnob* control = dynamic_cast<CRAFXKnob*>(pControl);
		if(control)
			return true;

		return false;
	}
    */

	inline bool isCVuMeterWP(CControl* pControl)
	{
		CVuMeterWP* control = dynamic_cast<CVuMeterWP*>(pControl);
		if(control)
			return true;

		return false;
	}

	inline bool isCXYPadWP(CControl* pControl)
	{
		CXYPadWP* control = dynamic_cast<CXYPadWP*>(pControl);
		if(control)
			return true;

		return false;
	}

	inline bool isCXYPadWP(CXYPad* pControl)
	{
		CXYPadWP* control = dynamic_cast<CXYPadWP*>(pControl);
		if(control)
			return true;

		return false;
	}

 	inline void updateAnimKnobs(int nTag, float fValue)
	{
		for(int i=0; i<m_nAnimKnobCount; i++)
		{
			if(m_ppKnobControls[i]->getTag() == nTag)
			{
				m_ppKnobControls[i]->setValue(fValue);
				m_ppKnobControls[i]->invalid();
			}
		}
	}

   	inline float getPluginParameterValue(float fControlValue, CControl* pControl = NULL)
	{
		if(pControl)
		{
			if(isOptionMenuControl(pControl))
				return pControl->getValue()/((float)((COptionMenu*)pControl)->getNbEntries() - 1);
			else if(isVertSwitchControl(pControl) || isHorizSwitchControl(pControl))
				return pControl->getValue()/pControl->getMax();
			else
				return pControl->getValue();
		}

		return fControlValue;
	}

	// trim from start
	static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
	}

	// trim from end
	static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
	}

	// trim from both ends
	static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
	}

	inline float getInitControlValue(CUICtrl* pCtrl)
	{
		if(!pCtrl)
			return 0.0;

		switch(pCtrl->uUserDataType)
		{
			case floatData:
			{
				return pCtrl->fInitUserFloatValue;
				break;
			}
			case doubleData:
			{
				return pCtrl->fInitUserDoubleValue;
				break;
			}
			case intData:
			{
				return pCtrl->fInitUserIntValue;
				break;
			}
			case UINTData:
			{
				return pCtrl->fInitUserUINTValue;
				break;
			}
			default:
				break;
		}
		return 0;
	}

	inline float getInitDisplayValue(CUICtrl* pCtrl)
	{
		return calcDisplayVariable(pCtrl->fUserDisplayDataLoLimit, pCtrl->fUserDisplayDataHiLimit, getInitControlValue(pCtrl));
	}
	inline float getInitNormalizedValue(CUICtrl* pCtrl)
	{
		return calcSliderVariable(pCtrl->fUserDisplayDataLoLimit, pCtrl->fUserDisplayDataHiLimit, getInitControlValue(pCtrl));
	}

	inline CTextEdit* getLCDEditBox(int nTag)
	{
		for(int i=0; i<m_nTextEditCount; i++)
		{
			if(m_ppTextEditControls[i]->getTag() == nTag)
				return m_ppTextEditControls[i];
		}
		return NULL;
	}

	inline void inflateRect(CRect& rect, int x, int y)
	{
		rect.left -= x;
		rect.right += x;
		rect.top -= y;
		rect.bottom += y;
	}

    // -- not needed?
	inline bool hasRafxTemplate(string templateName)
	{
		return m_XMLParser.hasRafxTemplate(templateName.c_str());
	}

	string* getUserTemplates(int& nCount);
	inline pugi::xml_node getRafxXMLNode(CView* pV)
	{
		int32_t outSize;
		pugi::xml_node node;

		pV->getAttribute(RAFX_XML_NODE, sizeof(pugi::xml_node*), &node, outSize);

		return node;
	}

	inline const char* getViewNodeAttribute(CView* pV, const char* att)
	{
		pugi::xml_node node = getRafxXMLNode(pV);

		if(node.empty())
			return NULL;

		const char* attribute = m_XMLParser.getAttribute(node, att);

		return attribute;
	}

	inline string makeXYString(string strX, string strY)
	{
		string s("0, 0");
		if(strX.length() == 0)
			return s;
		if(strY.length() == 0)
			return s;

		string s2(strX);
		s2.append(" ,");
		s2.append(strY);
		return s2;
	}

    inline bool makeOriginSizeStrings(CRect rect, string& strOrigin, string& strSize)
	{
		string x(floatToString(rect.left, 0));
		string y(floatToString(rect.top, 0));
		strOrigin = makeXYString(x, y);

		string w(floatToString(rect.width(), 0));
		string h(floatToString(rect.height(), 0));
		strSize = makeXYString(w, h);
		return true;
	}

	inline void updateViewSize(CView* pView, const char* pSizeString)
	{
		if(strlen(pSizeString) <= 0)
			return;

		int32_t outSize;
		pugi::xml_node node;

		CRect viewSize;
		pView->getViewSize(viewSize);
		const CPoint origin2(viewSize.left, viewSize.top);
		const CPoint size2 = getCPointFromString(pSizeString);

		CRect newRect2(origin2, size2);
		pView->setViewSize(newRect2);
		pView->setMouseableArea(newRect2);
		pView->invalid();
	}


    // --- v6.6 Custom ---------------------
	//
   	inline bool isRAFXSwitchView(CViewContainer* pCont)
	{
		CRAFXSwitchView* control = dynamic_cast<CRAFXSwitchView*>(pCont);
		if(control)
			return true;

		return false;
	}

	void checkInitUISwitchViews(int nControlTag, int nControlValue);
  	void syncTabViews(CControl* pTabControl, int nControlTag, int nControlValue);

	// --- simple GUI API stuff
	void initPluginCustomGUI();

	VSTGUI_VIEW_INFO guiInfoStruct;

	inline bool fillUIDescriptionMap(std::map<std::string,std::string>* map, pugi::xml_node node)
	{
		map->clear();
		int nCount = m_XMLParser.getAttributeCount(node);
		for(int i=0; i<nCount; i++)
		{
			const char_t* attributeName;
			const char_t* attributeValue;

			attributeName = m_XMLParser.getAttributeName(node, i);
			attributeValue = m_XMLParser.getAttributeValue(node, i);

			if(attributeName && attributeValue)
				map->insert(std::make_pair(attributeName, attributeValue));

		}
		return true;
	}

    inline CView* checkCreateCustomView(pugi::xml_node node, void* p = NULL)
	{
		CView* pView = NULL;
		const char_t* customView = m_XMLParser.getAttribute(node, "custom-view-name");
		const char_t* ctagName = m_XMLParser.getAttribute(node, "control-tag");
		const char_t* origin =  m_XMLParser.getAttribute(node, "origin");
		const char_t* size =  m_XMLParser.getAttribute(node, "size");
		const char_t* offset =  m_XMLParser.getAttribute(node, "background-offset");
		const char_t* bitmap =  m_XMLParser.getAttribute(node, "bitmap");
		const char_t* bitmap2 =  m_XMLParser.getAttribute(node, "handle-bitmap");
		const char_t* bitmap3 =  m_XMLParser.getAttribute(node, "off-bitmap");
/*		const char_t* orientation =  m_XMLParser.getAttribute(node, "orientation");
		const char_t* backcolor =  m_XMLParser.getAttribute(node, "back-color");
		const char_t* framecolor =  m_XMLParser.getAttribute(node, "frame-color");
		const char_t* fontcolor =  m_XMLParser.getAttribute(node, "font-color");
		const char_t* framewidth =  m_XMLParser.getAttribute(node, "frame-width");
		const char_t* rrradius =  m_XMLParser.getAttribute(node, "round-rect-radius");
		const char_t* styleRoundRect =  m_XMLParser.getAttribute(node, "style-round-rect");
		const char_t* styleNoFrame =  m_XMLParser.getAttribute(node, "style-no-frame");
		const char_t* htOneImage =  m_XMLParser.getAttribute(node, "height-of-one-image");
		const char_t* subPixmaps =  m_XMLParser.getAttribute(node, "sub-pixmaps");*/

		string sOffset(offset);

		CRect rect = getRectFromOriginSizeStrings(origin, size);
		CPoint offsetPoint = getCPointFromString(sOffset);

		const char_t* ctag = m_XMLParser.getControlTagAttribute(ctagName, "tag");
		int32_t tag = -1;
		if(strlen(ctag) > 0)
			tag = atoi(ctag);

		VSTGUI_VIEW_INFO* pInfoStruct = new VSTGUI_VIEW_INFO;

		if(m_pPlugIn && strlen(customView) > 0)
		{
			std::map<std::string,std::string> attributes;
			if(fillUIDescriptionMap(&attributes, node))
			{
				pInfoStruct->message = GUI_CUSTOMVIEW;
				pInfoStruct->hPlugInInstance = NULL;
				pInfoStruct->hRAFXInstance = NULL;
				pInfoStruct->size.width = 0; pInfoStruct->size.height = 0;
				pInfoStruct->window = NULL;

				// --- custom view only!
				pInfoStruct->customViewName = (char*)customView;
				pInfoStruct->customViewTag = tag;
				pInfoStruct->customViewRect.top = rect.top;
				pInfoStruct->customViewRect.bottom = rect.bottom;
				pInfoStruct->customViewRect.left = rect.left;
				pInfoStruct->customViewRect.right = rect.right;
				pInfoStruct->customViewOffset.x = offsetPoint.x;
				pInfoStruct->customViewOffset.y = offsetPoint.y;

				if(strlen(bitmap) > 0)
				{
					pInfoStruct->customViewBitmapName = addStrings((char*)bitmap, ".png");
				}
				else
					pInfoStruct->customViewBitmapName = "";

                pInfoStruct->customViewHandleBitmapName = "";
				pInfoStruct->customViewOffBitmapName = "";

				if(strlen(bitmap2) > 0)
				{
					pInfoStruct->customViewHandleBitmapName = addStrings((char*)bitmap2, ".png"); // bitmap2 is for slider handle or LEDMeter-off only
				}

				if(strlen(bitmap3) > 0)
				{
					pInfoStruct->customViewOffBitmapName = addStrings((char*)bitmap3, ".png");
				}

				const char_t* orientation =  m_XMLParser.getAttribute(node, "orientation");
				pInfoStruct->customViewOrientation = (char*)orientation;

				pInfoStruct->customViewBackColor = NULL;
				pInfoStruct->customViewFrameColor = NULL;
				pInfoStruct->customViewFontColor = NULL;

				const char_t* backcolor =  m_XMLParser.getAttribute(node, "back-color");
				if(strlen(backcolor) > 0)
				{
					CColor backCColor = getCColor(backcolor);
					pInfoStruct->customViewBackColor = (void*)&backCColor;
				}

				const char_t* framecolor =  m_XMLParser.getAttribute(node, "frame-color");
				if(strlen(framecolor) > 0)
				{
					CColor frameCColor = getCColor(framecolor);
					pInfoStruct->customViewFrameColor = (void*)&frameCColor;
				}

				const char_t* fontcolor =  m_XMLParser.getAttribute(node, "font-color");
				if(strlen(fontcolor) > 0)
				{
					CColor vontCColor = getCColor(fontcolor);
					pInfoStruct->customViewFontColor = (void*)&vontCColor;
				}

				pInfoStruct->customViewFrameWidth = 0;
				const char_t* framewidth =  m_XMLParser.getAttribute(node, "frame-width");
				if(strlen(framewidth) > 0)
					pInfoStruct->customViewFrameWidth = atoi(framewidth);

				pInfoStruct->customViewRoundRectRadius = 0;
				const char_t* rrradius =  m_XMLParser.getAttribute(node, "round-rect-radius");
				if(strlen(rrradius) > 0)
					pInfoStruct->customViewRoundRectRadius = atoi(rrradius);

				pInfoStruct->customViewStyleRoundRect = false;
				const char_t* styleRoundRect =  m_XMLParser.getAttribute(node, "style-round-rect");
				if(strlen(styleRoundRect) > 0)
					if(strcmp(styleRoundRect, "true") == 0) pInfoStruct->customViewStyleRoundRect = true;

				pInfoStruct->customViewStyleNoFrame = false;
				const char_t* styleNoFrame =  m_XMLParser.getAttribute(node, "style-no-frame");
				if(strlen(styleNoFrame) > 0)
					if(strcmp(styleNoFrame, "true") == 0) pInfoStruct->customViewStyleNoFrame = true;

				pInfoStruct->customViewHtOneImage = 0;
				const char_t* htOneImage =  m_XMLParser.getAttribute(node, "height-of-one-image");
				if(strlen(htOneImage) > 0)
					pInfoStruct->customViewHtOneImage = atoi(htOneImage);

				pInfoStruct->customViewSubPixmaps = 0;
				const char_t* subPixmaps =  m_XMLParser.getAttribute(node, "sub-pixmaps");
				if(strlen(subPixmaps) > 0)
					pInfoStruct->customViewSubPixmaps = atoi(subPixmaps);

				pInfoStruct->subControllerName = "";
				pInfoStruct->editor = (void*)this;

				if(p)
					pInfoStruct->listener = p;
				else
					pInfoStruct->listener = (void*)this;

				pView = (CView*)(m_pPlugIn->showGUI((void*)(pInfoStruct)));

				attributes.clear();
			}
		}
		return pView;
	}

	inline const CRect getRectFromOriginSizeStrings(const char_t* origin, const char_t* size)
	{
		// --- make rect
		string sOrigin = string(origin);

		int x, y = 0;
		if(!getXYFromString(sOrigin, x, y))
		{
			const CRect rect(0,0,0,0);
			return rect;
		}

		string sSize = string(size);

		int w, h = 0;
		if(!getXYFromString(sSize, w, h))
		{
			const CRect rect(0,0,0,0);
			return rect;
		}
		const CRect rect(x, y, x+w, y+h);
		return rect;
	}

protected:
	CPlugIn* m_pPlugIn;
    AudioUnit m_AU;
 	AUEventListenerRef AUEventListener;

	UINT* m_pControlMap;
	UINT* m_pLCDControlMap;
	int m_nLCDControlCount;
	int m_nAlphaWheelIndex;

	// --- arrays - remember to add to CalculateFrameSize() when you addd morearrays
	CTextLabel** m_ppTextLabels;
	int m_nTextLabelCount;

	CTextEdit** m_ppTextEditControls;
	int m_nTextEditCount;

	CAnimKnob** m_ppKnobControls;
	int m_nAnimKnobCount;

	CSlider** m_ppSliderControls;
	int m_nSliderCount;

	CKickButton** m_ppKickButtonControls;
	int m_nKickButtonCount;

	COnOffButton** m_ppOnOffButtonControls;
	int m_nOnOffButtonCount;

	CVerticalSwitch** m_ppVerticalSwitchControls;
	int m_nVerticalSwitchCount;

    CHorizontalSwitch** m_ppHorizontalSwitchControls;
	int m_nHorizontalSwitchCount;

	CVuMeter** m_ppVuMeterControls;
	int m_nVuMeterCount;

	CXYPad** m_ppXYPads;
	int m_nXYPadCount;
	float m_fJS_X;
	float m_fJS_Y;

	COptionMenu** m_ppOptionMenuControls;
	int m_nOptionMenuCount;

	CViewContainer** m_ppViewContainers;
	int m_nViewContainerCount;

	CView** m_ppViews;
	int m_nViewCount;
};

}

#endif // __CRafxVSTEditor__

