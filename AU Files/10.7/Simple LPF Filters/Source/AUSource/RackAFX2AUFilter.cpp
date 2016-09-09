/*
 File: RackAFX2AUFilter.cpp

 Originally provided in Apple Sample AU Code 2007 as FilterDemo.cpp
 Modified by Will Pirkle for use with RackAFX Software 2015
 www.willpirkle.com

*/

#define fxplugin
//#define synthplugin

#include "AUInstrumentBase.h"   // for Synths
#include "AUEffectBase.h"       // For FX
#include <AudioToolbox/AudioUnitUtilities.h>
#include "FilterVersion.h"
#include "RackAFX2AUFilter.h"
#include <math.h>

#include "SimpleFilterTester.h"
#include "RafxVSTEditor.h"
#include "ftypes.h"

using namespace VSTGUI;
using namespace Steinberg;

#pragma mark ____RackAFX2AUFilter
namespace VSTGUI
{

// 0x4201
#ifdef fxplugin
class RackAFX2AUFilter : public AUEffectBase
{
public:
	RackAFX2AUFilter(AudioUnit component);
    ~RackAFX2AUFilter();

	virtual ComponentResult	Version() {return kFilterVersion;}

	virtual ComponentResult	Initialize();

	virtual ComponentResult	GetPropertyInfo(AudioUnitPropertyID	inID,
                                            AudioUnitScope		nScope,
                                            AudioUnitElement	inElement,
                                            UInt32&             outDataSize,
                                            Boolean&			outWritable );

	virtual ComponentResult	GetProperty(AudioUnitPropertyID inID,
                                        AudioUnitScope 		inScope,
                                        AudioUnitElement 	inElement,
                                        void*               outData );

	virtual OSStatus SetProperty(AudioUnitPropertyID inID,
                                 AudioUnitScope 	 inScope,
                                 AudioUnitElement 	 inElement,
                                 const void*		 inData,
                                 UInt32 			 inDataSize);

	virtual ComponentResult	GetParameterInfo(AudioUnitScope			inScope,
                                             AudioUnitParameterID	inParameterID,
                                             AudioUnitParameterInfo	&outParameterInfo );

    // --- handle presets:
    virtual ComponentResult	GetPresets(CFArrayRef* outData)	const;
    virtual OSStatus NewFactoryPresetSet (const AUPreset& inNewFactoryPreset);

	// --- report a 0.5000 sec tail.
	virtual	bool SupportsTail() {return true;}
    virtual Float64	GetTailTime() {return 0.5000;}

    // --- NOTE: AU takes latency in seconds, not samples; this is recalculated
    //           during init() and reset() operations
    Float64 m_fLatencyInSeconds;
    virtual Float64	GetLatency() {return m_fLatencyInSeconds;}

    virtual ComponentResult SetParameter(AudioUnitParameterID	 inID,
                                         AudioUnitScope 		 inScope,
                                         AudioUnitElement 		 inElement,
                                         AudioUnitParameterValue inValue,
                                         UInt32					 inBufferOffsetInFrames);

    virtual OSStatus Render(AudioUnitRenderActionFlags &		ioActionFlags,
                            const AudioTimeStamp &              inTimeStamp,
                            UInt32                              inNumberFrames);


    virtual OSStatus ProcessBufferLists(AudioUnitRenderActionFlags& ioActionFlags,
                                        const AudioBufferList&		inBuffer,
                                        AudioBufferList&            outBuffer,
                                        UInt32					    inFramesToProcess );

    virtual ComponentResult	Reset(AudioUnitScope   inScope,
                                  AudioUnitElement inElement);

    virtual ComponentResult	GetParameterValueStrings(AudioUnitScope		  inScope,
                                                     AudioUnitParameterID inParameterID,
                                                     CFArrayRef*		  outStrings);

    // --- need this for when user selects a NON factory-preset (ie they created the preset in the Client)
    virtual ComponentResult	RestoreState(CFPropertyListRef inData);

	virtual UInt32 SupportedNumChannels(const AUChannelInfo** outInfo);

    // --- *RAFX*
    CPlugIn* m_pRAFXPlugIn;
	int m_nCurrentPreset;
    void refreshAllControls();

    // --- VST/AU Buffers
    float** m_pVSTInputBuffers;
    float** m_pVSTOutputBuffers;
    float** m_pVSTSidechainInputBuffers;

    // --- sidechaining
    bool m_bHasSidechain;
    AudioBufferList* m_pSidechainBufferList;
    int m_nSidechainChannelCount;

    // --- VSTGUI4
    CRafxVSTEditor m_RafxVSTGUI;
    UINT* createControlMap();
    UINT* createLCDControlMap(int& nLCDControlCount);
    int getGUIControlCount();
    int getLCDControlCount();
};
// 0x4202
#else

class RackAFX2AUFilter : public AUInstrumentBase
{
public:
    // --- const/dest
    RackAFX2AUFilter(AudioUnit inComponentInstance);
    virtual	~RackAFX2AUFilter();

    virtual OSStatus Initialize();

    virtual UInt32 SupportedNumChannels(const AUChannelInfo** outInfo);

    virtual OSStatus Version() {return 0x00010000;}

    OSStatus RestoreState(CFPropertyListRef plist);

    virtual OSStatus Reset(AudioUnitScope   inScope,
                           AudioUnitElement inElement);

    virtual OSStatus Render(AudioUnitRenderActionFlags& ioActionFlags,
                            const AudioTimeStamp&       inTimeStamp,
                            UInt32                      inNumberFrames);

     virtual OSStatus GetParameterInfo(AudioUnitScope          inScope,
                                      AudioUnitParameterID    inParameterID,
                                      AudioUnitParameterInfo& outParameterInfo);

