/*----------------------------------------------------------------------------
 Virtuoso Reality - Virtual Reality real-time audio visualization tool
 
 Copyright (c) 2016 Edsel Malasig
 http://virtuosoreality.xdlogic.com
 
 Backend - Virtuoso Reality : MARSYAS : PORTAUDIO : libaudiodecoder
 Copyright (c) 2004 Ge Wang, Perry R. Cook, Ananya Misra.
 All rights reserved.
 http://www.gewang.com/
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 U.S.A.
 -----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// name: Virtuoso_Reality.cpp
// desc: small real-time spectrum visualizer, originally implemented
//       by Ge Wang for the memex project and to entertain small children
//       at parties.
//
// authors:
//          Edsel Malasig (http://www.xdlogic.com)
//			Ge Wang (http://www.gewang.com/)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// thanks to:
//          Yilei Shao, Qin Lv, Tom Briggs
// library:
//          (PortAudio) www.portaudio.com
//          (FFT) CARL CMusic Distribution
//          (Marsyas) George Tzanetakis (gtzan@cs.princeton.edu)
//          (STK) Perry R. Cook (prc@cs.princeton.edu)
//          (libsndfile) Erik de Castro Lopo
//          (ImGUI) Ocornut
// date: 11.28.2003 - initial version
//       spring 2015 - updated for OSX Yosemite
//ƒ
// usage: (type 'h' while running or see list of command line arguments)
//-----------------------------------------------------------------------------


//#include "imguifilesystem.h"

#include <stdio.h>
#include <fstream>
#include "Virtuoso_Reality.h"
#include "Windowing_System.h"
#include "Virtuoso_Reality_Util.h"
#include "Camera.h"

GLfloat VirtuosoReality::g_wf_delay_ratio = 1.0f / 3.0f;
GLuint VirtuosoReality::g_depth = 48;
GLint VirtuosoReality::g_buffer_size = SND_BUFFER_SIZE;
GLboolean VirtuosoReality::g_draw_features = TRUE;
int VirtuosoReality::b_seek = 0;
int VirtuosoReality::g_position = 0;
GLboolean VirtuosoReality::g_pause = false;

GLint VirtuosoReality::g_buffer_count_a = 0;
GLint VirtuosoReality::g_buffer_count_b = 0;
GLboolean VirtuosoReality::g_running = false;
GLboolean VirtuosoReality::g_mute = FALSE;

std::string VirtuosoReality::g_filename;
GLint VirtuosoReality::g_starting = 0;
GLint VirtuosoReality::g_wf = 0;
GLboolean VirtuosoReality::g_freeze = false;

GLboolean VirtuosoReality::g_restart = false;
GLboolean * VirtuosoReality::g_draw;
GLboolean VirtuosoReality::g_ready;

VirtuosoReality::Pt2D ** VirtuosoReality::g_spectrums;// = NULL;
SAMPLE ** VirtuosoReality::g_waveforms;
GLuint VirtuosoReality::g_wf_index = 0;
SAMPLE VirtuosoReality::g_audio_buffer[SND_BUFFER_SIZE];
SAMPLE VirtuosoReality::g_stereo_buffer[SND_BUFFER_SIZE*2];

AudioDecoder * VirtuosoReality::g_sf;
Mutex VirtuosoReality::g_mutex;

const int VirtuosoReality::NUM_CHANNELS;
WindowingSystem * VirtuosoReality::ws;

GLfloat VirtuosoReality::g_light0_pos[4] = { 2.0f, 1.2f, 4.0f, 1.0f };
GLfloat VirtuosoReality::g_light1_pos[4] = { -2.0f, 0.0f, -4.0f, 1.0f };

GLfloat VirtuosoReality::g_dz = .1f;
GLfloat VirtuosoReality::g_z = 0.0f;
GLfloat VirtuosoReality::g_space = .12f; // for john: .1f
GLboolean VirtuosoReality::g_waveform = TRUE;
GLboolean VirtuosoReality::g_wutrfall = TRUE;
GLboolean VirtuosoReality::g_usedb = false;
GLfloat VirtuosoReality::g_time_scale = 1.0f;
GLfloat VirtuosoReality::g_freq_scale = 1.0f;
GLint VirtuosoReality::g_time_view = 1;
GLfloat VirtuosoReality::g_inc_val_kb = INC_VAL_KB;

GLint VirtuosoReality::g_ds;
GLfloat VirtuosoReality::g_inc_val_mouse = INC_VAL_MOUSE;
GLboolean VirtuosoReality::g_fullscreen = FALSE;
GLboolean VirtuosoReality::g_lissajous = TRUE;
GLfloat VirtuosoReality::g_lissajous_scale = 0.45f;
GLint VirtuosoReality::g_delay = SND_BUFFER_SIZE/2;
GLfloat VirtuosoReality::g_log_factor = 1;
GLfloat VirtuosoReality::g_log_space = 0;
GLint VirtuosoReality::g_fft_size = SND_FFT_SIZE;
GLboolean VirtuosoReality::g_rainbow = TRUE;
GLboolean VirtuosoReality::g_show_time = FALSE;
GLboolean VirtuosoReality::g_backwards = FALSE;
GLfloat VirtuosoReality::g_begintime = 1;
GLint VirtuosoReality::g_wf_delay = (GLint)(g_depth * VirtuosoReality::g_wf_delay_ratio + .5f);
GLint VirtuosoReality::g_freq_view = 2;
GLfloat VirtuosoReality::g_log_positions[SND_FFT_SIZE/2];
bool VirtuosoReality::showui = true;
bool VirtuosoReality::g_play = false;
float VirtuosoReality::t = 0;
int VirtuosoReality::samplesRead = 0;
GLboolean VirtuosoReality::g_file_running = false;
int VirtuosoReality::out_numframes = 0;

VirtuosoReality::VirtuosoReality(int argc, char ** argv){
    ws = new WindowingSystem();
    
    g_wf_delay = (GLuint)(g_depth * g_wf_delay_ratio + .5f);
    g_depth = 48;
    g_buffer_size = SND_BUFFER_SIZE;
    g_draw_features = TRUE;
    b_seek = 0;
    g_position = 0;
    g_pause = false;
    
    g_buffer_count_a = 0;
    g_buffer_count_b = 0;
    g_running = TRUE;
    g_mute = FALSE;
    
#ifdef __OS_MACOSX__
    //save working dir
    char * cwd = getcwd( NULL, 0 );
    //restore working dir
    chdir( cwd );
    //free( cwd );
#endif
    
    this->init(argc, argv);
}

//-----------------------------------------------------------------------------
// name: cb()
// desc: audio callback
//-----------------------------------------------------------------------------
//int cb( char * buffer, int buffer_size, void * user_data )

int VirtuosoReality::cb( const void * inputBuffer, void * outputBuffer,  unsigned long numFrames,
                        const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags, void * userData )
{
    g_sf = static_cast<AudioDecoder *>(userData);
    // Play it safe when debugging and coding, protect your ears by clearing
    // the output buffer.
    memset(outputBuffer, 0, numFrames * NUM_CHANNELS * sizeof(float));
    
    int temp;
    out_numframes = numFrames;
    if (b_seek){
        temp = g_position + g_sf->positionInSamples()+1;
        g_sf->seek(temp);
        samplesRead = temp;
    }
    else if (g_position < 0){
        g_position = 0;
        temp = g_position;
        g_sf->seek(temp);
        samplesRead = temp;
    }
    else{
        temp = g_sf->positionInSamples();
        g_sf->seek(temp);
    }
    
    // Decode the number of samples that PortAudio said it needs to send to the
    // soundcard.
    
    
    g_mutex.lock();
    
    // freeze frame
    if( g_freeze ) {
        memset( outputBuffer, 0, numFrames * NUM_CHANNELS * sizeof(SAMPLE) );
        g_ready = TRUE;
        return 0;
    }
    
    // check for restart
    if( g_restart )
    {
        
        // set playback position to begin
        g_sf->seek( 0 );
        g_wf_index = 0;
        g_wf = 0;
        g_starting = 1;
        g_restart = FALSE;
        // clear waveforms and waterfall and drawing booleans
        for( GLint i = 0; i < g_wf_delay || i < g_depth; i++ )
        {
            if( i < g_wf_delay )
                memset( g_waveforms[i], 0, g_buffer_size * 2 * sizeof(SAMPLE) );
            if( i < g_depth )
            {
                memset( g_spectrums[i], 0, sizeof(Pt2D)*SND_FFT_SIZE );
                g_draw[i] = false;
            }
        }
        
    }
    
    samplesRead += g_sf->read(numFrames * NUM_CHANNELS,
                                 static_cast<SAMPLE*>(g_stereo_buffer));
    
    if( g_sf->channels() == 2 )
    {
        // convert stereo to mono
        for( int i = 0; i < numFrames; i++)
        {
            g_audio_buffer[i] = g_stereo_buffer[i*2] + g_stereo_buffer[i*2+1];
            g_audio_buffer[i] /= 2.0f;
        }
    }
    else
    {
        // actually mono
        memcpy( g_audio_buffer, g_stereo_buffer, numFrames * sizeof(SAMPLE) );
        // convert mono to stereo
        for( int i = 0; i < numFrames; i++ )
        {
            g_stereo_buffer[i*2] = g_stereo_buffer[i*2+1] = g_audio_buffer[i];
        }
    }
    
    // time-domain waterfall delay
    if( g_waveforms != NULL )
    {
        // put current buffer in time-domain waterfall
        memcpy( g_waveforms[g_wf_index], g_stereo_buffer, numFrames * 2 * sizeof(SAMPLE) );
        // incrment index (this is also the index to copy out of)
        g_wf_index = (GLint)((g_wf_index + 1.0f)) % (GLint)(g_wf_delay);
        // copy delayed buffer out of time-domain waterfall
        memcpy( g_stereo_buffer, g_waveforms[g_wf_index], numFrames * 2 * sizeof(SAMPLE) );
    }
    
    // play stereo
    memcpy( outputBuffer, g_stereo_buffer, numFrames * 2 * sizeof(SAMPLE) );
    
    // count
    g_buffer_count_a++;
    
    // done...
    
    if( samplesRead > g_sf->numSamples() ){
        // done...
        g_running = FALSE;
        g_play = FALSE;
        g_filename = "";
        g_file_running = FALSE;
        samplesRead = 0;
        g_sf = NULL;
        
        // zero
        memset( g_audio_buffer, 0, numFrames * sizeof(SAMPLE) );
        // copy remaining delayed waveform buffers one by one
        if( g_wf_delay )
        {
            memset( g_waveforms[g_wf_index], 0, numFrames * 2 * sizeof(SAMPLE) );
            g_wf_index = (GLint)((g_wf_index + 1.0f)) % (GLint)(g_wf_delay);
            memcpy( g_stereo_buffer, g_waveforms[g_wf_index], numFrames * 2 * sizeof(SAMPLE) );
            memcpy( outputBuffer, g_stereo_buffer, numFrames * 2 * sizeof(SAMPLE) );
        }
        else
            memset( outputBuffer, 0, 2 * numFrames * sizeof(SAMPLE) );
        
        if( Pa_StopStream( g_sf ) != paNoError ){
            std::cerr << "Failed to stop stream." << std::endl;
            
        }
        if (Pa_CloseStream(g_sf) != paNoError){
            std::cerr << "Failed to close stream." << std::endl;
        }
        if(Pa_Terminate() != paNoError){
            std::cerr << "Failed to terminate stream." << std::endl;
        }else
            std::cerr << "Stream terminated." << std::endl;
        
        b_seek = false;
        g_ready = TRUE;
        g_mutex.unlock();
        delete g_sf;
        std::cout << "*************** paComplete: " << paComplete << std::endl;
        return 3;
    }
    
    
    b_seek = false;
    // set flag
    g_ready = TRUE;
    // unlock
    g_mutex.unlock();
    /*
     // mute the real-time audio
     if( g_mute )
     memset( outputBuffer, 0, numFrames * 2 * sizeof(SAMPLE) );
     */
    return paContinue;
    
}


