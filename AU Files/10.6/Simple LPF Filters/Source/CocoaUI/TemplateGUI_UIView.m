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

#import "TemplateGUI_UIView.h"


enum
{
	// index values to link your controls to the GUI (from RackAFX)
	kfreqIndex,
	kqValueIndex,
	kfilterNumberIndex,
	kNumberOfParameters
};


#pragma mark ____ LISTENER CALLBACK DISPATCHER ____

// This listener responds to parameter changes, gestures, and property notifications
void EventListenerDispatcher (void *inRefCon, void *inObject, const AudioUnitEvent *inEvent, UInt64 inHostTime, Float32 inValue)
{
	TemplateGUI_UIView *SELF = (TemplateGUI_UIView *)inRefCon;
	[SELF priv_eventListener:inObject event: inEvent value: inValue];
}

@implementation TemplateGUI_UIView

-(void) awakeFromNib 
{
	// create the background
    // see NSColor for many ways to do this
    mBackgroundColor = [NSColor gridColor];
}

#pragma mark ____ (INIT /) DEALLOC ____
- (void)dealloc 
{
    [self priv_removeListeners];
	
    [mBackgroundColor release];
		
	[[NSNotificationCenter defaultCenter] removeObserver: self];
	
    [super dealloc];
}

#pragma mark ____ PUBLIC FUNCTIONS ____
- (void)setAU:(AudioUnit)inAU 
{
	// remove previous listeners
	if (mAU) 
		[self priv_removeListeners];
	
	mAU = inAU;
    
	// add new listeners
	[self priv_addListeners];
	
	// initial setup
	[self priv_synchronizeUIWithParameterValues];
}

- (void)drawRect:(NSRect)rect
{
    // just fill the background
	[mBackgroundColor set];
	NSRectFill(rect);		// this call is much faster than using NSBezierPath, but it doesn't handle non-opaque colors
	
    // other custom drawing here...
    
	[super drawRect: rect];	// we call super to draw all other controls after we have filled the background
}


#pragma mark ____ INTERFACE ACTIONS ____
//
// Message handlers for control changes
- (IBAction)sliderEdit1Changed:(id)sender 
{
    // get the value from the control
    float floatValue = [sender floatValue];
    
    // TODO: connect to plugin
    /*
    // make an AudioUnitParameter set in our AU buddy
	AudioUnitParameter volLeft = {mAU, kVolume_dBLeft, kAudioUnitScope_Global, 0 };
	
    // set the AU Parameter; this calls SetParameter() in the au
	AUParameterSet(mAUEventListener, sender, &volLeft, (Float32)floatValue, 0);
    
    // update the slider
    [slider1 setFloatValue: floatValue];	
     */
}

- (IBAction)SliderEdit2Changed:(id)sender 
{
    // get the value from the control 
    float floatValue = [sender floatValue];
 
    
    // TODO: connect to plugin
    /*
    // make an AudioUnitParameter set in our AU buddy 
	AudioUnitParameter volRight = {mAU, kVolume_dBRight, kAudioUnitScope_Global, 0 };
   
    // set the AU Parameter; this calls SetParameter() in the au
	AUParameterSet(mAUEventListener, sender, &volRight, (Float32)floatValue, 0);
    
    // update the slider 
    [slider2 setFloatValue: floatValue];	    
     */
}

- (IBAction) slider1Changed:(id)sender
{
    // get the value from the control  
 	float floatValue = [sender floatValue];
    
    // TODO: connect to plugin
    /*
    // make an AudioUnitParameter set in our AU buddy    
	AudioUnitParameter volLeft = {mAU, kVolume_dBLeft, kAudioUnitScope_Global, 0 };
	
    // set the AU Parameter; this calls SetParameter() in the au 
	AUParameterSet(mAUEventListener, sender, &volLeft, (Float32)floatValue, 0);
  
    // update the text field 
    [slider1EditBox setFloatValue: floatValue];	
     */

}
- (IBAction) slider2Changed:(id)sender
{
    // get the value from the control   
    float floatValue = [sender floatValue];
    
    // TODO: connect to plugin
    /*
    // make an AudioUnitParameter set in our AU buddy       
	AudioUnitParameter volRight = {mAU, kVolume_dBRight, kAudioUnitScope_Global, 0 };
    
    // set the AU Parameter; this calls SetParameter() in the au 
	AUParameterSet(mAUEventListener, sender, &volRight, (Float32)floatValue, 0);

    // update the text field 
    [slider2EditBox setFloatValue: floatValue];	// update the frequency text field    
     */
}

- (IBAction)muteButtonClicked:(id)sender 
{
    // get the value from the control   
    int intValue = [sender intValue];
    
    // TODO: connect to plugin
    /*
    // 0 = MUTE_OFF
    // 1 = MUTE_ON
    // make an AudioUnitParameter set in our AU buddy       
    AudioUnitParameter mute = {mAU, kMute, kAudioUnitScope_Global, 0 };
    
    // update the text field  
	AUParameterSet(mAUEventListener, sender, &mute, (int)intValue, 0);
     */
    
    // could also force the sliders to -96dB as an exercise
}


