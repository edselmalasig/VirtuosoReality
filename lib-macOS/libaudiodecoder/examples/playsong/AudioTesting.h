//
//  AudioTesting.h
//  AudioTesting
//
//  Created by Edsel Malasig on 1/13/17.
//  Copyright © 2017 xdlogic. All rights reserved.
//

#ifndef AudioTesting_h
#define AudioTesting_h
#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <audiodecoder.h> // libaudiodecoder
#include <portaudio.h>                 // PortAudio

class PlaySong{
public:

    PlaySong();
    
    int play (int argc, const char ** argv);
    static AudioDecoder * mAudio;
    // All audio will be handled as stereo.
    static const int NUM_CHANNELS = 2;
    
    // Declaration for audio callback called by PortAudio.
    static int audioCallback(const void *input, void *output,
                      unsigned long frameCount,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void* userData);
    
    
    
private:
protected:
};

#endif /* AudioTesting_h */