void VirtuosoReality::init(int argc, char ** argv){
    
    this->help();
    this->probe();
    GLboolean set_play = TRUE;
    // command line arguments
    for( int i = 1; i < argc; i++ )
    {
        if( !strncmp( argv[i], "-", 1 ) )
        {
            if( !strcmp( argv[i], "--print" ) )
                g_stdout = TRUE;
            else if( !strcmp( argv[i], "--help" ) || !strcmp( argv[i], "--about" ) )
            {
                usage();
                exit( 0 );
            }
            else if( !strncmp( argv[i], "--inputDevice:", 14) )
                g_audioInputDevice = atoi(argv[i]+14) >= 0 ? atoi(argv[i]+14) : g_audioInputDevice;
            else if( !strncmp( argv[i], "--outputDevice:", 15) )
                g_audioOutputDevice = atoi(argv[i]+15) >= 0 ? atoi(argv[i]+15) : g_audioOutputDevice;
            else if( !strcmp( argv[i], "--sndout" ) )
                g_sndout = 2;
            else if( !strcmp( argv[i], "--nodisplay" ) )
                g_display = FALSE;
            else if( !strcmp(argv[i], "--fullscreen") || !strcmp(argv[i], "--fullscreen:ON") )
                g_fullscreen = TRUE;
            else if( !strcmp(argv[i], "--fullscreen:OFF") )
                g_fullscreen = FALSE;
            else if( !strcmp(argv[i], "--waveform") || !strcmp(argv[i], "--waveform:ON") )
                g_waveform = TRUE;
            else if( !strcmp(argv[i], "--waveform:OFF") )
                g_waveform = FALSE;
            else if( !strcmp(argv[i], "--lissajous") || !strcmp(argv[i], "--lissajous:ON") )
                g_lissajous = TRUE;
            else if( !strcmp(argv[i], "--lissajous:OFF") )
                g_lissajous = FALSE;
            else if( !strcmp(argv[i], "--waterfall") || !strcmp(argv[i], "--waterfall:ON") )
                g_wutrfall = TRUE;
            else if( !strcmp(argv[i], "--waterfall:OFF") )
                g_wutrfall = FALSE;
            else if( !strcmp(argv[i], "--dB") || !strcmp(argv[i], "--dB:ON") )
                g_usedb = TRUE;
            else if( !strcmp(argv[i], "--dB:OFF") )
                g_usedb = FALSE;
            else if( !strcmp(argv[i], "--features") || !strcmp(argv[i], "--features:ON") )
                g_draw_features = TRUE;
            else if( !strcmp(argv[i], "--features:OFF") )
                g_draw_features = FALSE;
            else if( !strcmp(argv[i], "--fallcolors") || !strcmp(argv[i], "--fallcolors:ON") )
                g_rainbow = TRUE;
            else if( !strcmp(argv[i], "--fallcolors:OFF") )
                g_rainbow = FALSE;
            else if( !strcmp(argv[i], "--backward") || !strcmp(argv[i], "--backward:ON") )
                g_backwards = TRUE;
            else if( !strcmp(argv[i], "--backward:OFF") )
                g_backwards = FALSE;
            else if( !strcmp(argv[i], "--showtime") || !strcmp(argv[i], "--showtime:ON") )
                g_show_time = TRUE;
            else if( !strcmp(argv[i], "--showtime:OFF") )
                g_show_time = FALSE;
            else if( !strcmp(argv[i], "--freeze") || !strcmp(argv[i], "--freeze:ON") )
                g_freeze = g_pause = TRUE;
            else if( !strcmp(argv[i], "--freeze:OFF") )
                g_freeze = g_pause = FALSE;
            else if( !strcmp(argv[i], "--drawplay") || !strcmp(argv[i], "--drawplay:ON") )
                g_draw_play = set_play = TRUE;
            else if( !strcmp(argv[i], "--drawplay:OFF") )
            {   g_show_time = FALSE; set_play = TRUE;   }
            else if( !strncmp(argv[i], "--srate:", 8) )
                g_srate = atoi( argv[i]+8 ) > 0 ? atoi( argv[i]+8 ) : g_srate;
            else if( !strncmp(argv[i], "--timescale:", 12) )
                g_time_scale = atof( argv[i]+12 );
            else if( !strncmp(argv[i], "--freqscale:", 12) )
                g_freq_scale = atof( argv[i]+12 );
            else if( !strncmp(argv[i], "--lissscale:", 12) )
                g_lissajous_scale = atof( argv[i]+12 );
            else if( !strncmp(argv[i], "--logfactor:", 12) )
                g_log_factor = atof( argv[i]+12 ) > 0 ? atof( argv[i]+12 ) : g_log_factor;
            else if( !strncmp(argv[i], "--spacing:", 10) )
                g_space = atof( argv[i]+10 ) > 0 ? atof( argv[i]+10 ) : g_space;
            else if( !strncmp(argv[i], "--rotatem:", 10) )
                g_inc_val_mouse = atof( argv[i]+10 );
            else if( !strncmp(argv[i], "--rotatek:", 10) )
                g_inc_val_kb = atof( argv[i]+10 ) / (INC_VAL_MOUSE/INC_VAL_KB);
            else if( !strncmp(argv[i], "--zpos:", 7 ) )
            {
                g_z = atof( argv[i]+7);
                g_z_set = TRUE;
            }
            else if( !strncmp(argv[i], "--dzpos:", 8 ) )
                g_dz = atof( argv[i]+8 );
            else if( !strncmp(argv[i], "--depth:", 8) )
            {
                g_depth = atoi( argv[i]+8 ) > 0 ? atoi( argv[i]+8 ) : g_depth;
            }
            else if( !strncmp(argv[i], "--preview:", 10) )
            {
                g_wf_delay_ratio = atof( argv[i]+10 );
                if( g_wf_delay_ratio < 0 || g_wf_delay_ratio >= 1 )
                {
                    fprintf( stderr, "[Virtuoso Reality]: --preview requires 0 <= value < 1...\n" );
                    usage();
                }
            }
            else if( !strncmp(argv[i], "--yview:", 8) )
                ws->g_eye_y = atof( argv[i] + 8 );
            else if( !strncmp( argv[i], "--begintime:", 12 ) )
                g_begintime = atof( argv[i]+12 ) >= 0 ? atof( argv[i]+12 ) : g_begintime;
            else if( !strncmp( argv[i], "--ds:", 5 ) )
            {
                g_ds = atoi( argv[i] + 5 ) >= 0 ? atoi( argv[i] + 5 ) : 0;
                g_downsample = (g_ds > 0);
            }
            else
            {
                fprintf( stderr, "[Virtuoso Reality]: unrecognized option '%s'...\n", argv[i] );
                usage();
            }

            //g_filename = string(argv[1]);
            g_filename = argv[1];
            g_file_running = false;
        }
        else
        {
            if( g_filename.empty() )
            {
                fprintf( stderr, "[Virtuoso Reality]: multiple filenames specified...\n" );
                usage();
            }
            
            // reading from file...
            //g_filename = string(argv[1]);
            
            g_filename = argv[1];

            g_file_running = false;
            
            g_sndout = 2;
            g_starting = 1;
        }
    }
    glutInit( &argc, argv );
    ImGui_ImplGlfw_Init(ws->window, true);
    glfwMakeContextCurrent(ws->window);
    glfwSetCursorPosCallback( ws->window, (GLFWcursorposfun)mouseFunc );
    
    glfwSetKeyCallback(ws->window, (GLFWkeyfun)keyboardFunc);
    glfwSwapInterval(1);
    
    ImGui::SetupImGuiStyle(false, 0.1f);
    this->initialize_analysis();
    this->initialize_graphics();
    this->run();
}

