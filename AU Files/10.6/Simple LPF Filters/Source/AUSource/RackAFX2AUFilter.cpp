/*	Copyright  2013 Tritone Systems, Inc. All Rights Reserved.

 Disclaimer: IMPORTANT:  This software is supplied to you by
 Tritone Systems Inc. ("Tritone Systems") in consideration of your agreement to the
 following terms, and your use, installation, modification or
 redistribution of this Tritone Systems software constitutes acceptance of these
 terms.  If you do not agree with these terms, please do not use,
 install, modify or redistribute this Tritone Systems software.

 The Tritone Systems Software is provided by Tritone Systems on an "AS IS" basis.
 Tritone Systems:
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE Tritone Systems SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.

 IN NO EVENT SHALL Tritone Systems BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE Tritone Systems SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF Tritone Systems HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.

 This software is based on the free sample code provided by Apple called FilterDemo. I modified
 this demo software. Please see the original source at:

 https://developer.apple.com/library/mac/#samplecode/FilterDemo/Introduction/Intro.html#//apple_ref/doc/uid/DTS10003570

 for more information.
 */

#include "AUEffectBase.h"
#include <AudioToolbox/AudioUnitUtilities.h>
#include "FilterVersion.h"
#include "RackAFX2AUFilter.h"
#include <math.h>

#include "SimpleFilterTester.h"

#pragma mark ____RackAFX2AUFilter

class RackAFX2AUFilter : public AUEffectBase
{
public:
	RackAFX2AUFilter(AudioUnit component);
    ~RackAFX2AUFilter();

	virtual ComponentResult		Version() { return kFilterVersion; }

	virtual ComponentResult		Initialize();

	// for custom property
	virtual ComponentResult		GetPropertyInfo(	AudioUnitPropertyID		inID,
													AudioUnitScope			inScope,
													AudioUnitElement		inElement,
													UInt32 &				outDataSize,
													Boolean	&				outWritable );

	virtual ComponentResult		GetProperty(		AudioUnitPropertyID 	inID,
													AudioUnitScope 			inScope,
													AudioUnitElement 		inElement,
													void 					* outData );


	virtual ComponentResult		GetParameterInfo(	AudioUnitScope			inScope,
													AudioUnitParameterID	inParameterID,
													AudioUnitParameterInfo	&outParameterInfo );

    // handle presets:
    virtual ComponentResult		GetPresets(	CFArrayRef	*outData	)	const;
    virtual OSStatus			NewFactoryPresetSet (	const AUPreset & inNewFactoryPreset	);

	// we'll report a 1ms tail.   A reverb effect would have a much more substantial tail on
	// the order of several seconds....
	//
	virtual	bool				SupportsTail () { return true; }
    virtual Float64				GetTailTime() {return 0.001;}

	// we have no latency
	//
	// A lookahead compressor or FFT-based processor should report the true latency in seconds
    virtual Float64				GetLatency() {return 0.0;}

    // Will's overrides for a *RAFX* Wrapper: Simple no GUI
    virtual ComponentResult 	SetParameter(AudioUnitParameterID			inID,
                                             AudioUnitScope 				inScope,
                                             AudioUnitElement 				inElement,
                                             AudioUnitParameterValue		inValue,
                                             UInt32							inBufferOffsetInFrames);

    virtual OSStatus			ProcessBufferLists(AudioUnitRenderActionFlags&      ioActionFlags,
                                                   const AudioBufferList&			inBuffer,
                                                   AudioBufferList&                 outBuffer,
                                                   UInt32							inFramesToProcess );

    virtual ComponentResult		Reset(AudioUnitScope 			inScope,
                                      AudioUnitElement 			inElement);

    virtual ComponentResult		GetParameterValueStrings(AudioUnitScope					inScope,
                                                         AudioUnitParameterID			inParameterID,
                                                         CFArrayRef *					outStrings);

    // need this for when user selects a NON factory-preset (ie they created the preset in the Client)
    virtual ComponentResult		RestoreState(CFPropertyListRef inData);

	virtual UInt32				SupportedNumChannels (	const AUChannelInfo**			outInfo);

    // *RAFX*
    CPlugIn* m_pRAFXPlugIn;
	int m_nCurrentPreset;
    void refreshAllControls();