    virtual OSStatus GetParameterValueStrings(AudioUnitScope       inScope,
                                              AudioUnitParameterID inParameterID,
                                              CFArrayRef*          outStrings);

    virtual OSStatus GetPropertyInfo(AudioUnitPropertyID inID,
                                     AudioUnitScope      inScope,
                                     AudioUnitElement    inElement,
                                     UInt32&             outDataSize,
                                     Boolean&            outWritable);

    virtual OSStatus GetProperty(AudioUnitPropertyID inID,
                                 AudioUnitScope      inScope,
                                 AudioUnitElement    inElement,
                                 void*               outData);

    virtual OSStatus SetProperty(AudioUnitPropertyID inID,
                                 AudioUnitScope      inScope,
                                 AudioUnitElement    inElement,
                                 const void*         inData,
                                 UInt32              inDataSize);

    virtual ComponentResult SetParameter(AudioUnitParameterID		 inID,
                                             AudioUnitScope 		 inScope,
                                             AudioUnitElement 		 inElement,
                                             AudioUnitParameterValue inValue,
                                             UInt32					 inBufferOffsetInFrames);

    // --- MIDI Functions
    //
    // --- MIDI Note On
    virtual OSStatus StartNote(MusicDeviceInstrumentID     inInstrument,
                               MusicDeviceGroupID          inGroupID,
                               NoteInstanceID*             outNoteInstanceID,
                               UInt32                      inOffsetSampleFrame,
                               const MusicDeviceNoteParams &inParams);

    // --- MIDI Note Off
    virtual OSStatus StopNote(MusicDeviceGroupID inGroupID,
                              NoteInstanceID     inNoteInstanceID,
                              UInt32             inOffsetSampleFrame);

    // --- MIDI Pitchbend (slightly different from all other CCs)
    virtual OSStatus HandlePitchWheel(UInt8  inChannel,
                                      UInt8  inPitch1,
                                      UInt8  inPitch2,
                                      UInt32 inStartFrame);

    // --- all other MIDI CC messages
    virtual OSStatus HandleControlChange(UInt8  inChannel,
                                         UInt8  inController,
                                         UInt8  inValue,
                                         UInt32	inStartFrame);

    // --- for ALL other MIDI messages you can get them here
    OSStatus HandleMidiEvent(UInt8  status,
                             UInt8  channel,
                             UInt8  data1,
                             UInt8  data2,
                             UInt32 inStartFrame);

    // --- handle presets:
    virtual ComponentResult	GetPresets(CFArrayRef* outData)	const;
    virtual OSStatus NewFactoryPresetSet(const AUPreset& inNewFactoryPreset);

    // --- *RAFX*
    CPlugIn* m_pRAFXPlugIn;
	int m_nCurrentPreset;
    void refreshAllControls();

    // --- VST/AU Buffers
    float** m_pVSTInputBuffers;
    float** m_pVSTOutputBuffers;
    float** m_pVSTSidechainInputBuffers;

    // --- sidechaining
    bool m_bHasSidechain;
    AudioBufferList* m_pSidechainBufferList;
    int m_nSidechainChannelCount;

    // --- VSTGUI4
    CRafxVSTEditor m_RafxVSTGUI;
    UINT* createControlMap();
    UINT* createLCDControlMap(int& nLCDControlCount);
    int getGUIControlCount();
    int getLCDControlCount();
};
// 0x4302
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getMyComponentDirectory()
//
// returns the directory where the .component resides
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
char* getMyComponentDirectory(CFStringRef bundleID)
{
    if (bundleID != NULL)
    {
        CFBundleRef helixBundle = CFBundleGetBundleWithIdentifier( bundleID );
        if(helixBundle != NULL)
        {
            CFURLRef bundleURL = CFBundleCopyBundleURL ( helixBundle );
            if(bundleURL != NULL)
            {
                CFURLRef componentFolderPathURL = CFURLCreateCopyDeletingLastPathComponent(NULL, bundleURL);

                CFStringRef myComponentPath = CFURLCopyFileSystemPath(componentFolderPathURL, kCFURLPOSIXPathStyle);
                CFRelease(componentFolderPathURL);

                if(myComponentPath != NULL)
                {
                    int nSize = CFStringGetLength(myComponentPath);
                    char* path = new char[nSize+1];
                    memset(path, 0, (nSize+1)*sizeof(char));

                    bool success = CFStringGetCString(myComponentPath, path, nSize+1, kCFStringEncodingASCII);
                    CFRelease(myComponentPath);

                    if(success) return path;
                    else return NULL;
                }
                CFRelease(bundleURL);
            }
        }
        CFRelease(bundleID);
    }
    return NULL;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Standard DSP AudioUnit implementation

#ifdef fxplugin
AUDIOCOMPONENT_ENTRY(AUBaseFactory, RackAFX2AUFilter)
#else
AUDIOCOMPONENT_ENTRY(AUMusicDeviceFactory, RackAFX2AUFilter)
#endif

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

const UINT FILTER_CONTROL_USER_VSTGUI_VARIABLE = 106;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma mark ____Construction_Initialization


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::RackAFX2AUFilter
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef fxplugin
RackAFX2AUFilter::RackAFX2AUFilter(AudioUnit component) : AUEffectBase(component, false) // false = do not use Process(), use ProcessBufferList() instead
#else
RackAFX2AUFilter::RackAFX2AUFilter(AudioUnit inComponentInstance) : AUInstrumentBase(inComponentInstance, 0, 1) // No inputs, One output
#endif
{
    CreateElements(); // --- create input, output ports, groups and parts

    // (1) Create the RackAFX inner (wrapped) object
    //
    // *RAFX*
    m_pRAFXPlugIn = new CSimpleFilterTester;

    // (2) call the post creation initialize()
    m_pRAFXPlugIn->initialize();

    // (3) create VST/AU buffers, in case user has overridden that function
    m_pVSTInputBuffers = new float*[2];
    m_pVSTOutputBuffers = new float*[2];
    m_pVSTSidechainInputBuffers = new float*[2];

	// (4) all the AU-parameters must be set to their initial values here
    //     this is equivalent to inittUI() which is the first method in the RackAFX constructor
	//
	//     these calls have the effect both of defining the parameters for the first time
	//     and assigning their initial values
    //
    //     NOTE: this is only for the default GUI; the custom GUI deals with parameters independently
    //
    // the "legal" values are identical in VST; that is, these do not support a few RackAFX variables
    // directly such as Meters
    // We are always going to use countLegalVSTIF() to get the control count except in Custom GUI
    int nParams = m_pRAFXPlugIn->m_bUseCustomVSTGUI ? m_pRAFXPlugIn->m_UIControlList.count() :
                                                      m_pRAFXPlugIn->m_UIControlList.countLegalVSTIF();

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
                    Globals()->SetParameter(i, (int)(*pUICtrl->m_pUserCookedIntData));
                    break;
                case floatData:
                    Globals()->SetParameter(i, *pUICtrl->m_pUserCookedFloatData);
                    break;
                case doubleData:
                    Globals()->SetParameter(i, (double)(*pUICtrl->m_pUserCookedDoubleData));
                    break;
                case UINTData:
                    Globals()->SetParameter(i, (UINT)(*pUICtrl->m_pUserCookedUINTData));
                    break;

                default:
                    break;
            }
        }
    }

    // --- sidechain
    m_bHasSidechain = false;
    m_pSidechainBufferList = NULL;
    m_nSidechainChannelCount = 0;