void VirtuosoReality::run(){
    std::cout << "run" << std::endl;
    this->initialize_audio();

    while(g_running){
        this->avrLoop();
    }
}

void VirtuosoReality::avrLoop(){
 
    int display_w;
    int display_h;
    int show_display = false;
    int audio_play_done = 0;
    int count;
    while( !glfwWindowShouldClose(ws->window) )
    {
        // Set frame time
        
        GLfloat currentFrame = (GLfloat) glfwGetTime();
        ws->deltaTime = currentFrame - ws->lastFrame;
        ws->lastFrame = currentFrame;
        
        glfwGetFramebufferSize(ws->window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glfwPollEvents();
        //this->displayFunc();
        if(showui){
            this->showUI();
            ImGui::Render();
            /*
            if(g_filename.empty()){
                glfwPollEvents();
                glfwSwapBuffers(ws->window);
                continue;
            }
            */
        }
        //if(!g_filename.empty()){// && !g_file_running)
            //g_file_running = true;
            //this->initialize_graphics();
            
            
        //}
        /*
        if(!g_filename.empty() && !g_file_running){
            audio_play_done = initialize_audio();
            //g_filename = new char[MAX_PATH_BYTES];
            show_display = true;
         g_running = FALSE;
         g_play = FALSE;
         g_filename = "";
         g_file_running = FALSE;
         samplesRead = 0;
         g_sf = NULL;
        }*/
        
        if(!g_filename.empty() && g_file_running){
            this->displayFunc();
            show_display = g_running;
        }
        glfwPollEvents();
        glfwSwapBuffers(ws->window);
    }
}

void VirtuosoReality::showUI(){
    ImGui_ImplGlfw_NewFrame();
    
    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    {
        ImGui::Begin("Virtuoso Reality");
        static float f = 0.0f;
        ImGui::Text("Virtuoso Reality - from sndpeek");
        if (ImGui::Button("Demo Window")) show_demo_window ^= 1;
        if (ImGui::Button("File Chooser Window")){ show_fchooser_window ^= 1;};
        if (ImGui::Button("Help Window")) show_help_window ^= 1;
        if (ImGui::Button("About")) show_about_window ^= 1;
        //if (ImGui::Button("Status Window")) show_stat_window ^= 1;
        ImGui::End();
    }
    
    // 2. Show another simple window, this time using an explicit Begin/End pair
    if (show_fchooser_window)
    {
        static ImGuiFs::Dialog dlg;
        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("File Chooser", &show_fchooser_window);

        browseButtonPressed = ImGui::Button("Choose [a] file[s] ...");  // we need a trigger boolean variable
                                     // one per dialog (and must be static)
        
        dlg.chooseFileDialog(browseButtonPressed);              // see other dialog types and the full list of arguments for advanced usage
        
        if (browseButtonPressed) {
            ImGui::Text("Chosen file: \"%s\"",dlg.getChosenPath());
        }
        
        // If you want to copy the (valid) returned path somewhere, you can use something like:
        //static char myPath[ImGuiFs::MAX_PATH_BYTES];
        if (strlen(dlg.getChosenPath())>0 && strlen(musicPath) == 0) {
            strcpy(musicPath,dlg.getChosenPath());
            ImGui::Text("Chosen file: \"%s\"",dlg.getChosenPath());
            
            string str(musicPath);
            g_filename = str;
            std::cout << musicPath << " : " << g_filename << std::endl;
            browseButtonPressed = !browseButtonPressed;
            show_fchooser_window = !show_fchooser_window;
            
        }
        
        // If you want to copy the (valid) returned path somewhere, you can use something like:
        /*
        char myPath[ImGuiFs::MAX_PATH_BYTES];
        if (strlen(dlg.getChosenPath())>0) {
            strcpy(g_filename,dlg.getChosenPath());
            
            std::cerr << g_filename << std::endl;
        }
         */
        ImGui::End();
    }
    
    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
    if (show_demo_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow(&show_demo_window);
    }
    
    if(show_help_window){
        ImGui::Begin("Help", &show_help_window);
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::Text("Virtuoso Reality - from sndpeek");
        ImGui::Text("----------------------------------------------------" );
        ImGui::Text("Virtuoso Reality + wutrfall (1.4)" );
        ImGui::Text("Edsel Malasig, Ge Wang, Perry R. Cook, Ananya Misra" );
        ImGui::Text("http://www.xdlogic.com/" );
        ImGui::Text("----------------------------------------------------" );
        ImGui::Text("'SPACE' - show ui");
        ImGui::Text("'h' - print this help message" );
        ImGui::Text("'p' - print current settings to console" );
        ImGui::Text("'f' - toggle fullscreen" );
        ImGui::Text("'F2' - freeze frame! (can still rotate/scale)" );
        ImGui::Text("'1' - toggle waveform display" );
        ImGui::Text("'q' - toggle lissajous display" );
        ImGui::Text("'3' - toggle wutrfall display" );
        ImGui::Text("'4' - toggle feature extraction (broken)" );
        ImGui::Text("'d' - toggle dB plot for spectrum" );
        ImGui::Text("'r' - toggle rainbow waterfall" );
        ImGui::Text("'b' - toggle waterfall moving backwards/forwards" );
        ImGui::Text("'x' - restart file playback (if applicable)" );
        ImGui::Text("'n' - print current settings to terminal" );
        ImGui::Text("'m' - mute" );
        ImGui::Text("'j' - move spectrum + z" );
        ImGui::Text("'k' - move spectrum - z" );
        ImGui::Text("'u' - spacing more!" );
        ImGui::Text("'z' - spacing less!" );
        ImGui::Text("L, R mouse button - rotate left or right" );
        ImGui::Text("'[', ']' - rotate up or down" );
        ImGui::Text("'-' - scale DOWN the spectrum" );
        ImGui::Text("'F1' - scale UP the spectrum" );
        ImGui::Text("'`' - scale DOWN the waveform" );
        ImGui::Text("'=' - scale UP the waveform" );
        ImGui::Text("'F4' = LOWER log factor of spectrum display" );
        ImGui::Text("'c' = RAISE log factor of spectrum display" );
        ImGui::Text("      (log factor of 1.0 is linear display" );
        ImGui::Text("'v' - LOWER amount of viewable waveform" );
        ImGui::Text("'F3' - RAISE amount of viewable waveform" );
        ImGui::Text("';' - scale DOWN the lissajous!" );
        ImGui::Text("'l' - scale UP the lissajous!" );
        ImGui::Text("'y' - decrease delay for lissajous plot" );
        ImGui::Text("'Y' - increase delay for lissajous plot" );
        ImGui::Text("'t' - show time" );
        ImGui::Text("'ESC' - quit" );
        ImGui::Text("----------------------------------------------------" );
        ImGui::Text("" );
        ImGui::End();
    }
    
    if(show_about_window){
        ImGui::Begin("About", &show_about_window);
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::Text("/*----------------------------------------------------------------------------");
        ImGui::Text(" Virtuoso Reality - Virtual Reality real-time audio visualization tool");
        ImGui::Text(" ");
        ImGui::Text(" Copyright (c) 2016 Edsel Malasig");
        ImGui::Text(" http:ImGui::Text(virtuosoreality.xdlogic.com");
        ImGui::Text(" ");
        ImGui::Text(" Backend - Virtuoso Reality : MARSYAS : PORTAUDIO : libaudiodecoder");
        ImGui::Text(" Copyright (c) 2004 Ge Wang, Perry R. Cook, Ananya Misra.");
        ImGui::Text(" All rights reserved.");
        ImGui::Text(" www.gewang.com");
        ImGui::Text(" ");
        ImGui::Text(" This program is free software; you can redistribute it and/or modify");
        ImGui::Text(" it under the terms of the GNU General Public License as published by");
        ImGui::Text(" the Free Software Foundation; either version 2 of the License, or");
        ImGui::Text(" (at your option) any later version.");
        ImGui::Text(" ");
        ImGui::Text(" This program is distributed in the hope that it will be useful,");
        ImGui::Text(" but WITHOUT ANY WARRANTY; without even the implied warranty of");
        ImGui::Text(" MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the");
        ImGui::Text(" GNU General Public License for more details.");
        ImGui::Text(" ");
        ImGui::Text(" You should have received a copy of the GNU General Public License");
        ImGui::Text(" along with this program; if not, write to the Free Software");
        ImGui::Text(" Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307");
        ImGui::Text(" U.S.A.");
    ImGui::Text("-----------------------------------------------------------------------------*/");
        
    ImGui::Text("-----------------------------------------------------------------------------");
        ImGui::Text(" name: Virtuoso_Reality");
        ImGui::Text(" desc: small real-time spectrum visualizer, originally implemented");
        ImGui::Text("       by Ge Wang for the memex project and to entertain small children");
        ImGui::Text("       at parties.");
        ImGui::Text("");
        ImGui::Text(" authors:");
        ImGui::Text("          Edsel Malasig (www.xdlogic.com)");
        ImGui::Text("			Ge Wang (http:www.gewang.com)");
        ImGui::Text("          Perry R. Cook (prc@cs.princeton.edu)");
        ImGui::Text("          Ananya Misra (amisra@cs.princeton.edu)");
        ImGui::Text(" thanks to:");
        ImGui::Text("          Yilei Shao, Qin Lv, Tom Briggs");
        ImGui::Text(" library:");
        ImGui::Text("          (PortAudiolibpo) www.portaudio.com");
        ImGui::Text("          (FFT) CARL CMusic Distribution");
        ImGui::Text("          (Marsyas) George Tzanetakis (gtzan@cs.princeton.edu)");
        ImGui::Text("          (STK) Perry R. Cook (prc@cs.princeton.edu)");
        ImGui::Text("          (libsndfile) Erik de Castro Lopo");
        ImGui::Text("          (ImGUI) Ocornut");
        ImGui::Text("          (AudioDecoder) Marsoni");
        ImGui::Text(" date: 11.28.2003 - initial version");
        ImGui::Text("       spring 2015 - updated for OSX Yosemite");
        ImGui::Text("");
        ImGui::Text(" usage: (type 'h' while running or see list of command line arguments)");
        ImGui::Text(" Special thanks to www.GITHUB.com");
    ImGui::Text("-----------------------------------------------------------------------------");
        ImGui::End();
    }
}

//-----------------------------------------------------------------------------
// name: help()
// desc: ...
//-----------------------------------------------------------------------------
void VirtuosoReality::help()
{
    fprintf( stderr, "----------------------------------------------------\n" );
    fprintf( stderr, "Virtuoso Reality + wutrfall (1.4)\n" );
    fprintf( stderr, "Edsel Malasig, Ge Wang, Perry R. Cook, Ananya Misra\n" );
    fprintf( stderr, "http://www.xdlogic.com/\n" );
    fprintf( stderr, "----------------------------------------------------\n" );
    fprintf( stderr, "'SPACE' - show ui\n");
    fprintf( stderr, "'h' - print this help message\n" );
    fprintf( stderr, "'p' - print current settings to console\n" );
    fprintf( stderr, "'f' - toggle fullscreen\n" );
    fprintf( stderr, "'F2' - freeze frame! (can still rotate/scale)\n" );
    fprintf( stderr, "'1' - toggle waveform display\n" );
    fprintf( stderr, "'q' - toggle lissajous display\n" );
    fprintf( stderr, "'3' - toggle wutrfall display\n" );
    fprintf( stderr, "'4' - toggle feature extraction (broken)\n" );
    fprintf( stderr, "'d' - toggle dB plot for spectrum\n" );
    fprintf( stderr, "'r' - toggle rainbow waterfall\n" );
    fprintf( stderr, "'b' - toggle waterfall moving backwards/forwards\n" );
    fprintf( stderr, "'x' - restart file playback (if applicable)\n" );
    fprintf( stderr, "'n' - print current settings to terminal\n" );
    fprintf( stderr, "'m' - mute\n" );
    fprintf( stderr, "'j' - move spectrum + z\n" );
    fprintf( stderr, "'k' - move spectrum - z\n" );
    fprintf( stderr, "'u' - spacing more!\n" );
    fprintf( stderr, "'z' - spacing less!\n" );
    fprintf( stderr, "L, R mouse button - rotate left or right\n" );
    fprintf( stderr, "'[', ']' - rotate up or down\n" );
    fprintf( stderr, "'-' - scale DOWN the spectrum\n" );
    fprintf( stderr, "'F1' - scale UP the spectrum\n" );
    fprintf( stderr, "'`' - scale DOWN the waveform\n" );
    fprintf( stderr, "'=' - scale UP the waveform\n" );
    fprintf( stderr, "'F4' = LOWER log factor of spectrum display\n" );
    fprintf( stderr, "'c' = RAISE log factor of spectrum display\n" );
    fprintf( stderr, "      (log factor of 1.0 is linear display\n" );
    fprintf( stderr, "'v' - LOWER amount of viewable waveform\n" );
    fprintf( stderr, "'F3' - RAISE amount of viewable waveform\n" );
    fprintf( stderr, "';' - scale DOWN the lissajous!\n" );
    fprintf( stderr, "'l' - scale UP the lissajous!\n" );
    fprintf( stderr, "'y' - decrease delay for lissajous plot\n" );
    fprintf( stderr, "'Y' - increase delay for lissajous plot\n" );
    fprintf( stderr, "'t' - show time\n" );
    fprintf( stderr, "'ESC' - quit\n" );
    fprintf( stderr, "----------------------------------------------------\n" );
    fprintf( stderr, "\n" );
}




//-----------------------------------------------------------------------------
// name: probe()
// desc: probe audio info
//-----------------------------------------------------------------------------
void VirtuosoReality::probe()
{
    
}




//-----------------------------------------------------------------------------
// name: usage()
// desc: ...
//-----------------------------------------------------------------------------
void VirtuosoReality::usage()
{
    fprintf( stderr, "-----------------------\n" );
    fprintf( stderr, "usage: Virtuoso Reality  --[options] [filename]\n" );
    fprintf( stderr, "  ON/OFF options: fullscreen|waveform|lissajous|waterfall|\n" );
    fprintf( stderr, "                  dB|features|fallcolors|backward|showtime|\n" );
    fprintf( stderr, "                  freeze\n" );
    fprintf( stderr, "  number options: inputDevice|outputDevice|\n" );
    fprintf( stderr, "                  timescale|freqscale|lissscale|logfactor|\n" );
    fprintf( stderr, "                  spacing|zpos|dzpos|depth|preview|yview|\n" );
    fprintf( stderr, "                  rotatem|rotatek|begintime|ds\n" );
    fprintf( stderr, "   other options: about|nodisplay|print\n" );
    fprintf( stderr, "\n" );
    fprintf( stderr, "example:\n" );
    fprintf( stderr, "    Virtuoso Reality --fullscreen:ON --inputDevice:1 --spacing:.05\n" );
    fprintf( stderr, "\n" );
    probe();
    fprintf( stderr, "\n" );
    fprintf( stderr, "Virtuoso Reality version: 1.4\n" );
    fprintf( stderr, "    http://www.xdlogic.com/\n" );
    fprintf( stderr, "\n" );
}



//-----------------------------------------------------------------------------
// name: initialize_audio( )
// desc: set up audio capture and playback and initializes any application data
//-----------------------------------------------------------------------------

bool VirtuosoReality::initialize_audio( )
{
    // set sample rate
    Stk::setSampleRate( g_srate );
    
    // variables
    unsigned int bufferBytes;
    // frame size
    unsigned int bufferFrames;
    unsigned int devices;
    
    PaStreamParameters iParams, oParams;
    
    
    // Initialize the PortAudio library.
    if (Pa_Initialize() != paNoError) {
        std::cout << "Failed to initialize audio." << std::endl;
        return false;
    };
    
    // read from file
    if( !g_filename.empty() )
    {
        fprintf( stderr, "[Virtuoso Reality]: opening %s...\n", g_filename.c_str() );
        // attempt to open file
        g_sf = new AudioDecoder(g_filename);
        if (g_sf->open() != AUDIODECODER_OK)
        {
            std::cout << "Failed to open " << g_filename << std::endl;
            return false;
        }
        
        // set flag
        g_file_running = TRUE;
        
        // set srate from the WvIn
        fprintf( stderr, "[Virtuoso Reality]: setting sample rate to %d\n", g_srate );
        g_srate = g_sf->sampleRate();
        PaStream * g_audio;
        
        // check for audio devices
        devices = Pa_GetDeviceCount();
        cout << devices << " audio devices found!" << endl;
        if (devices < 1)
        {
            // nopes
            cout << "[Virtuoso Reality]: ERROR: no audio devices found!" << endl;
            return false;
        }
        
        // set to default
        if (g_audioInputDevice < 0) g_audioInputDevice = Pa_GetDefaultInputDevice();
        if (g_audioOutputDevice < 0) g_audioOutputDevice = Pa_GetDefaultOutputDevice();
        
        // log
        cout << "[Virtuoso Reality]: opening input device: " << g_audioInputDevice
        << " output device: " << g_audioOutputDevice << "..." << endl;
        
        // set input and output parameters
        iParams.device = g_audioInputDevice;
        iParams.channelCount = NUM_CHANNELS;
        //iParams.firstChannel = 0;
        oParams.device = g_audioOutputDevice;
        oParams.channelCount = NUM_CHANNELS;
        //oParams.firstChannel = 0;
        
        bufferFrames = g_buffer_size;
        bufferBytes = 0;
        
        
        // if not done yet...
        // Open the PortAudio stream (opens the soundcard device).
        if (Pa_OpenDefaultStream(&g_audio,
                                 0, // No input channels
                                 2, // 2 output channel
                                 paFloat32, // Sample format (see PaSampleFormat)
                                 44100, // Sample Rate
                                 SND_BUFFER_SIZE,  // Frames per buffer
                                 &cb,
                                 static_cast<void*>(g_sf)) != paNoError)
        {
            std::cout << "Failed to open the default audio stream." << std::endl;
            return false;
        }
        
        // let RtAudio print messages to stderr.
        //g_audio->showWarnings( true );
        
        
        // Start the audio stream. PortAudio will then start calling our callback function
        // every time the soundcard needs audio.
        // Note that this is non-blocking by default!
        if (Pa_StartStream(g_audio) != paNoError)
        {
            std::cout << "Failed to start the audio stream." << std::endl;
            return false;
        }
        
        cout << "opening default stream and file read" << endl;
    }
    else
    {
        // no time-domain waterfall delay!
        g_wf_delay = 0;
    }
    
    //make sound
    if( g_filename.empty() )
    {
        cout << "no file to play." << endl;
        return false;
    }
    
    
    // make the transform VRImGui->window
    hanning( g_window, g_buffer_size );
    
    // initialize waveforms
    if( g_wf_delay )
    {
        g_waveforms = new SAMPLE *[g_wf_delay];
        for( int i = 0; i < g_wf_delay; i++ )
        {
            // allocate memory (stereo)
            g_waveforms[i] = new SAMPLE[g_buffer_size * 2];
            // zero it
            memset( g_waveforms[i], 0, g_buffer_size * 2 * sizeof(SAMPLE) );
        }
    }
    //delete g_sf;
    return true;
    
    
}





//-----------------------------------------------------------------------------
// Name: initialize_analysis( )
// Desc: sets initial audio analysis parameters
//-----------------------------------------------------------------------------
void VirtuosoReality::initialize_analysis( )
{
    // spectral centroid
    g_centroid = new Centroid( SND_MARSYAS_SIZE );
    // down sampler
    g_down_sampler = new DownSampler( g_buffer_size, 2 );
    // flux
    g_flux = new Flux( SND_MARSYAS_SIZE );
    // lpc
    g_lpc = new LPC( SND_MARSYAS_SIZE );
    g_lpc->init();
    // mfcc
    g_mfcc = new MFCC( SND_MARSYAS_SIZE, 0 );
    g_mfcc->init();
    // rms
    g_rms = new RMS( SND_MARSYAS_SIZE );
    // rolloff
    g_rolloff = new Rolloff( SND_MARSYAS_SIZE, 0.5f );
    g_rolloff2 = new Rolloff( SND_MARSYAS_SIZE, 0.8f );
}




//-----------------------------------------------------------------------------
// Name: initialize_graphics( )
// Desc: sets initial OpenGL states and initializes any application data
//-----------------------------------------------------------------------------
void VirtuosoReality::initialize_graphics()
{
    // set the GL clear color - use when the color buffer is cleared
    glClearColor( 0.3f, 0.3f,0.3f, 1.0f );
    // set the shading model to 'smooth'
    glShadeModel( GL_SMOOTH );
    // enable depth
    glEnable( GL_DEPTH_TEST );
    // set the front faces of polygons
    glFrontFace( GL_CCW );
    // set fill mode
    glPolygonMode( GL_FRONT_AND_BACK, g_fillmode );
    // enable lighting
    glEnable( GL_LIGHTING );
    // enable lighting for front
    glLightModeli( GL_FRONT_AND_BACK, GL_TRUE );
    // material have diffuse and ambient lighting
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    // enable color
    glEnable( GL_COLOR_MATERIAL );
    
    // enable light 0
    glEnable( GL_LIGHT0 );
    
    // setup and enable light 1
    glLightfv( GL_LIGHT1, GL_AMBIENT, g_light1_ambient );
    glLightfv( GL_LIGHT1, GL_DIFFUSE, g_light1_diffuse );
    glLightfv( GL_LIGHT1, GL_SPECULAR, g_light1_specular );
    glEnable( GL_LIGHT1 );
    
    // blend? (Jeff's contribution)
    // glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    // glEnable( GL_BLEND );
    
    // initialize
    g_spectrums = new Pt2D *[g_depth];
    for( int i = 0; i < g_depth; i++ )
    {
        g_spectrums[i] = new Pt2D[SND_FFT_SIZE];
        memset( g_spectrums[i], 0, sizeof(Pt2D)*SND_FFT_SIZE );
    }
    g_draw = new GLboolean[g_depth];
    memset( g_draw, 0, sizeof(GLboolean)*g_depth );
    
    // compute log spacing
    g_log_space = compute_log_spacing( g_fft_size / 2, g_log_factor );
}




//-----------------------------------------------------------------------------
// Name: reshapeFunc( )
// Desc: called when window size changes
//-----------------------------------------------------------------------------
void VirtuosoReality::reshapeFunc( int w, int h )
{
    // save the new window size
    ws->g_width = w; ws->g_height = h;
    // map the view port to the client area
    glViewport( 0, 0, w, h );
    // set the matrix mode to project
    
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    glLoadIdentity( );
    // create the viewing frustum
    gluPerspective( 45.0, (GLfloat) w / (GLfloat) h, 1.0f, 300.0 );
    // set the matrix mode to modelview
    
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );
    // position the view point
    
    /*
     gluLookAt( 0.0f, 3.5f * sin( g_eye_y ), 3.5f * cos( g_eye_y ),
     0.0f, 0.0f, 0.0f,
     0.0f, ( cos( g_eye_y ) < 0 ? -1.0f : 1.0f ), 0.0f );
     */
    gluLookAt( ws->camera->Position.x, ws->camera->Position.y, ws->camera->Position.z,
              ws->camera->Position.x + ws->camera->Front.x, ws->camera->Position.y + ws->camera->Front.y, ws->camera->Position.z + ws->camera->Front.z,
              //camera.Front.x, camera.Front.y, camera.Front.z,
              ws->camera->Up.x, ws->camera->Up.y, ws->camera->Up.z);
    // set the position of the lights
    glLightfv( GL_LIGHT0, GL_POSITION, g_light0_pos );
    glLightfv( GL_LIGHT1, GL_POSITION, g_light1_pos );
    
    //SDL_GL_SwapWindow(window);
}



