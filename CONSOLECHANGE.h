#pragma once
#ifndef _CONSOLE_CHANGE_H_
#define _CONSOLE_CHANGE_H_


#ifdef _WIN32
	#define CLEAR_SCREEN "cls"
#else
	#define CLEAR_SCREEN "clear"
#endif // !_WIN32

#endif