void addParamListener (AUEventListenerRef listener, void* refCon, AudioUnitEvent *inEvent)
{
    // uncomment if you want to receive/track mouse clicks and movements
    /*
	inEvent->mEventType = kAudioUnitEvent_BeginParameterChangeGesture;
	verify_noerr ( AUEventListenerAddEventType(	listener, refCon, inEvent));
	
	inEvent->mEventType = kAudioUnitEvent_EndParameterChangeGesture;
	verify_noerr ( AUEventListenerAddEventType(	listener, refCon, inEvent));
	*/
    
    // setup the event for when a preset is loaded so our controls can move
	inEvent->mEventType = kAudioUnitEvent_ParameterValueChange;
	verify_noerr ( AUEventListenerAddEventType(	listener, refCon, inEvent));	
}

#pragma mark ____ PRIVATE FUNCTIONS ____
- (void)priv_addListeners 
{
	if (mAU) 
    {
		verify_noerr( AUEventListenerCreate(EventListenerDispatcher, self,
											CFRunLoopGetCurrent(), kCFRunLoopDefaultMode, 0.05, 0.05, 
											&mAUEventListener));      
        
        
        // TODO: connect to plugin
        /*
        // start with first control
 		AudioUnitEvent auEvent;
		AudioUnitParameter parameter = {mAU, kVolume_dBLeft, kAudioUnitScope_Global, 0 };
		
        // set param & add it
        auEvent.mArgument.mParameter = parameter;		
		addParamListener (mAUEventListener, self, &auEvent);
		
        // notice the way additional params are added using paramIDs
		auEvent.mArgument.mParameter.mParameterID = kVolume_dBRight;
		addParamListener (mAUEventListener, self, &auEvent);
       
        // add more here
        auEvent.mArgument.mParameter.mParameterID = kMute;
		addParamListener (mAUEventListener, self, &auEvent);
         */

	}
}

- (void)priv_removeListeners 
{
	if (mAUEventListener) verify_noerr (AUListenerDispose(mAUEventListener));
	mAUEventListener = NULL;
	mAU = NULL;
}

- (void)priv_synchronizeUIWithParameterValues 
{
    
    // TODO: connect to plugin
    /*
    // get the parameter values
  	Float32 left, right, mute;
    
    // make an AudioUnitParameter get from our AU buddy          
	AudioUnitParameter parameter = {mAU, kVolume_dBLeft, kAudioUnitScope_Global, 0 };
	
    // get the AU Parameter[kVolume_dBLeft] = left vol
	AudioUnitGetParameter(mAU, kVolume_dBLeft, kAudioUnitScope_Global, 0, &left);
    
    // update the controls
    [slider1EditBox setFloatValue: left];	// update the frequency text field
    [slider1 setFloatValue:left];

    // change the parameterID for subsequent controls
	parameter.mParameterID = kVolume_dBRight;
  
    // get the AU Parameter[kVolume_dBRight] = right vol
	AudioUnitGetParameter(mAU, kVolume_dBRight, kAudioUnitScope_Global, 0, &right);
	
    // update the controls
    [slider2EditBox setFloatValue: right];	// update the frequency text field 
    [slider2 setFloatValue:right];
    
    // change the parameterID for subsequent controls
	parameter.mParameterID = kMute;
    
    // get the AU Parameter[kMute] = mute state; note its always a Float32
	AudioUnitGetParameter(mAU, kMute, kAudioUnitScope_Global, 0, &mute);
	
    // update the control
    [muteButton setIntValue:mute];
     */
    
}

#pragma mark ____ LISTENER CALLBACK DISPATCHEE ____
//
// PRESETS:
//
// Handle kAudioUnitProperty_PresentPreset event
- (void)priv_eventListener:(void *) inObject event:(const AudioUnitEvent *)inEvent value:(Float32)inValue 
{
	switch (inEvent->mEventType) 
    {
		case kAudioUnitEvent_ParameterValueChange:	
        {
            
            // TODO: connect to plugin
            /*
            switch (inEvent->mArgument.mParameter.mParameterID) 
            {
                // set in controls
				case kVolume_dBLeft:                            
					[slider1EditBox setFloatValue: inValue];	// update the text field
                    [slider1 setFloatValue:inValue];            // move the slider
					break;
				
                case kVolume_dBRight:                           
                    [slider2EditBox setFloatValue: inValue];	// update the text field
					[slider2 setFloatValue:inValue];			// move the slider		
                    break;					
				
                case kMute:                           
                    [muteButton setIntValue: (int)inValue];     // update the button; note casting
                   break;					
              
                default:
                    break;
                */
                    
                // for mouse movements and clicks
                /*
                 case kAudioUnitEvent_BeginParameterChangeGesture:			// Begin gesture
                    // do something here...
                    break;
                 case kAudioUnitEvent_EndParameterChangeGesture:				// End gesture
                    // do something here...
                    break;

            }
            */ 
        }
	}
}

/* If we get a mouseDown, that means it was not in the graph view, or one of the text fields. 
   In this case, we should make the window the first responder. This will deselect our text fields if they are active. */
- (void) mouseDown: (NSEvent *) theEvent {
	[super mouseDown: theEvent];
	[[self window] makeFirstResponder: self];
}

- (BOOL) acceptsFirstResponder {
	return YES;
}

- (BOOL) becomeFirstResponder {	
	return YES;
}

- (BOOL) isOpaque {
	return YES;
}

@end