//-----------------------------------------------------------------------------
// Name: keyboardFunc( )
// Desc: key event
//-----------------------------------------------------------------------------
void VirtuosoReality::keyboardFunc( GLFWwindow * window, int key, int scancode, int action = GLFW_PRESS)
{
    const char * s = glfwGetKeyName(GLFW_KEY_COMMA, 0);
    fprintf( stderr, "[Virtuoso Reality]: KEY PRESSED %s\n", glfwGetKeyName(key, 0) );
    switch( key )
    {
        case GLFW_KEY_SPACE:
            showui = !showui;
            fprintf( stderr, "[Virtuoso Reality]: show ui: %b\n", showui );
            break;
        case GLFW_KEY_COMMA:
            g_position -= 500;
            b_seek = true;
            break;
        case GLFW_KEY_PERIOD:
            g_position += 500;
            b_seek = true;
            break;
        case GLFW_KEY_J:
            g_z += g_dz;
            fprintf( stderr, "[Virtuoso Reality]: zpos:%f\n", g_z );
            break;
        case GLFW_KEY_K:
            g_z -= g_dz;
            fprintf( stderr, "[Virtuoso Reality]: zpos:%f\n", g_z );
            break;
        case GLFW_KEY_U:
            g_space *= 1.02f;
            fprintf( stderr, "[Virtuoso Reality]: spacing:%f\n", g_space );
            break;
        case GLFW_KEY_Z:
            g_space *= .98f;
            fprintf( stderr, "[Virtuoso Reality]: spacing:%f\n", g_space );
            break;
        case GLFW_KEY_1:
            g_waveform = !g_waveform;
            fprintf( stderr, "[Virtuoso Reality]: waveform:%s\n", g_waveform ? "ON" : "OFF" );
            break;
        case GLFW_KEY_2:
            g_wutrfall = !g_wutrfall;
            fprintf( stderr, "[Virtuoso Reality]: waterfall:%s\n", g_wutrfall ? "ON" : "OFF" );
            break;
        case GLFW_KEY_3:
            g_usedb = !g_usedb;
            fprintf( stderr, "[Virtuoso Reality]: dB:%s\n", g_usedb ? "ON" : "OFF" );
            break;
        case GLFW_KEY_4:
            g_draw_features = !g_draw_features;
            fprintf( stderr, "[Virtuoso Reality]: features:%s\n", g_draw_features ? "ON" : "OFF" );
            break;
        case GLFW_KEY_ESCAPE:
            exit( 0 );
            glfwSetWindowShouldClose(ws->window, GL_TRUE);
            break;
        case GLFW_KEY_GRAVE_ACCENT:
            g_time_scale *= .99f;
            fprintf( stderr, "[Virtuoso Reality]: timescale:%f\n", g_time_scale );
            break;
        case GLFW_KEY_EQUAL:
            g_time_scale *= 1.01f;
            fprintf( stderr, "[Virtuoso Reality]: timescale:%f\n", g_time_scale );
            break;
        case GLFW_KEY_MINUS:
            g_freq_scale *= .99f;
            fprintf( stderr, "[Virtuoso Reality]: freqscale:%f\n", g_freq_scale );
            break;
        case GLFW_KEY_F1:
            g_freq_scale *= 1.01f;
            fprintf( stderr, "[Virtuoso Reality]: freqscale:%f\n", g_freq_scale );
            break;
            
        case GLFW_KEY_V:
            if( g_time_view > 1 )
                g_time_view--;
            
            fprintf( stderr, "[Virtuoso Reality]: time domain %i samples", g_buffer_size / g_time_view );
            fprintf( stderr, g_time_view == 1 ? " - (MAX)\n" : "\n" );
            break;
        case GLFW_KEY_F3:
            if( g_time_view < 32 )
                g_time_view++;
            
            fprintf( stderr, "[Virtuoso Reality]: time domain %i samples", g_buffer_size / g_time_view );
            fprintf( stderr, g_time_view == 32 ? " - (MIN)\n" : "\n" );
            break;
        case GLFW_KEY_LEFT_BRACKET:
            ws->g_eye_y -= g_inc_val_kb;
            fprintf( stderr, "[Virtuoso Reality]: yview:%f\n", ws->g_eye_y );
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            ws->g_eye_y += g_inc_val_kb;
            fprintf( stderr, "[Virtuoso Reality]: yview:%f\n", ws->g_eye_y );
            break;
        case GLFW_KEY_H:
            help();
            break;
        case GLFW_KEY_F:
        {
            if( !g_fullscreen )
            {
                ws->g_last_width = ws->g_width;
                ws->g_last_height = ws->g_height;
                //glutFullScreen();
            }
            else
                //glutReshapeWindow( g_last_width, g_last_height );
                
                g_fullscreen = !g_fullscreen;
            fprintf( stderr, "[Virtuoso Reality]: fullscreen:%s\n", g_fullscreen ? "ON" : "OFF" );
        }
            break;
        case GLFW_KEY_M:
            g_mute = !g_mute;
            fprintf( stderr, "[Virtuoso Reality]: mute:%s\n", g_mute ? "ON" : "OFF" );
            break;
        case GLFW_KEY_X:
            if( g_sf != NULL)
            {
                g_restart = TRUE;
                fprintf( stderr, "[Virtuoso Reality]: restarting file...\n" );
            }
            break;
        case GLFW_KEY_Q:
            g_lissajous = !g_lissajous;
            fprintf( stderr, "[Virtuoso Reality]: lissajous:%s\n", g_lissajous ? "ON" : "OFF" );
            break;
        case GLFW_KEY_SEMICOLON:
            g_lissajous_scale *= .95f;
            fprintf( stderr, "[Virtuoso Reality]: lissscale:%f\n", g_lissajous_scale );
            break;
        case GLFW_KEY_L:
            g_lissajous_scale *= 1.05f;
            fprintf( stderr, "[Virtuoso Reality]: lissscale:%f\n", g_lissajous_scale );
            break;
        case GLFW_KEY_Y:
            g_delay -= 10;
            if( g_delay < 0 )
                g_delay = 0;
            fprintf( stderr, "[Virtuoso Reality]: lissdelay = %i\n", g_delay );
            break;
        case GLFW_KEY_I:
            g_delay += 10;
            if( g_delay > g_buffer_size )
                g_delay = g_buffer_size;
            fprintf( stderr, "[Virtuoso Reality]: lissdelay = %i\n", g_delay );
            break;
        case GLFW_KEY_F2:
            g_freeze = g_pause = !g_pause;
            fprintf( stderr, "[Virtuoso Reality]: free(ze)!\n" );
            break;
        case GLFW_KEY_F4:
            g_log_factor *= .98; //.99985;
            g_log_space = compute_log_spacing( g_fft_size / 2, g_log_factor );
            fprintf( stderr, "[Virtuoso Reality]: logfactor:%f\n", g_log_factor );
            break;
        case GLFW_KEY_C:
            g_log_factor /= .98; //.99985;
            g_log_space = compute_log_spacing( g_fft_size / 2, g_log_factor );
            fprintf( stderr, "[Virtuoso Reality]: logfactor:%f\n", g_log_factor );
            break;
        case GLFW_KEY_R:
            g_rainbow = !g_rainbow;
            fprintf( stderr, "[Virtuoso Reality]: fallcolors:%s\n", g_rainbow ? "ON" : "OFF" );
            break;
        case GLFW_KEY_T:
            g_show_time = !g_show_time;
            fprintf( stderr, "[Virtuoso Reality]: show time:%s\n", g_show_time ? "ON" : "OFF" );
            break;
        case GLFW_KEY_B:
            g_backwards = !g_backwards;
            fprintf( stderr, "[Virtuoso Reality]: backward:%s\n", g_backwards ? "ON" : "OFF" );
            break;
        case GLFW_KEY_N:
            fprintf( stderr, "----------------------------------------------------\n" );
            fprintf( stderr, " current Virtuoso Reality settings...\n" );
            fprintf( stderr, "----------------------------------------------------\n" );
            fprintf( stderr, "[Virtuoso Reality]: waveform:%s\n", g_waveform ? "ON" : "OFF" );
            fprintf( stderr, "[Virtuoso Reality]: lissajous:%s\n", g_lissajous ? "ON" : "OFF" );
            fprintf( stderr, "[Virtuoso Reality]: waterfall:%s\n", g_wutrfall ? "ON" : "OFF" );
            fprintf( stderr, "[Virtuoso Reality]: features:%s\n", g_draw_features ? "ON" : "OFF" );
            fprintf( stderr, "[Virtuoso Reality]: fallcolors:%s\n", g_rainbow ? "ON" : "OFF" );
            fprintf( stderr, "[Virtuoso Reality]: backward:%s\n", g_backwards ? "ON" : "OFF" );
            fprintf( stderr, "[Virtuoso Reality]: fullscreen:%s\n", g_fullscreen ? "ON" : "OFF" );
            fprintf( stderr, "[Virtuoso Reality]: dB:%s\n", g_usedb ? "ON" : "OFF" );
            fprintf( stderr, "[Virtuoso Reality]: mute:%s\n", g_mute ? "ON" : "OFF" );
            fprintf( stderr, "[Virtuoso Reality]: showtime:%s\n", g_show_time ? "ON" : "OFF" );
            fprintf( stderr, "[Virtuoso Reality]: freeze:%s\n", g_freeze ? "ON" : "OFF" );
            fprintf( stderr, "[Virtuoso Reality]: timescale:%f\n", g_time_scale );
            fprintf( stderr, "[Virtuoso Reality]: freqscale:%f\n", g_freq_scale );
            fprintf( stderr, "[Virtuoso Reality]: logfactor:%f\n", g_log_factor );
            fprintf( stderr, "[Virtuoso Reality]: lissscale:%f\n", g_lissajous_scale );
            fprintf( stderr, "[Virtuoso Reality]: lissdelay = %i\n", g_delay );
            fprintf( stderr, "[Virtuoso Reality]: zpos:%f\n", g_z );
            fprintf( stderr, "[Virtuoso Reality]: dzpos:%f\n", g_dz );
            fprintf( stderr, "[Virtuoso Reality]: spacing:%f\n", g_space );
            fprintf( stderr, "[Virtuoso Reality]: yview:%f\n", ws->g_eye_y );
            fprintf( stderr, "[Virtuoso Reality]: depth:%i\n", g_depth );
            fprintf( stderr, "[Virtuoso Reality]: preview:%f (delay: %i)\n", g_wf_delay_ratio, g_wf_delay);
            fprintf( stderr, "[Virtuoso Reality]: rotatem:%f\n", g_inc_val_mouse );
            fprintf( stderr, "[Virtuoso Reality]: rotatek:%f\n", g_inc_val_kb * (INC_VAL_MOUSE/INC_VAL_KB) );
            fprintf( stderr, "[Virtuoso Reality]: begintime:%f (seconds)\n", g_begintime );
            fprintf( stderr, "[Virtuoso Reality]: ds:%i\n", g_ds );
            fprintf( stderr, "----------------------------------------------------\n" );
            break;
        default:
            
            break;
            
    }
    
    // do a reshape since g_eye_y might have changed
    
    doMovement(key);
    reshapeFunc(ws->g_width,ws->g_height);
    
    //glutPostRedisplay( );
    
}

