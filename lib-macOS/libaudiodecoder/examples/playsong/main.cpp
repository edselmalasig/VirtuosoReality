//
//  main.cpp
//  AudioTesting
//
//  Created by Edsel Malasig on 1/13/17.
//  Copyright © 2017 xdlogic. All rights reserved.
//

#include <iostream>
#include "AudioTesting.h"

int main(int argc, const char ** argv) {
    PlaySong * p = new PlaySong();
    p->play(argc, argv);
    std::cout << "Hello, World!\n";
    return 0;
}
 
