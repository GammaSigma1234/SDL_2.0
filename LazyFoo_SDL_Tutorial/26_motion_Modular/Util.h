/**
 * @file Util.h
 * 
 * @brief 
 * 
 * @copyright This source code copyrighted by Lazy Foo' Productions (2004-2022)
 * and may not be redistributed without written permission.
 **/

#ifndef UTIL_H
#define UTIL_H

//Starts up SDL and creates window
bool init(void);

//Loads media
bool loadMedia(void);

//Frees media and shuts down SDL
void close(void);

void PressEnter(void);

#endif