void VirtuosoReality::doMovement(int key){
    if(key == GLFW_KEY_W)
        ws->camera->ProcessKeyboard(FORWARD, ws->deltaTime);
    if(key == GLFW_KEY_S)
        ws->camera->ProcessKeyboard(BACKWARD, ws->deltaTime);
    if(key == GLFW_KEY_A)
        ws->camera->ProcessKeyboard(LEFT, ws->deltaTime);
    if(key == GLFW_KEY_D)
        ws->camera->ProcessKeyboard(RIGHT, ws->deltaTime);
}

//-----------------------------------------------------------------------------
// Name: mouseFunc( )
// Desc: handles mouse stuff
//-----------------------------------------------------------------------------
GLFWcursorposfun VirtuosoReality::mouseFunc(double xpos, double ypos)
{
    if(ws->firstMouse)
    {
        ws->lastX = xpos;
        ws->lastY = ypos;
        ws->firstMouse = false;
    }
    
    GLfloat xoffset = xpos - ws->lastX;
    GLfloat yoffset = ws->lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    
    ws->lastX = xpos;
    ws->lastY = ypos;
    
    ws->camera->ProcessMouseMovement(xoffset, yoffset);
    reshapeFunc(ws->g_width, ws->g_height);
    //SDL_GL_SwapWindow(window);
    
    return 0;
    
}