    // VST/AU Buffers
    float** m_pVSTInputBuffers;
    float** m_pVSTOutputBuffers;

protected:
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Standard DSP AudioUnit implementation

COMPONENT_ENTRY(RackAFX2AUFilter)


// Factory presets
// -- Written by RackAFX: DO NOT MODIFY if you indend on using RackAFX to update this project later!
//
// -- 0xA983 --

static const int kNumberPresets = 1;

static AUPreset kPresets[kNumberPresets] = {
{ 0, CFSTR("Factory Preset") }
};

static const int kPresetDefault = 0;
static const int kPresetDefaultIndex = 0;

// -- 0xAF66 --

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma mark ____Construction_Initialization


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::RackAFX2AUFilter
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
RackAFX2AUFilter::RackAFX2AUFilter(AudioUnit component)
	: AUEffectBase(component, false) // false = do not use Process(), use ProcessBufferList() instead
{
    // (1) Create the RackAFX inner (wrapped) object
    //
    // *RAFX*
    m_pRAFXPlugIn = new CSimpleFilterTester;

    // (2) initialize
    m_pRAFXPlugIn->initialize();

    // (3) crearte VST/AU buffers, in case user has overridden that function
    m_pVSTInputBuffers = new float*[2];
    m_pVSTOutputBuffers = new float*[2];

	// (4) all the AU-parameters must be set to their initial values here
    //     this is equivalent to inittUI() which is the first method in the RackAFX constructor
	//
	// these calls have the effect both of defining the parameters for the first time
	// and assigning their initial values
    //
    //
    // From WP:
    // the "legal" values are identical in VST; that is, these do not support a few RackAFX variables
    // directly such as Meters
    // We are always going to use countLegalVSTIF() to get the control count
    int nParams = m_pRAFXPlugIn->m_UIControlList.countLegalVSTIF();

    // iterate
    for(int i = 0; i < nParams; i++)
    {
        // they are in AU proper order in the ControlList - do NOT reference them with RackAFX ID values any more!
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(i);

        if(pUICtrl)
        {
            // enum {intData, floatData, doubleData, UINTData, nonData};
            switch (pUICtrl->uUserDataType)
            {
                case intData:
                    AUEffectBase::SetParameter(i, (int)(*pUICtrl->m_pUserCookedIntData));
                    break;
                case floatData:
                    AUEffectBase::SetParameter(i, *pUICtrl->m_pUserCookedFloatData);
                    break;
                case doubleData:
                    AUEffectBase::SetParameter(i, (double)(*pUICtrl->m_pUserCookedDoubleData));
                    break;
                case UINTData:
                    AUEffectBase::SetParameter(i, (UINT)(*pUICtrl->m_pUserCookedUINTData));
                    break;

                default:
                    break;
            }
        }
    }

	// kFilterParam_CutoffFrequency max value depends on sample-rate
	SetParamHasSampleRateDependency(true );

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::~RackAFX2AUFilter
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
RackAFX2AUFilter::~RackAFX2AUFilter()
{
    // destory all allocated objects
 	if(m_pVSTInputBuffers)
        delete [] m_pVSTInputBuffers;

    if(m_pVSTOutputBuffers)
        delete [] m_pVSTOutputBuffers;


	if(m_pRAFXPlugIn)
		delete m_pRAFXPlugIn;

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



#pragma mark ____RackAFX main Functions


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::Reset -> RackAFX::prepareForPlay()
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult	RackAFX2AUFilter::Reset(AudioUnitScope 			inScope,
                                        AudioUnitElement              inElement)
{
    // this function is called once on startup and then every time PLAY is pressed
    // *RAFX*
    if(m_pRAFXPlugIn)
    {
        m_pRAFXPlugIn->m_nSampleRate = (int)GetSampleRate();
        
        // forward the call
        m_pRAFXPlugIn->prepareForPlay();
    }    return noErr;

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::ProcessBufferLists -> RackAFX::processAudioFrames() OR processVSTBuffers()
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// process buffers!
OSStatus RackAFX2AUFilter::ProcessBufferLists(AudioUnitRenderActionFlags &	ioActionFlags,
                                              const AudioBufferList &		inBuffer,
                                              AudioBufferList &				outBuffer,
                                              UInt32						inFramesToProcess )
{
    // (1) get information about i/o
    SInt16 auNumInputs = (SInt16) GetInput(0)->GetStreamFormat().mChannelsPerFrame;
	SInt16 auNumOutputs = (SInt16) GetOutput(0)->GetStreamFormat().mChannelsPerFrame;

    // get buffer pointers
    float* pInputL = (float*)inBuffer.mBuffers[0].mData;
    float* pOutputL = (float*)outBuffer.mBuffers[0].mData;
    float* pInputR = NULL;
    float* pOutputR = NULL;

    // RackAFX Frames; max is 2-channel but you can
    // change this if you want to process more channels
    float input[2];
    float output[2];

    // VST and AU share the same buffering system; named "VST" here because its called VST buffering in RackAFX
    m_pVSTInputBuffers[0] = pInputL;
    m_pVSTOutputBuffers[0] = pOutputL;
    m_pVSTInputBuffers[1] = NULL;
    m_pVSTOutputBuffers[1] = NULL;

    if(auNumInputs == 2)
    {
        pInputR = (float*)inBuffer.mBuffers[1].mData;
        m_pVSTInputBuffers[1] = pInputR;
    }

    if(auNumOutputs == 2)
    {
        pOutputR = (float*)outBuffer.mBuffers[1].mData;
        m_pVSTOutputBuffers[1] = pOutputR;
    }

    // VST and AU use an identical system for buffering; see appendinx A of my book
    if(m_pRAFXPlugIn->m_bWantVSTBuffers)
    {
        // NOTE: VST buffer processing only takes one channel value (assumes in and out are same numbers)
        //       So, check m_pVSTInputBuffers[1] == NULL and m_pVSTOutputBuffers[1] == NULL to figure out
        //       the channel counts.
        m_pRAFXPlugIn->processVSTAudioBuffer(m_pVSTInputBuffers, m_pVSTOutputBuffers, 2, inFramesToProcess);
    }
    else // standard RAFX frames
    {
        for(int i=0; i<inFramesToProcess; i++)
        {
            input[0] = pInputL[i];
            input[1] = pInputR ? pInputR[i] : 0.0;

            // forward the call to the inner object
            m_pRAFXPlugIn->processAudioFrame(&input[0], &output[0], auNumInputs, auNumOutputs);

            pOutputL[i] = output[0];
            if(pOutputR)
                pOutputR[i] = output[1];
        }
    }

    return noErr;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::SetParameter -> RackAFX::userInterfaceChange()
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// *RAFX* Override this so we can set the value directly on the plugin
//        and call userInterfaceChange()
ComponentResult RackAFX2AUFilter::SetParameter(AudioUnitParameterID	inID,
                                               AudioUnitScope 				inScope,
                                               AudioUnitElement 				inElement,
                                               AudioUnitParameterValue		inValue,
                                               UInt32							inBufferOffsetInFrames)
{
    if(m_pRAFXPlugIn)
    {
        // they are in AU proper order in the ControlList - do NOT reference them with RackAFX ID values any more!
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(inID);

        if(pUICtrl)
        {
            // these are the 4 RackAFX datatypes for controls
            // enum {intData, floatData, doubleData, UINTData, nonData};
            switch (pUICtrl->uUserDataType)
            {
                case intData:
                    *pUICtrl->m_pUserCookedIntData = (int)inValue;
                    break;
                case floatData:
                    *pUICtrl->m_pUserCookedFloatData = (float)inValue;
                    break;
                case doubleData:
                    *pUICtrl->m_pUserCookedDoubleData = (double)inValue;
                    break;
                case UINTData:
                    *pUICtrl->m_pUserCookedUINTData = (UINT)inValue;
                    break;

                default:
                    break;
            }

            // forward the call to the inner object
            m_pRAFXPlugIn->userInterfaceChange(inID);

        }
    }

    return AUBase::SetParameter(inID, inScope, inElement, inValue, inBufferOffsetInFrames);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



#pragma mark ____Extra RackAFX Functions

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::refreshAllControls
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void RackAFX2AUFilter::refreshAllControls()
{
    // for preset changes, etc...
    // may need later with GUIs
    int nParams = m_pRAFXPlugIn->m_UIControlList.countLegalVSTIF();
    for(int i = 0; i < nParams; i++)
    {
        // they are in AU proper order in the ControlList - do NOT reference them with RackAFX ID values any more!
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(i);

        if(pUICtrl)
        {
            double dParam = GetParameter(i);

            switch (pUICtrl->uUserDataType)
            {
                case intData:
                    *pUICtrl->m_pUserCookedIntData = (int)dParam;
                    break;
                case floatData:
                    *pUICtrl->m_pUserCookedFloatData = (float)dParam;
                    break;
                case doubleData:
                    *pUICtrl->m_pUserCookedDoubleData = (double)dParam;
                    break;
                case UINTData:
                    *pUICtrl->m_pUserCookedUINTData = (UINT)dParam;
                    break;

                default:
                    break;
            }

            m_pRAFXPlugIn->userInterfaceChange(i);
        }
    }
}



#pragma mark ____AU Functions: Init/Restore


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::SupportedNumChannels
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UInt32 RackAFX2AUFilter::SupportedNumChannels (const AUChannelInfo** outInfo)
{
	// set an array of arrays of different combinations of supported numbers
	// of ins and outs
    //
    // RackAFX explicitly supports 3 modes: 1x1, 1x2 and 2x2
    // Change this if your plugin can support more
	static const AUChannelInfo sChannels[3] = { {1, 1}, {1, 2}, {2, 2}};

    // write out
	if (outInfo) *outInfo = sChannels;

    // return var
	return sizeof (sChannels) / sizeof (AUChannelInfo);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::Initialize
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult	RackAFX2AUFilter::Initialize()
{
    // this is also placed here for clients such as AULab that do NOT call Reset()
    // before streaming audio! 
    if(m_pRAFXPlugIn)
    {
        m_pRAFXPlugIn->m_nSampleRate = (int)GetSampleRate();
        
        // forward the call
        m_pRAFXPlugIn->prepareForPlay();
    }
    
    ComponentResult result = AUEffectBase::Initialize();

	if(result == noErr )
	{
		// in case the AU was un-initialized and parameters were changed, the view can now
		// be updated
        //
	}

	return result;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::RestoreState
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// called when a user-made preset is updated
ComponentResult	RackAFX2AUFilter::RestoreState(CFPropertyListRef inData)
{
    ComponentResult result = AUBase::RestoreState(inData);

    // set everything
    refreshAllControls();

    return result;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



#pragma mark ____AU Functions: Parameters

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::GetParameterInfo
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult	RackAFX2AUFilter::GetParameterInfo(	AudioUnitScope			inScope,
                                                   AudioUnitParameterID     inParameterID,
                                                   AudioUnitParameterInfo	&outParameterInfo )
{
    // here, the client is querying us for each of our controls. It wants a description
    // (name) and I have set it up for custom units since that's the most general so
    // we also give it units.
    //
    // You will see similarities in this and what you set up in the slider in RackAFX
	ComponentResult result = noErr;

	outParameterInfo.flags = 	kAudioUnitParameterFlag_IsWritable
						+		kAudioUnitParameterFlag_IsReadable;

	if (inScope == kAudioUnitScope_Global)
    {
        // should not happen
        if(!m_pRAFXPlugIn)
            return kAudioUnitErr_InvalidParameter;

        // they are in AU proper order in the ControlList - do NOT reference them with RackAFX ID values any more!
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(inParameterID);

        if(!pUICtrl)
            return kAudioUnitErr_InvalidParameter;

        // make the name objects
        CFStringRef name = CFStringCreateWithCString(NULL, pUICtrl->cControlName, 8);
        CFStringRef units = CFStringCreateWithCString(NULL, pUICtrl->cControlUnits, 8);

        // fill in the name; you have to call a function to do this
        AUBase::FillInParameterName (outParameterInfo, name, false);

        // if UINT data, tell it we are Indexed; this will make it query us for
        // strings to fill a dropdown control; those strings are chunks of your
        // enum string for that control
        if(pUICtrl->uUserDataType == UINTData)
            outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;

        else
        {
            // custom, set units
            outParameterInfo.unit = kAudioUnitParameterUnit_CustomUnit;
            outParameterInfo.unitName = units;
        }

        // set min and max
        outParameterInfo.minValue = pUICtrl->fUserDisplayDataLoLimit;
        outParameterInfo.maxValue = pUICtrl->fUserDisplayDataHiLimit;

        // initialize
        switch (pUICtrl->uUserDataType)
        {
            case intData:
                outParameterInfo.defaultValue = pUICtrl->fInitUserIntValue;
                break;
            case floatData:
                outParameterInfo.defaultValue = pUICtrl->fInitUserFloatValue;
                break;
            case doubleData:
                outParameterInfo.defaultValue = pUICtrl->fInitUserDoubleValue;
                break;
            case UINTData:
                outParameterInfo.defaultValue = pUICtrl->fInitUserUINTValue;
                break;

            default:
                break;
        }

        // rest of flags
        outParameterInfo.flags += kAudioUnitParameterFlag_IsHighResolution;

        // for log sliders in RackAFX
        if(pUICtrl->bLogSlider)
            outParameterInfo.flags += kAudioUnitParameterFlag_DisplayLogarithmic;
	}
    else
    {
		result = kAudioUnitErr_InvalidParameter;
	}

	return result;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::GetParameterValueStrings
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// this fills the default I/F Dropown Boxes with the enumerated strings
ComponentResult	RackAFX2AUFilter::GetParameterValueStrings(AudioUnitScope			inScope,
                                                     AudioUnitParameterID			inParameterID,
                                                     CFArrayRef *					outStrings)
{

    if (inScope == kAudioUnitScope_Global)
    {

        if (outStrings == NULL)
            return noErr;

        // these will be enum UINT types of controls
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(inParameterID);

        if(!pUICtrl)
            return kAudioUnitErr_InvalidParameter;

        // convert the list into an array
        CFStringRef enumList = CFStringCreateWithCString(NULL, pUICtrl->cEnumeratedList, 8);
        CFStringRef comma CFSTR(",");
        CFArrayRef strings = CFStringCreateArrayBySeparatingStrings(NULL, enumList, comma);

        // create the array COPY (important: these are local variables above!)
        *outStrings = CFArrayCreateCopy(NULL, strings);

        return noErr;
    }
    return kAudioUnitErr_InvalidParameter;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



#pragma mark ____AU Functions: Properties

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::GetPropertyInfo
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult	RackAFX2AUFilter::GetPropertyInfo (AudioUnitPropertyID				inID,
                                                   AudioUnitScope					inScope,
                                                   AudioUnitElement                 inElement,
                                                   UInt32 &                         outDataSize,
                                                   Boolean &						outWritable)
{
	if (inScope == kAudioUnitScope_Global)
	{

		switch (inID)
		{
			case kAudioUnitProperty_CocoaUI:
				outWritable = false;
				outDataSize = sizeof (AudioUnitCocoaViewInfo);
				return noErr;
		}

	}

	return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::GetProperty
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult	RackAFX2AUFilter::GetProperty (AudioUnitPropertyID          inID,
                                               AudioUnitScope 				inScope,
                                               AudioUnitElement             inElement,
                                               void *						outData)
{
	if (inScope == kAudioUnitScope_Global)
	{

		switch (inID)
		{
                // This property allows the host application to find the UI associated with this
                // AudioUnit
                //
			case kAudioUnitProperty_CocoaUI:
			{
				// Look for a resource in the main bundle by name and type. // NOTE!!! WP com.apple.demo.audiounit.FilterDemo
				CFBundleRef bundle = CFBundleGetBundleWithIdentifier( CFSTR("developer.audiounit.Simple LPF Filters") );

				if (bundle == NULL)
                    return fnfErr;

				CFURLRef bundleURL = CFBundleCopyResourceURL( bundle,
                                                             CFSTR("Simple LPF Filters"),	// this is the name of the cocoa bundle as specified in the CocoaViewFactory.plist
                                                             CFSTR("bundle"),			// this is the extension of the cocoa bundle
                                                             NULL);

                if (bundleURL == NULL)
                    return fnfErr;

				CFStringRef className = CFSTR("Simple LPF Filters_ViewFactory");	// name of the main class that implements the AUCocoaUIBase protocol
				AudioUnitCocoaViewInfo cocoaInfo = { bundleURL, className };
				*((AudioUnitCocoaViewInfo *)outData) = cocoaInfo;

				return noErr;
			}
		}

	}

	// if we've gotten this far, handles the standard properties
	return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



#pragma mark ____AU Functions: Presets

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::GetPresets
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult	RackAFX2AUFilter::GetPresets (CFArrayRef *outData) const
{
    // this is used to determine if presets are supported
    // which in this unit they are so we implement this method!
	if (outData == NULL) return noErr;

	// make the array
	CFMutableArrayRef theArray = CFArrayCreateMutable (NULL, kNumberPresets, NULL);

    // copy our preset names
	for (int i = 0; i < kNumberPresets; ++i)
    {
		CFArrayAppendValue (theArray, &kPresets[i]);
    }

    *outData = (CFArrayRef)theArray; //(NULL, (CFIndex)nPrograms, theArray);

	//*outData = (CFArrayRef)theArray;	// client is responsible for releasing the array
	return noErr;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::NewFactoryPresetSet
//
//  In RackAFX, presets are held as individual lists of controls. For each preset, there is
//  a linked list of CUICtrl object that represent that preset. These are created above
//  in initializePresetClones() -- the clones are the copies of the presets
//  The presets are stored on arrays on each control in the main list. You probably
//  dont' want to mess with this stuff.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus RackAFX2AUFilter::NewFactoryPresetSet (const AUPreset & inNewFactoryPreset)
{
	SInt32 chosenPreset = inNewFactoryPreset.presetNumber;

    if (chosenPreset < 0 || chosenPreset >= kNumberPresets)
		return kAudioUnitErr_InvalidPropertyValue;

	//OutputDebugString("setProgram()\n");
    m_nCurrentPreset = chosenPreset;

    int nParams = m_pRAFXPlugIn->m_UIControlList.countLegalVSTIF();
	for(int j=0; j<nParams; j++)
	{
		// my controls
		CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(j);

		double dPreset = pUICtrl->dPresetData[m_nCurrentPreset];

        // set on base
        AUEffectBase::SetParameter(j, dPreset);

		// save the default data
		switch(pUICtrl->uUserDataType)
		{
			case intData:
			{
                *(pUICtrl->m_pUserCookedIntData) = int(dPreset);
				break;
			}
			case floatData:
			{
                *(pUICtrl->m_pUserCookedFloatData) = float(dPreset);
				break;
			}
			case doubleData:
			{
                *(pUICtrl->m_pUserCookedDoubleData) = dPreset;
				break;
			}
			case UINTData:
			{
                *(pUICtrl->m_pUserCookedUINTData) = UINT(dPreset);
				break;
			}
			default:
			{
				break;
			}
		}

        // forward the call to the inner object
        m_pRAFXPlugIn->userInterfaceChange(j);

		// now do the Joystick!
		float* pJSProg = m_pRAFXPlugIn->m_PresetJSPrograms[chosenPreset];

		for(int i=0; i<MAX_JS_PROGRAM_STEPS; i++)
		{
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,0)] = pJSProg[JS_PROG_INDEX(i,0)];
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,1)] = pJSProg[JS_PROG_INDEX(i,1)];
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,2)] = pJSProg[JS_PROG_INDEX(i,2)];
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,3)] = pJSProg[JS_PROG_INDEX(i,3)];
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,4)] = pJSProg[JS_PROG_INDEX(i,4)];
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,5)] = pJSProg[JS_PROG_INDEX(i,5)];
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,6)] = pJSProg[JS_PROG_INDEX(i,6)];
		}
	}

	// now the additional presets
	float* pPresetData = m_pRAFXPlugIn->m_AddlPresetValues[m_nCurrentPreset];
	if(pPresetData)
	{
		int nCount = m_pRAFXPlugIn->getNumAddtlPresets();
		for(int i=0; i<nCount; i++)
		{
			m_pRAFXPlugIn->setAddtlPresetValue(i, pPresetData[i]);
		}
	}

    return noErr;

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~