#ifdef fxplugin
    m_bHasSidechain = m_pRAFXPlugIn->m_uPlugInEx[ENABLE_SIDECHAIN_VSTAU];

    if(m_bHasSidechain)
    {
        SetBusCount(kAudioUnitScope_Input, 2);
        // Inputs().SetNumberOfElements(2);
        SafeGetElement(kAudioUnitScope_Input, 0)->SetName(CFSTR("Main Input"));
        SafeGetElement(kAudioUnitScope_Input, 1)->SetName(CFSTR("Sidechain"));
    }

    m_fLatencyInSeconds = 0.0;
    SetParamHasSampleRateDependency(true);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::~RackAFX2AUFilter
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
RackAFX2AUFilter::~RackAFX2AUFilter()
{
    // --- destory all allocated objects
 	if(m_pVSTInputBuffers)
        delete [] m_pVSTInputBuffers;

    if(m_pVSTSidechainInputBuffers)
        delete [] m_pVSTSidechainInputBuffers;

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
ComponentResult	RackAFX2AUFilter::Reset(AudioUnitScope 	 inScope,
                                        AudioUnitElement inElement)
{
    // --- reset the base class
    AUBase::Reset(inScope, inElement);

    // --- this function is called once on startup and then every time PLAY is pressed
    // *RAFX*
    if(m_pRAFXPlugIn)
    {
#ifdef fxplugin
        // --- set our latency whenever sample rate changes
        m_fLatencyInSeconds = m_pRAFXPlugIn->m_fPlugInEx[0] / GetOutput(0)->GetStreamFormat().mSampleRate;
#endif

		// --- set sample rate on plugin
        m_pRAFXPlugIn->m_nSampleRate = GetOutput(0)->GetStreamFormat().mSampleRate;

        // forward the call
        m_pRAFXPlugIn->prepareForPlay();
    }

    return noErr;

}

#pragma mark ____AU Functions: Init/Restore

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::SupportedNumChannels
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UInt32 RackAFX2AUFilter::SupportedNumChannels (const AUChannelInfo** outInfo)
{
    // --- set an array of arrays of different combinations of supported numbers
    //     of ins and outs
    if(m_pRAFXPlugIn->m_bOutputOnlyPlugIn)
    {
        // --- Synths are mono/stereo
        static const AUChannelInfo sChannels[2] = {{0, 1}, {0, 2}};

        if(outInfo) *outInfo = sChannels;
        return sizeof (sChannels) / sizeof (AUChannelInfo);

    }
    else
    {
        // --- RackAFX explicitly supports 3 modes for FX Processing: 1x1, 1x2 and 2x2
        static const AUChannelInfo sChannels[3] = { {1, 1}, {1, 2}, {2, 2}};

        if(outInfo) *outInfo = sChannels;
        return sizeof (sChannels) / sizeof (AUChannelInfo);
    }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::Initialize
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult	RackAFX2AUFilter::Initialize()
{
    // --- this is also placed here for clients such as AULab that do NOT call Reset() before streaming audio!
    if(m_pRAFXPlugIn)
    {
#ifdef fxplugin
        // --- set our latency whenever sample rate changes
        m_fLatencyInSeconds = m_pRAFXPlugIn->m_fPlugInEx[0] / GetOutput(0)->GetStreamFormat().mSampleRate;
#endif

		// --- set sample rate on plugin
        m_pRAFXPlugIn->m_nSampleRate = GetOutput(0)->GetStreamFormat().mSampleRate;

        // forward the call
        m_pRAFXPlugIn->prepareForPlay();
    }

    ComponentResult result = noErr;

#ifdef fxplugin
    result = AUEffectBase::Initialize();
#else
    result = AUInstrumentBase::Initialize();
#endif

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

    // --- set everything
    refreshAllControls();

    return result;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#pragma mark ____AU Functions: Process Audio

#ifdef fxplugin
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::Render -> this is only for picking up sidechain inputs, if they exist
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus RackAFX2AUFilter::Render(AudioUnitRenderActionFlags &		ioActionFlags,
                                  const AudioTimeStamp &            inTimeStamp,
                                  UInt32                            inNumberFrames)
{
    // --- this is only for sidechain support!
    bool bSCAvailable;
    try {bSCAvailable = HasInput(1);}
    catch (...) {bSCAvailable = false;}

    if(!m_bHasSidechain || !bSCAvailable)
    {
        m_pSidechainBufferList = NULL;
        m_nSidechainChannelCount = 0;
        return  AUEffectBase::Render(ioActionFlags, inTimeStamp, inNumberFrames);
    }

    // --- pick up the sidechain buffer list, if there is one
    m_nSidechainChannelCount = 0;

    if(bSCAvailable)
    {
        AUInputElement* SCInput = GetInput(1);
        if(SCInput != NULL)
        {
            OSStatus status = SCInput->PullInput(ioActionFlags, inTimeStamp, 1, inNumberFrames);
            if(status == noErr)
            {
                m_nSidechainChannelCount = SCInput->NumberChannels();
                m_pSidechainBufferList = &(SCInput->GetBufferList());
            }
            else
                m_pSidechainBufferList = NULL;
        }
        else
            m_pSidechainBufferList = NULL;
    }
    else
        m_pSidechainBufferList = NULL;

    return AUEffectBase::Render(ioActionFlags, inTimeStamp, inNumberFrames);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::ProcessBufferLists -> RackAFX::processAudioFrames() OR processVSTBuffers()
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

OSStatus RackAFX2AUFilter::ProcessBufferLists(AudioUnitRenderActionFlags& ioActionFlags,
                                              const AudioBufferList&	  inBuffer,
                                              AudioBufferList&			  outBuffer,
                                              UInt32					  inFramesToProcess )
{
    // --- get information about i/o
    SInt16 auNumInputs = (SInt16) GetInput(0)->GetStreamFormat().mChannelsPerFrame;
	SInt16 auNumOutputs = (SInt16) GetOutput(0)->GetStreamFormat().mChannelsPerFrame;

    // --- get buffer pointers
    float* pInputL = (float*)inBuffer.mBuffers[0].mData;
    float* pOutputL = (float*)outBuffer.mBuffers[0].mData;
    float* pInputR = NULL;
    float* pOutputR = NULL;

    // --- RackAFX Frames; max is 2-channel but you can
    //     change this if you want to process more channels
    float input[2];
    float output[2];
    float scInput[2] = {0};

    // --- VST and AU share the same buffering system; named "VST" here because its called VST buffering in RackAFX
    m_pVSTInputBuffers[0] = pInputL;
    m_pVSTOutputBuffers[0] = pOutputL;
    m_pVSTInputBuffers[1] = NULL;
    m_pVSTOutputBuffers[1] = NULL;

    // --- sidechain
    float* pSidechainInputL = NULL;
    float* pSidechainInputR = NULL;

    audioProcessData auxInputProcessData;
    auxInputProcessData.uInputBus = 1;
    auxInputProcessData.bInputEnabled = false;
    auxInputProcessData.uNumInputChannels = 0;
    auxInputProcessData.uBufferSize = 0;
    auxInputProcessData.pFrameInputBuffer = NULL;
    auxInputProcessData.pRAFXInputBuffer = NULL;
    auxInputProcessData.ppVSTInputBuffer = NULL;

    // --- m_pSidechainBufferList is only non-null when SC is enabled in both client and RAFX
    if(m_pSidechainBufferList)
    {
        pSidechainInputL = (float*)m_pSidechainBufferList->mBuffers[0].mData;
        if(m_nSidechainChannelCount == 2)
            pSidechainInputR = (float*)m_pSidechainBufferList->mBuffers[1].mData;

        m_pVSTSidechainInputBuffers[0] = pSidechainInputL;
        m_pVSTSidechainInputBuffers[1] = pSidechainInputR;

        auxInputProcessData.bInputEnabled = true;
        auxInputProcessData.uNumInputChannels = m_nSidechainChannelCount;
    }

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

    // --- VST and AU use an identical system for buffering; see appendinx A of my FX book
    if(m_pRAFXPlugIn->m_bWantVSTBuffers)
    {
        if(m_pSidechainBufferList)
        {
            auxInputProcessData.ppVSTInputBuffer = m_pVSTSidechainInputBuffers; //** to sidechain

            // --- process sidechain
            m_pRAFXPlugIn->processAuxInputBus(&auxInputProcessData);
        }


        // NOTE: VST buffer processing only takes one channel value (assumes in and out are same numbers)
        //       So, check m_pVSTInputBuffers[1] == NULL and m_pVSTOutputBuffers[1] == NULL to figure out
        //       the channel counts.
        m_pRAFXPlugIn->processVSTAudioBuffer(m_pVSTInputBuffers, m_pVSTOutputBuffers, 2, inFramesToProcess);
    }
    else // --- standard RAFX frames
    {
        for(int i=0; i<inFramesToProcess; i++)
        {
            input[0] = pInputL[i];
            input[1] = pInputR ? pInputR[i] : 0.0;

            if(m_pSidechainBufferList)
            {
                scInput[0] = pSidechainInputL[i];
                if(m_nSidechainChannelCount == 2)
                    scInput[1] = pSidechainInputR[i];

                auxInputProcessData.pFrameInputBuffer = &scInput[0]; //* to sidechain

                // --- process sidechain
                m_pRAFXPlugIn->processAuxInputBus(&auxInputProcessData);
            }

            // --- forward the call to the inner object
            m_pRAFXPlugIn->processAudioFrame(&input[0], &output[0], auNumInputs, auNumOutputs);

            pOutputL[i] = output[0];
            if(pOutputR)
                pOutputR[i] = output[1];
        }
    }

    return noErr;
}

#else

OSStatus RackAFX2AUFilter::Render(AudioUnitRenderActionFlags& ioActionFlags,
                                  const AudioTimeStamp&       inTimeStamp,
                                  UInt32                      nNumberFrames)
{

    // --- broadcast MIDI events
    PerformEvents(inTimeStamp);

    // --- get the number of channels
    AudioBufferList& bufferList = GetOutput(0)->GetBufferList();
    UInt32 numChans = bufferList.mNumberBuffers;

    // --- we only support mono/stereo
	if(numChans > 2)
        return kAudioUnitErr_FormatNotSupported;

    // --- get pointers for buffer lists
    float* pOutputL = (float*)bufferList.mBuffers[0].mData;
    float* pOutputR = numChans == 2 ? (float*)bufferList.mBuffers[1].mData : NULL;

    // --- RackAFX Frames; max is 2-channel but you can
    //     change this if you want to process more channels
    float input[2];
    float output[2];

    // --- VST and AU share the same buffering system; named "VST" here because its called VST buffering in RackAFX
    m_pVSTOutputBuffers[0] = pOutputL;
    m_pVSTOutputBuffers[1] = pOutputR;
    m_pVSTInputBuffers[0] = pOutputL; // dummy, not used in synth
    m_pVSTInputBuffers[1] = pOutputR; // dummy, not used in synth

    // --- VST and AU use an identical system for buffering; see appendinx A of my FX book
    if(m_pRAFXPlugIn->m_bWantVSTBuffers)
    {
        // NOTE: VST buffer processing only takes one channel value (assumes in and out are same numbers)
        //       So, check m_pVSTInputBuffers[1] == NULL and m_pVSTOutputBuffers[1] == NULL to figure out
        //       the channel counts.
        m_pRAFXPlugIn->processVSTAudioBuffer(m_pVSTInputBuffers, m_pVSTOutputBuffers, numChans, nNumberFrames);
    }
    else // --- standard RAFX frames
    {
        for(int i=0; i<nNumberFrames; i++)
        {
            input[0] = 0.0;
            input[1] = 0.0;

            // --- forward the call to the inner object
            m_pRAFXPlugIn->processAudioFrame(&input[0], &output[0], numChans, numChans);

            pOutputL[i] = output[0];
            if(pOutputR)
                pOutputR[i] = output[1];
        }
    }

    return noErr;
}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::SetParameter -> RackAFX::userInterfaceChange()
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// *RAFX* Override this so we can set the value directly on the plugin
//        and call userInterfaceChange() - this makes the controls sample accurate as
//        GetGlobals() does not have to be called from process method.
ComponentResult RackAFX2AUFilter::SetParameter(AudioUnitParameterID	   inID,
                                               AudioUnitScope 		   inScope,
                                               AudioUnitElement 	   inElement,
                                               AudioUnitParameterValue inValue,
                                               UInt32				   inBufferOffsetInFrames)
{
    if(m_pRAFXPlugIn)
    {
        // --- they are in AU proper order in the ControlList
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(inID);

        if(pUICtrl)
        {
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

            // --- forward the call to the inner object
            m_pRAFXPlugIn->userInterfaceChange(pUICtrl->uControlId);
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
    int nParams = m_pRAFXPlugIn->m_bUseCustomVSTGUI ? m_pRAFXPlugIn->m_UIControlList.count() :
                                                      m_pRAFXPlugIn->m_UIControlList.countLegalVSTIF();

    for(int i = 0; i < nParams; i++)
    {
        // --- they are in AU proper order in the ControlList
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(i);

        if(pUICtrl)
        {
            double dParam = Globals()->GetParameter(i);

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

            m_pRAFXPlugIn->userInterfaceChange(pUICtrl->uControlId);
        }
    }
}

#pragma mark ____AU Functions: Parameters

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::GetParameterInfo
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult	RackAFX2AUFilter::GetParameterInfo(AudioUnitScope		   inScope,
                                                   AudioUnitParameterID    inParameterID,
                                                   AudioUnitParameterInfo& outParameterInfo )
{
    // --- here, the client is querying us for each of our controls. It wants a description
    //     (name) and I have set it up for custom units since that's the most general so
    //     we also give it units.
    //
    // --- You will see similarities in this and what you set up in the slider in RackAFX
	ComponentResult result = noErr;

	outParameterInfo.flags = kAudioUnitParameterFlag_IsWritable + kAudioUnitParameterFlag_IsReadable;

	if(inScope == kAudioUnitScope_Global)
    {
        // --- should not happen
        if(!m_pRAFXPlugIn)
            return kAudioUnitErr_InvalidParameter;

        // --- they are in AU proper order in the ControlList
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(inParameterID);

        if(!pUICtrl)
            return kAudioUnitErr_InvalidParameter;

        // --- make the name objects
        CFStringRef name = CFStringCreateWithCString(NULL, pUICtrl->cControlName, 8);
        CFStringRef units = CFStringCreateWithCString(NULL, pUICtrl->cControlUnits, 8);

        // --- fill in the name; you have to call a function to do this
        AUBase::FillInParameterName (outParameterInfo, name, false);

        // --- if UINT data, tell it we are Indexed; this will make it query us for
        //     strings to fill a dropdown control; those strings are chunks of your
        //     enum string for that control
        if(pUICtrl->uUserDataType == UINTData)
            outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
        else
        {
            // --- custom, set units
            outParameterInfo.unit = kAudioUnitParameterUnit_CustomUnit;
            outParameterInfo.unitName = units;
        }

        // --- set min and max
        outParameterInfo.minValue = pUICtrl->fUserDisplayDataLoLimit;
        outParameterInfo.maxValue = pUICtrl->fUserDisplayDataHiLimit;

        // --- initialize
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

        // --- rest of flags
        outParameterInfo.flags += kAudioUnitParameterFlag_IsHighResolution;

        // --- for log sliders in RackAFX
        //if(pUICtrl->bLogSlider || pUICtrl->bExpSlider)
        //    outParameterInfo.flags += kAudioUnitParameterFlag_DisplayLogarithmic;
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
ComponentResult	RackAFX2AUFilter::GetParameterValueStrings(AudioUnitScope       inScope,
                                                           AudioUnitParameterID	inParameterID,
                                                           CFArrayRef *			outStrings)
{
    if (inScope == kAudioUnitScope_Global)
    {
        if (outStrings == NULL)
            return noErr;

        // --- these will be enum UINT types of controls
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(inParameterID);

        if(!pUICtrl)
            return kAudioUnitErr_InvalidParameter;

        // --- convert the list into an array
        CFStringRef enumList = CFStringCreateWithCString(NULL, pUICtrl->cEnumeratedList, 8);
        CFStringRef comma CFSTR(",");
        CFArrayRef strings = CFStringCreateArrayBySeparatingStrings(NULL, enumList, comma);

        // --- create the array COPY (important: these are local variables above!)
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
#ifdef fxplugin
ComponentResult	RackAFX2AUFilter::GetPropertyInfo(AudioUnitPropertyID inID,
                                                  AudioUnitScope      inScope,
                                                  AudioUnitElement    inElement,
                                                  UInt32&             outDataSize,
                                                  Boolean&            outWritable)
#else
OSStatus RackAFX2AUFilter::GetPropertyInfo(AudioUnitPropertyID inID,
                                           AudioUnitScope      inScope,
                                           AudioUnitElement    inElement,
                                           UInt32&             outDataSize,
                                           Boolean&            outWritable)
#endif
{
	if (inScope == kAudioUnitScope_Global)
	{
  		switch(inID)
		{
            // --- m_bUseCustomVSTGUI: we have a Cocoa GUI
			case kAudioUnitProperty_CocoaUI:
            {
                if(m_pRAFXPlugIn->m_bUseCustomVSTGUI)
                {
                    outWritable = false;
                    outDataSize = sizeof(AudioUnitCocoaViewInfo);
                    return noErr;
                }
            }
            case kOpenGUI:
            {
                outDataSize = sizeof(TPtrInt);
                outWritable = false;
                return noErr;
            }
		}
	}

#ifdef fxplugin
	return AUEffectBase::GetPropertyInfo(inID, inScope, inElement, outDataSize, outWritable);
#else
	return MusicDeviceBase::GetPropertyInfo(inID, inScope, inElement, outDataSize, outWritable);
#endif
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::GetProperty
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef fxplugin
ComponentResult	RackAFX2AUFilter::GetProperty(AudioUnitPropertyID inID,
                                              AudioUnitScope      inScope,
                                              AudioUnitElement    inElement,
                                              void*               outData)
#else
OSStatus RackAFX2AUFilter::GetProperty(AudioUnitPropertyID inID,
                                       AudioUnitScope      inScope,
                                       AudioUnitElement    inElement,
                                       void*               outData)
#endif
{
	if (inScope == kAudioUnitScope_Global)
	{
		switch(inID)
		{
            // --- This property allows the host application to find the UI associated with this
			case kAudioUnitProperty_CocoaUI:
			{
				// --- Look for a resource in the main bundle by name and type.
				CFBundleRef bundle = CFBundleGetBundleWithIdentifier( CFSTR("developer.audiounit.Simple LPF Filters") );

				if(bundle == NULL) return fnfErr;

				CFURLRef bundleURL = CFBundleCopyResourceURL(bundle,
                                                             CFSTR("Simple LPF Filters"),	// this is the name of the cocoa bundle as specified in the CocoaViewFactory.plist
                                                             CFSTR("bundle"),			// this is the extension of the cocoa bundle
                                                             NULL);

                if(bundleURL == NULL) return fnfErr;

				CFStringRef className = CFSTR("Simple LPF Filters_ViewFactory");	// name of the main class that implements the AUCocoaUIBase protocol
				AudioUnitCocoaViewInfo cocoaInfo = { bundleURL, className };
				*((AudioUnitCocoaViewInfo *)outData) = cocoaInfo;

				return noErr;
			}
            return kAudioUnitErr_InvalidProperty;
		}
	}
#ifdef fxplugin
	// if we've gotten this far, handles the standard properties
	return AUEffectBase::GetProperty(inID, inScope, inElement, outData);
#else
   	return AUBase::GetProperty(inID, inScope, inElement, outData);
#endif
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::SetProperty
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus	RackAFX2AUFilter::SetProperty(AudioUnitPropertyID inID,
                                          AudioUnitScope 	  inScope,
                                          AudioUnitElement 	  inElement,
                                          const void*		  inData,
                                          UInt32 			  inDataSize)
{
   	if (inScope == kAudioUnitScope_Global)
	{
		switch (inID)
		{
            case kOpenGUI:
            {
                VIEW_STRUCT* pVS = (VIEW_STRUCT*)inData;

                // --- first see if they have a custom GUI
                void* createdCustomGUI = NULL;
                VSTGUI_VIEW_INFO info;
                info.message = GUI_HAS_USER_CUSTOM;
                info.bHasUserCustomView = false; // this flag will be set if they do
                m_pRAFXPlugIn->showGUI((void*)&info);

                if(info.bHasUserCustomView)
                {
                    info.message = GUI_USER_CUSTOM_OPEN;
                    info.window = (void*)pVS->pWindow;
                    info.hPlugInInstance = (void*)pVS->au;  // for AU, this is the AU object param
                    info.hRAFXInstance = NULL;              // unused in AU
                    info.size.width = 0;
                    info.size.height = 0;
                    info.bHasUserCustomView = true;

                    createdCustomGUI = m_pRAFXPlugIn->showGUI((void*)&info);
                    if(createdCustomGUI)
                    {
                        pVS->width = info.size.width;
                        pVS->height = info.size.height;
                        return noErr;
                        break;
                    }
                }
                if(!createdCustomGUI)
                {
                    // --- Look for a resource in the main bundle by name and type.
                    CFBundleRef bundle = CFBundleGetBundleWithIdentifier( CFSTR("developer.audiounit.Simple LPF Filters") );

                    if (bundle == NULL) return fnfErr;

                    // --- get .uidesc file
                    CFURLRef bundleURL = CFBundleCopyResourceURL(bundle,CFSTR("RackAFX"),CFSTR("uidesc"),NULL);
                    CFStringRef xmlPath = CFURLCopyPath(bundleURL);
                    int nSize = CFStringGetLength(xmlPath);
                    char* path = new char[nSize+1];
                    memset(path, 0, (nSize+1)*sizeof(char));
                    CFStringGetCString(xmlPath, path, nSize+1, kCFStringEncodingASCII);
                    CFRelease(xmlPath);

                    // --- create GUI
                    m_RafxVSTGUI.open(pVS->pWindow, m_pRAFXPlugIn, path, pVS->au);
                    m_RafxVSTGUI.getSize(pVS->width, pVS->height);

                    // --- v6.6
                    m_RafxVSTGUI.initPluginCustomGUI();

                    // --- setup control maps
                    int nLCDControlCount = 0;
                    UINT* pControlMap = createControlMap();
                    UINT* pLCDControlMap = createLCDControlMap(nLCDControlCount);

                    m_RafxVSTGUI.setControlMap(pControlMap);
                    m_RafxVSTGUI.setLCDControlMap(pLCDControlMap);
                    m_RafxVSTGUI.setLCDControlCount(nLCDControlCount);
                    m_RafxVSTGUI.initControls();

                    delete [] path;
                    return noErr;
                    break;
                }
                break;
            }
            case kCloseGUI:
            {
                VSTGUI_VIEW_INFO info;
                info.message = GUI_HAS_USER_CUSTOM;
                info.bHasUserCustomView = false; // this flag will be set if they do
                m_pRAFXPlugIn->showGUI((void*)&info);
                if(info.bHasUserCustomView)
                {
                    info.message = GUI_USER_CUSTOM_CLOSE;
                    m_pRAFXPlugIn->showGUI((void*)&info);
                }
                else
                    m_RafxVSTGUI.close();

                return noErr;
                break;
            }
        }
    }
   return noErr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::createControlMap
//  creates mapping arrays from RAFX control indexes to VSTGUI indexes
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UINT* RackAFX2AUFilter::createControlMap()
{
    int nCount = getGUIControlCount();
    UINT *pControlMap = nCount > 0 ? new UINT[nCount] : NULL;

    int m = 0;
    int nControls = m_pRAFXPlugIn->m_UIControlList.count();
    for(int i = 0; i< nControls; i++)
    {
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(i);

        if(pUICtrl->uControlType != FILTER_CONTROL_CONTINUOUSLY_VARIABLE &&
           pUICtrl->uControlType != FILTER_CONTROL_COMBO_VARIABLE &&
           pUICtrl->uControlType != FILTER_CONTROL_USER_VSTGUI_VARIABLE &&
           pUICtrl->uControlType != FILTER_CONTROL_RADIO_SWITCH_VARIABLE &&
           pUICtrl->uControlType != FILTER_CONTROL_LED_METER)
            continue;

        // --- add the index
        if(pControlMap)
            pControlMap[m++] = i;

    }

    return pControlMap;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::createLCDControlMap
//  creates mapping arrays from RAFX control indexes to VSTGUI indexes
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UINT* RackAFX2AUFilter::createLCDControlMap(int& nLCDControlCount)
{
    nLCDControlCount = getLCDControlCount();
    UINT *pLCDControlMap = nLCDControlCount > 0 ? new UINT[nLCDControlCount] : NULL;

    int m = 0;
    int nControls = m_pRAFXPlugIn->m_UIControlList.count();
    for(int i = 0; i< nControls; i++)
    {
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(i);

        if(pUICtrl->uControlTheme[31] == 1 && pLCDControlMap)
            pLCDControlMap[m++] = i;
    }
    return pLCDControlMap;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::getGUIControlCount
//  returns number of VSTGUI4 controls (see RackAFX Plugin->Add/Remove User Variables
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int RackAFX2AUFilter::getGUIControlCount()
{
    int nCount = 0;
    int nControls = m_pRAFXPlugIn->m_UIControlList.count();
    for(int i = 0; i< nControls; i++)
    {
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(i);

        if(pUICtrl->uControlType != FILTER_CONTROL_CONTINUOUSLY_VARIABLE &&
           pUICtrl->uControlType != FILTER_CONTROL_COMBO_VARIABLE &&
           pUICtrl->uControlType != FILTER_CONTROL_USER_VSTGUI_VARIABLE &&
           pUICtrl->uControlType != FILTER_CONTROL_RADIO_SWITCH_VARIABLE &&
           pUICtrl->uControlType != FILTER_CONTROL_LED_METER)
            continue;

        // --- inc
        nCount++;
    }
    return nCount;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::getLCDControlCount
//  returns number of embedded controls in LCD
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int RackAFX2AUFilter::getLCDControlCount()
{
    int nLCDCount = 0;
    int nControls = m_pRAFXPlugIn->m_UIControlList.count();
    for(int i = 0; i< nControls; i++)
    {
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(i);
        if(pUICtrl->uControlTheme[31] == 1)
            nLCDCount++;
    }
    return nLCDCount;
}

#pragma mark ____AU Functions: Presets

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	RackAFX2AUFilter::GetPresets
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef fxplugin
ComponentResult	RackAFX2AUFilter::GetPresets(CFArrayRef *outData) const
#else
OSStatus RackAFX2AUFilter::GetPresets(CFArrayRef *outData) const
#endif
{
    // --- this is used to determine if presets are supported
    //     which in this unit they are so we implement this method!
	if(outData == NULL) return noErr;

	// make the array
	CFMutableArrayRef theArray = CFArrayCreateMutable (NULL, kNumberPresets, NULL);

    // copy our preset names
	for (int i = 0; i < kNumberPresets; ++i)
    {
		CFArrayAppendValue (theArray, &kPresets[i]);
    }

    *outData = (CFArrayRef)theArray;
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
OSStatus RackAFX2AUFilter::NewFactoryPresetSet(const AUPreset & inNewFactoryPreset)
{
	SInt32 chosenPreset = inNewFactoryPreset.presetNumber;

    if (chosenPreset < 0 || chosenPreset >= kNumberPresets)
		return kAudioUnitErr_InvalidPropertyValue;

	m_nCurrentPreset = chosenPreset;

    int nParams = m_pRAFXPlugIn->m_bUseCustomVSTGUI ? m_pRAFXPlugIn->m_UIControlList.count() :
                                                      m_pRAFXPlugIn->m_UIControlList.countLegalVSTIF();

	for(int j=0; j<nParams; j++)
	{
		// my controls
		CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(j);

		double dPreset = pUICtrl->dPresetData[m_nCurrentPreset];

        Globals()->SetParameter(j, dPreset);

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
        m_pRAFXPlugIn->userInterfaceChange(pUICtrl->uControlId);

		// now do the Joystick!
		float* pJSProg = m_pRAFXPlugIn->m_PresetJSPrograms[m_nCurrentPreset];

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

		// call the interface function
		m_pRAFXPlugIn->userInterfaceChange(pUICtrl->uControlId);
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

#ifdef synthplugin
// --- Note On Event handler
OSStatus RackAFX2AUFilter::StartNote(MusicDeviceInstrumentID      inInstrument,
                                     MusicDeviceGroupID           inGroupID,
                                     NoteInstanceID*              outNoteInstanceID,
                                     UInt32                       inOffsetSampleFrame,
                                     const MusicDeviceNoteParams& inParams)
{
    m_pRAFXPlugIn->midiNoteOn((UINT)inGroupID, (UINT)inParams.mPitch, (UINT)inParams.mVelocity);
    return noErr;
}

// --- Note Off handler
OSStatus RackAFX2AUFilter::StopNote(MusicDeviceGroupID inGroupID,
                                    NoteInstanceID     inNoteInstanceID,
                                    UInt32             inOffsetSampleFrame)
{
    m_pRAFXPlugIn->midiNoteOff((UINT)inGroupID, (UINT)inNoteInstanceID, 0, false);
    return noErr;
}

// -- Pitch Bend handler
OSStatus RackAFX2AUFilter::HandlePitchWheel(UInt8  inChannel,
                                            UInt8  inPitch1,
                                            UInt8  inPitch2,
                                            UInt32 inStartFrame)
{
    // --- convert 14-bit concatentaion of inPitch1, inPitch2
    int nActualPitchBendValue = (int) ((inPitch1 & 0x7F) | ((inPitch2 & 0x7F) << 7));
    float fNormalizedPitchBendValue = (float) (nActualPitchBendValue - 0x2000) / (float) (0x2000);

    m_pRAFXPlugIn->midiPitchBend((UINT)inChannel, nActualPitchBendValue, fNormalizedPitchBendValue);
    return noErr;
}

/*
 NOTE: if using Logic, Volume and Pan will not be transmitted
 // --- NOTE: Logic hooks the Volume and Pan controls
 // --- But since MIDI CC 7 and 10 (volume and pan respectively) are reserved by the main channel strip controls,
 //     it's best to use MIDI CC 11 (expression) to automate volume effects
 //     http://www.soundonsound.com/sos/apr08/articles/logictech_0408.htm
 //
 On some plugins and instruments, CC#11 does nothing but control volume. On other plugins/instruments, CC#11 is programmed to control volume and timbre (brightness) simultaneously. This is a feature of the programming of the plugin or instrument and not an inherent quality of CC#11 data. In such a case, higher CC#11 values make a sound both louder and brighter, and vice versa. If in fact your instruments respond to CC#11 only with a change in volume then you might as well not try and fight city hall: use CC#11 as your volume control.
 */
// --- CC handler
OSStatus RackAFX2AUFilter::HandleControlChange(UInt8  inChannel,
                                               UInt8  inController,
                                               UInt8  inValue,
                                               UInt32 inStartFrame)
{
    // --- is mod wheel?
    if(inController == MOD_WHEEL)
        m_pRAFXPlugIn->midiModWheel((UINT)inChannel, (UINT)inValue);
    else if(inController == ALL_NOTES_OFF)
        m_pRAFXPlugIn->midiNoteOff(0, 0, 0, true);
    else if(m_pRAFXPlugIn->m_bWantAllMIDIMessages)
        m_pRAFXPlugIn->midiMessage((unsigned char)inChannel, CONTROL_CHANGE, (unsigned char)inController, (unsigned char)inValue);

    return noErr;
}

OSStatus RackAFX2AUFilter::HandleMidiEvent(UInt8  status,
                                           UInt8  channel,
                                           UInt8  data1,
                                           UInt8  data2,
                                           UInt32 inStartFrame)
{
    if(m_pRAFXPlugIn->m_bWantAllMIDIMessages)
        m_pRAFXPlugIn->midiMessage((unsigned char)channel, (unsigned char)status, (unsigned char)data1, (unsigned char)data2);

    // --- call base class to do its thing
    return AUMIDIBase::HandleMidiEvent(status, channel, data1, data2, inStartFrame);
}
#endif
} // namespace VSTGUI4