//-----------------------------------------------------------------------------
// Name: idleFunc( )
// Desc: callback from GLUT
//-----------------------------------------------------------------------------
void VirtuosoReality::idleFunc( )
{
    // render the scene
    //glutPostRedisplay( );
}

//-----------------------------------------------------------------------------
// name: draw_string()
// desc: ...
//-----------------------------------------------------------------------------
void VirtuosoReality::draw_string( GLfloat x, GLfloat y, GLfloat z, const char * str, GLfloat scale = 1.0f )
{
    GLint len = strlen( str ), i;
    
    glPushMatrix();
    glTranslatef( x, y, z );
    glScalef( .001f * scale, .001f * scale, .001f * scale );
    
    for( i = 0; i < len; i++ )
        glutStrokeCharacter( GLUT_STROKE_ROMAN, str[i] );
    
    glPopMatrix();
}


//-----------------------------------------------------------------------------
// name: ...
// desc: ...
//-----------------------------------------------------------------------------
void VirtuosoReality::draw_Lissajous( SAMPLE * stereobuffer, int len, int channels)
{
    float x, y;
    SAMPLE * buffer;
    
    // 1 or 2 channels only for now
    //assert( channels >= 1 && channels <= 2 );
    
    // mono
    if( g_sf->channels() == 1 )
    {
        buffer = g_cur_buffer;
        // convert to mono
        for( int m = 0; m < len; m++)
        {
            buffer[m] = stereobuffer[m*2] + stereobuffer[m*2+1];
            buffer[m] /= 2.0f;
        }
    }
    else
    {
        buffer = stereobuffer;
    }
    
    // color
    glColor3f( 1.0f, 1.0f, .5f );
    // save current matrix state
    glPushMatrix();
    // translate
    glTranslatef( 1.2f, 0.0f, 0.0f );
    // draw it
    glBegin( GL_POINTS );
    for( int i = 0; i < len * channels; i += channels )
    {
        x = buffer[i] * g_lissajous_scale;
        if( channels == 1 )
        {
            // delay
            y = (i - g_delay >= 0) ? buffer[i-g_delay] : g_back_buffer[len + i-g_delay];
            y *= g_lissajous_scale;
        }
        else
        {
            y = buffer[i + channels-1] * g_lissajous_scale;
        }
        
        glVertex3f( x, y, 0.0f );
        // glVertex3f( x, y, sqrt( x*x + y*y ) * -g_lissajous_scale );
    }
    glEnd();
    // restore matrix state
    glPopMatrix();
    
    // hmm...
    if( channels == 1 )
        memcpy( g_back_buffer, buffer, len * sizeof(SAMPLE) );
}




//-----------------------------------------------------------------------------
// Name: map_log_spacing( )
// Desc: ...
//-----------------------------------------------------------------------------
double VirtuosoReality::map_log_spacing( double ratio, double power )
{
    // compute location
    return ::pow(ratio, power) * g_fft_size/g_freq_view;
}




//-----------------------------------------------------------------------------
// Name: compute_log_spacing( )
// Desc: ...
//-----------------------------------------------------------------------------
double VirtuosoReality::compute_log_spacing( int fft_size, double power )
{
    static int maxbin = fft_size; // for future in case we want to draw smaller range
    static int minbin = 0; // what about adding this one?
    
    for(int i = 0; i < fft_size; i++)
    {
        // compute location
        g_log_positions[i] = map_log_spacing( (double)i/fft_size, power );
        // normalize, 1 if maxbin == fft_size
        g_log_positions[i] /= pow((double)maxbin/fft_size, power);
    }
    
    return 1/::log(fft_size);
}




//-----------------------------------------------------------------------------
// Name: displayFunc( )
// Desc: callback function invoked to draw the client area
//-----------------------------------------------------------------------------
void VirtuosoReality::displayFunc( )
{
    // static variables to keep across function calls
    static const int LP = 4;
    static long int count = 0;
    static char str[1024];
    static float centroid_val, flux_val, rms_val, rolloff_val, rolloff2_val;
    static fvec in(SND_MARSYAS_SIZE),
    centroid(1), flux(1), lpc(g_lpc->outSize()), mfcc(13), rms(1), rolloff(1),
    rolloff2(1), centroid_lp(LP), flux_lp(LP), rms_lp(LP), rolloff_lp(LP),
    rolloff2_lp(LP);
    
    // local variables
    SAMPLE * buffer = new SAMPLE; buffer = g_fft_buffer; SAMPLE * ptr = new SAMPLE; ptr = in.getData();
    GLfloat ytemp, fval;
    GLint i;
    
    // clear the color and depth buffers
    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // save current matrix state
    glPushMatrix( );
    
    // rotate the sphere about y axis
    glRotatef( g_angle_y += g_inc, 0.0f, 1.0f, 0.0f );
    
    // wait for data
    while( !g_ready ) usleep( 1000 );
    
    // lock
    g_mutex.lock();
    
    // get the latest (possibly preview) window
    memset( buffer, 0, SND_FFT_SIZE * sizeof(SAMPLE) );
    
    // copy currently playing audio into buffer
    memcpy( buffer, g_audio_buffer, g_buffer_size * sizeof(SAMPLE) );
    
    // some flag (hand off to audio cb thread)
    g_ready = FALSE;
    
    // unlock
    g_mutex.unlock();
    
    // lissajous
    if( g_lissajous )
    {
        if( g_filename.empty() ) { // real-time mic input
            draw_Lissajous( g_stereo_buffer, g_buffer_size, 1 );
        } else { // reading from file
            draw_Lissajous( g_stereo_buffer, g_buffer_size, 2 );
        }
    }
    
    // soon to be used drawing offsets
    GLfloat x = -1.8f, inc = 3.6f / g_buffer_size, y = .7f;
    // apply the transform window
    apply_window( (float*)buffer, g_window, g_buffer_size );
    
    // draw the time domain waveform
    if( g_waveform )
    {
        // save the current matrix state
        glPushMatrix();
        // color waveform
        glColor3f( 0.3f, 0.3f, 1.0f );
        // translate the waveform
        glTranslatef( x, y, 0.0f );
        // scale visually
        glScalef( inc * g_time_view , g_gain * g_time_scale * 2.0, 1.0 );
        // set vertex normals (for somewhat controlled lighting)
        glNormal3f( 0.0f, 0.0f, 1.0f );
        // draw waveform
        glLineWidth(2.5f);
        glBegin( GL_LINE_STRIP );
        {
            GLint ii = ( g_buffer_size - (g_buffer_size/g_time_view) ) / 2;
            GLfloat xcoord = 0.0f;
            // loop through samples
            for( i = ii; i < ii + g_buffer_size / g_time_view; i++ )
            {
                glVertex2f( xcoord++ , buffer[i] );
            }
            glEnd();
        }
        // restore previous matrix state
        glPopMatrix();
    }
    
    // take forward FFT; result in buffer as FFT_SIZE/2 complex values
    rfft( (float *)buffer, g_fft_size/2, FFT_FORWARD );
    // cast to complex
    complex * cbuf = (complex *)buffer;
    
    // reset drawing offsets
    x = -1.8f;
    y = -1.0f;
    
    // color the spectrum
    glColor3f( 0.4f, 1.0f, 0.4f );
    // set vertex normals
    glNormal3f( 0.0f, 1.0f, 0.0f );
    
    // copy current magnitude spectrum into waterfall memory
    for( i = 0; i < g_fft_size/2; i++ )
    {
        // copy x coordinate
        g_spectrums[g_wf][i].x = x;
        // copy y, depending on scaling
        if( !g_usedb ) {
            g_spectrums[g_wf][i].y = g_gain * g_freq_scale * 1.8f *
            ::pow( 25 * cmp_abs( cbuf[i] ), .5 ) + y;
        } else {
            g_spectrums[g_wf][i].y = g_gain * g_freq_scale *
            ( 20.0f * log10( cmp_abs(cbuf[i])/8.0 ) + 80.0f ) / 80.0f + y + .5f;
        }
        // increment x
        x += inc * g_freq_view;
    }
    
    // draw the right things
    g_draw[g_wf] = g_wutrfall;
    if( !g_starting )
        g_draw[(g_wf+g_wf_delay)%g_depth] = true;
    
    // reset drawing variables
    x = -1.8f;
    inc = 3.6f / g_fft_size;
    
    // save current matrix state
    glPushMatrix();
    // translate in world coordinate
    glTranslatef( x, 0.0, g_z );
    // scale it
    glScalef( inc*g_freq_view , 1.0 , -g_space );
    // loop through each layer of waterfall
    for( i = 0; i < g_depth; i++ )
    {
        if( i == g_wf_delay || !g_freeze || g_wutrfall )
        {
            // if layer is flagged for draw
            if( g_draw[(g_wf+i)%g_depth] )
            {
                // get the magnitude spectrum of layer
                Pt2D * pt = g_spectrums[(g_wf+i)%g_depth];
                // future
                if( i < g_wf_delay )
                {
                    // brightness based on depth
                    fval = (g_depth - g_wf_delay + i) / (float)(g_depth);
                    // rain or not
                    if( !g_rainbow ){
                        glColor3f( 1 * fval, 0.7 * fval, 0.2 * fval ); // depth cue
                        // interesting colors: (.7, 1, .2), (.4, .9. 1), (1.0, 0.7, 0.2)
                    } else {
                        // rainbow colors
                        float cval = 1 - (g_wf_delay - i) / (float)(g_wf_delay);
                        cval = 0.4f + cval * (1.0f - 0.4f);
                        glColor3f( 7.0f * fval, cval * fval, 0.2f * fval );
                    }
                }
                // present
                else if( i == g_wf_delay )
                {
                    // draw the now line?
                    if( g_draw_play )
                    {
                        glLineWidth( g_filename.empty() ? 2.0f : 3.0f );
                        glColor3f( .4f, 1.0f, 1.0f );
                    }
                }
                // past
                else
                {
                    // brightness based on depth
                    fval = (g_depth - i + g_wf_delay) / (float)(g_depth);
                    // draw rainbow?
                    if( !g_rainbow ) {
                        glColor3f( .4f * fval, 1.0f * fval, .4f * fval ); //depth cue
                    } else {
                        // rainbow-ish
                        float cval = 1 - (i - g_wf_delay) / (float)(g_depth - g_wf_delay);
                        cval = 0.4f + cval * (1.0f - 0.4f);
                        glColor3f( cval * fval, 1.0f * fval, .4f * fval );
                    }
                }
                
                // render the actual spectrum layer
                glBegin( GL_LINE_STRIP );
                for( GLint j = 0; j < g_fft_size/g_freq_view; j++, pt++ )
                {
                    // draw the vertex
                    float d = g_backwards ? g_depth - (float) i : (float) i;
                    glVertex3f( g_log_positions[j], pt->y, d );
                }
                glEnd();
                
                // back to default line width
                glLineWidth(1.5f);
            }
        }
    }
    // restore matrix state
    glPopMatrix();
    
    // if flagged, mark layer NOT to be drawn
    if( !g_wutrfall )
        g_draw[(g_wf+g_wf_delay)%g_depth] = false;
    
    // wtrfll
    if( !g_freeze )
    {
        // advance index
        g_wf--;
        // mod
        g_wf = (g_wf + g_depth) % g_depth;
        // can't remember what this does anymore...
        if( g_wf == g_depth - g_wf_delay )
            g_starting = 0;
    }
    
    // calculate and draw features
    if( g_draw_features )
    {
        // if not frozen
        if( !g_freeze )
        {
            // for rough downsampling
            int ratio = g_fft_size / SND_MARSYAS_SIZE / 2;
            // get magnitude response
            for( i = 0; i < SND_MARSYAS_SIZE; i++ )
                ptr[i] = cmp_abs( cbuf[i*ratio] );
            
            // centroid
            g_centroid->process( in, centroid );
            // flux
            g_flux->process( in, flux );
            // rms
            g_rms->process( in, rms );
            // rolloff 1
            g_rolloff->process( in, rolloff );
            // rolloff 2
            g_rolloff2->process( in, rolloff2 );
            
            // lowpass
            centroid_lp(count % LP) = centroid(0);
            flux_lp(count % LP) = flux(0);
            rms_lp(count % LP) = rms(0);
            rolloff_lp(count % LP) = rolloff(0);
            rolloff2_lp(count % LP) = rolloff2(0);
            count++;
            
            // get average values
            centroid_val = centroid_lp.mean();
            flux_val = flux_lp.mean();
            rms_val = rms_lp.mean();
            rolloff_val = rolloff_lp.mean();
            rolloff2_val = rolloff2_lp.mean();
        }
        
        // draw the centroid
        // TODO: need to update 'inc'?
        ytemp = y+.04f + 2 * (::pow( 30 * rms_val, .5 ) );
        float centroid_x = map_log_spacing( centroid_val/SND_MARSYAS_SIZE, g_log_factor );
        centroid_x *= (inc*g_freq_view);
        glColor3f( 1.0f, .4f, .4f );
        glBegin( GL_LINE_STRIP );
        glVertex3f( -1.8f + centroid_x, ytemp, 0.0f + g_z );
        glVertex3f( -1.8f + centroid_x, y-.04f, 0.0f + g_z );
        glEnd();
        
        // centroid value
        glBegin( GL_LINE_STRIP );
        glVertex3f( -1.8f + centroid_x, y-.04f, 0.0f + g_z );
        glVertex3f( -1.72f + centroid_x, y-.15f, 0.0f + g_z );
        glVertex3f( -1.15f + centroid_x, y-.15f, 0.0f + g_z );
        glEnd();
        
        // print centroid
        sprintf( str, "centroid = %.0f Hz", centroid_val / SND_MARSYAS_SIZE * g_srate / 2 );
        draw_string( -1.7f + centroid_x, y-.14f, 0.0f + g_z, str, .4f );
        
        // rms value
        glBegin( GL_LINE_STRIP );
        glVertex3f( -1.8f + centroid_x - .23f, ytemp, 0.0f + g_z );
        glVertex3f( -1.8f + centroid_x + .23f, ytemp, 0.0f + g_z );
        glEnd();
        
        // print RMS
        sprintf( str, "RMS = %f", 1000 * rms_val );
        draw_string( -1.8f + centroid_x - .23f, ytemp + .01f, 0.0f + g_z, str, 0.4f );
        
        // draw the rolloff
        glColor3f( 1.0f, 1.0f, .4f );
        float rolloff_x = map_log_spacing( rolloff_val/SND_MARSYAS_SIZE, g_log_factor );
        rolloff_x *= (inc*g_freq_view);
        glBegin( GL_LINE_STRIP );
        glVertex3f( -1.8f + rolloff_x, y-.04f, 0.0f + g_z );
        glVertex3f( -1.8f + rolloff_x, y+.04f, 0.0f + g_z );
        glEnd();
        
        // draw other rolloff
        glColor3f( 1.0f, 1.0f, 1.0f );
        float rolloff2_x = map_log_spacing( rolloff2_val/SND_MARSYAS_SIZE, g_log_factor );
        rolloff2_x *= (inc*g_freq_view);
        glBegin( GL_LINE_STRIP );
        glVertex3f( -1.8f + rolloff2_x, y-.04f, 0.0f + g_z );
        glVertex3f( -1.8f + rolloff2_x, y+.04f, 0.0f + g_z );
        glEnd();
        
        // centroid
        sprintf( str, "centroid = %.0f", centroid_val / SND_MARSYAS_SIZE * g_srate / 2 );
        draw_string( -1.7f, 0.4f, 0.0f, str, 0.4f );
        // flux
        sprintf( str, "flux = %.1f", flux_val );
        draw_string( -1.7f, 0.3f, 0.0f, str, 0.4f );
        // flux
        sprintf( str, "RMS = %.4f", 1000 * rms_val );
        draw_string( -1.7f, 0.2f, 0.0f, str, 0.4f );
        // flux
        sprintf( str, "50%% rolloff= %.0f", rolloff_val / SND_MARSYAS_SIZE * g_srate / 2 );
        draw_string( -1.7f, 0.1f, 0.0f, str, 0.4f );
        // flux
        sprintf( str, "80%% rolloff = %.0f", rolloff2_val / SND_MARSYAS_SIZE * g_srate / 2 );
        draw_string( -1.7f, 0.0f, 0.0f, str, 0.4f );
    }
    
    // print to console
    if( g_stdout )
    {
        fprintf( stdout, "%.2f  %.2f  %.8f  %.2f  %.2f  ", centroid(0), flux(0), rms(0), rolloff(0), rolloff2(0) );
        fprintf( stdout, "%.2f  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f %.2f %.2f  ",
                mfcc(0), mfcc(1), mfcc(2), mfcc(3), mfcc(4), mfcc(5), mfcc(6),
                mfcc(7), mfcc(8), mfcc(9), mfcc(10), mfcc(11), mfcc(12) );
        fprintf( stdout, "\n" );
    }
    
    // set color
    glColor3f( 1, 1, 1 );
    
    // title
    draw_string( 0.0f, 0.2f, 0.0f, "Virtuoso Reality + wutrfall", .5f );
    
    // time
    if( 1 )//g_show_time )
    {
        //float fsec = ((double)(double)g_sf->seek(t) - (double)g_wf_delay * g_buffer_size) / (double)g_srate;
        sprintf( str, "%.0f : %.0f", g_sf->positionInSamples(), samplesRead );
        draw_string( -1.7f, 1.1f, -.2f, str, .4f );
    }
    
    // pause?
    if( g_pause )
        draw_string( 0.95f, 1.1f, -.2f, "paused... (press f to resume)", .4f );
    
    // mute?
    if( g_mute )
        draw_string( 0.95f, 1.05f, -.2f, "muted... (press m to unmute)", .4f );
    
    // restore matrix state
    glPopMatrix( );
    
    // flush gl commands
    glFlush( );
    // swap the buffers
    //glutSwapBuffers( );
    
    // maintain count from render
    g_buffer_count_b++;
    // check against count from reading function
    if( !g_filename.empty() && !g_file_running && g_buffer_count_b == g_buffer_count_a )
        g_running = FALSE;
}




//-----------------------------------------------------------------------------
// Name: extract_buffer( )
// Desc: extract one buffer
//-----------------------------------------------------------------------------
void VirtuosoReality::extract_buffer( )
{
    // static stuff
    static fvec raw(g_buffer_size), in(SND_MARSYAS_SIZE),
    centroid(1), flux(1), lpc(g_lpc->outSize()), mfcc(13), rms(1), rolloff(1),
    rolloff2(1);
    
    // local
    SAMPLE * buffer = new SAMPLE; buffer = g_fft_buffer; SAMPLE * ptr = new SAMPLE; ptr = in.getData();
    GLint i;
    
    // wait for reading
    while( !g_ready )
        usleep( 1000 );
    
    // get data
    if( g_filename.empty() )
    {
        g_mutex.lock();
        memcpy( buffer, g_audio_buffer, g_buffer_size * sizeof(SAMPLE) );
        g_ready = FALSE;
        g_mutex.unlock();
    }
    else
    {
        memcpy( buffer, g_audio_buffer, g_buffer_size * sizeof(SAMPLE) );
        g_ready = FALSE;
    }
    
    // apply the window
    apply_window( (float*)buffer, g_window, g_buffer_size );
    
    // take the forward fft, leaving fftsize/2 complex values
    rfft( (float *)buffer, g_buffer_size/2, FFT_FORWARD );
    // cast to complex
    complex * cbuf = (complex *)buffer;
    
    // get magnitude spectrum
    for( i = 0; i < g_buffer_size/2; i++ )
        ptr[i] = cmp_abs( cbuf[i] );
    
    // centroid
    g_centroid->process( in, centroid );
    // flux
    g_flux->process( in, flux );
    // lpc
    g_lpc->process( in, lpc );
    // mfcc
    g_mfcc->process( in, mfcc );
    // rms
    g_rms->process( in, rms );
    // rolloff
    g_rolloff->process( in, rolloff );
    g_rolloff2->process( in, rolloff2 );
    
    // print to console
    if( g_stdout )
    {
        fprintf( stdout, "%.2f  %.2f  %.8f  %.2f  %.2f  ", centroid(0), flux(0), rms(0), rolloff(0), rolloff2(0) );
        fprintf( stdout, "%.2f  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f %.2f %.2f  ",
                mfcc(0), mfcc(1), mfcc(2), mfcc(3), mfcc(4), mfcc(5), mfcc(6),
                mfcc(7), mfcc(8), mfcc(9), mfcc(10), mfcc(11), mfcc(12) );
        fprintf( stdout, "\n" );
    }
    
    // file reading stuff
    g_buffer_count_b++;
    if( !g_filename.empty() && !g_file_running && g_buffer_count_a == g_buffer_count_b )
        g_running = FALSE;
}
