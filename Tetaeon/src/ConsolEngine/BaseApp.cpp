// Copyright 2009-2014 Blam Games, Inc. All Rights Reserved.
#pragma warning(disable : 4996)
#include "BaseApp.h"

#include <iostream>
#include <algorithm>
#include <time.h>
#include <conio.h>
#include <assert.h>
#include <strsafe.h>

#define MY_PERFORMENCE_COUNTER
#include "PerformanceCounter.h"
#include "../Debug.h"

BaseApp::BaseApp(int xSize, int ySize)
: X_SIZE(xSize)
, Y_SIZE(ySize)
{
    SetWindowSize(X_SIZE, Y_SIZE);
    DisableCursor();

	mChiBuffer = (CHAR_INFO*)malloc((X_SIZE+1)*(Y_SIZE+1)*sizeof(CHAR_INFO));

	mDwBufferSize.X = X_SIZE + 1;
	mDwBufferSize.Y = Y_SIZE + 1;		// размер буфера данных

	mDwBufferCoord.X = 0;
	mDwBufferCoord.Y = 0;				// координаты €чейки

	mLpWriteRegion.Left = 0;
	mLpWriteRegion.Top = 0;
	mLpWriteRegion.Right = X_SIZE + 1;
	mLpWriteRegion.Bottom = Y_SIZE + 1;	// пр€моугольник дл€ чтени€


	for (int x=0; x<X_SIZE+1; x++)
	{
		for (int y=0; y<Y_SIZE+1; y++)
		{
			SetChar(x, y, L' ');
		}
	}
}

BaseApp::~BaseApp()
{
	free(mChiBuffer);
}

void BaseApp::SetWindowSize(int X_SIZE, int Y_SIZE)
{
    SMALL_RECT windowSize = { 0, 0, (short)X_SIZE, (short)Y_SIZE };
    COORD windowBufSize = { (short)X_SIZE + 1, (short)Y_SIZE + 1 };

    mConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
    mConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (!SetConsoleWindowInfo(mConsoleOut, TRUE, &windowSize))
    {
        Dout() << "SetConsoleWindowInfo failed with error " << GetLastError() << std::endl;
    }
    if (!SetConsoleScreenBufferSize(mConsoleOut, windowBufSize))
    {
        Dout() << "SetConsoleScreenBufferSize failed with error " << GetLastError() << std::endl;
    }
}

void BaseApp::DisableCursor()
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(mConsoleOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    cursorInfo.dwSize = 1;
    SetConsoleCursorInfo(mConsoleOut, &cursorInfo);
}

void BaseApp::SetSquaredFont()
{
    static bool isSquared = false;
    if (isSquared) return;

    PCONSOLE_FONT_INFOEX mLpConsoleCurrentFontEx = (PCONSOLE_FONT_INFOEX)malloc(sizeof(CONSOLE_FONT_INFOEX));
    mLpConsoleCurrentFontEx->cbSize = sizeof(CONSOLE_FONT_INFOEX);
    if (!GetCurrentConsoleFontEx(mConsoleOut, FALSE, mLpConsoleCurrentFontEx))
    {
        Dout() << "GetCurrentConsoleFontEx failed with error " << GetLastError() << std::endl;
    }

    wchar_t faceName[] = L"Terminal";
    memcpy(mLpConsoleCurrentFontEx->FaceName, faceName, sizeof(faceName));
    isSquared =
        mLpConsoleCurrentFontEx->dwFontSize.X == mLpConsoleCurrentFontEx->dwFontSize.Y and
        mLpConsoleCurrentFontEx->dwFontSize.X == 35;

    mLpConsoleCurrentFontEx->dwFontSize.X = 35;
    mLpConsoleCurrentFontEx->dwFontSize.Y = 35;
    if (!SetCurrentConsoleFontEx(mConsoleOut, FALSE, mLpConsoleCurrentFontEx))
    {
        Dout() << "SetCurrentConsoleFontEx failed with error " << GetLastError() << std::endl;
    }
    free(mLpConsoleCurrentFontEx);
}

void BaseApp::SetChar(int x, int y, wchar_t c, int attributes)
{
	mChiBuffer[x + (X_SIZE+1)*y].Char.UnicodeChar = c;
	mChiBuffer[x + (X_SIZE+1)*y].Attributes = attributes;
}

wchar_t BaseApp::GetChar(int x, int y)
{
	return mChiBuffer[x + (X_SIZE+1)*y].Char.UnicodeChar;
}

void BaseApp::Render()
{
	if (!WriteConsoleOutput(mConsoleOut, mChiBuffer, mDwBufferSize, mDwBufferCoord, &mLpWriteRegion)) 
	{
		Dout() << "WriteConsoleOutput failed with error " << GetLastError() << std::endl; 
	}
}

void BaseApp::Run()
{
	CStopwatch timer;
	int sum = 0;
	int counter = 0;

	int deltaTime = 0;
	while (1)
	{
        SetSquaredFont();
		timer.Start();
		if (kbhit())
		{
			KeyPressed (getch());
			if (!FlushConsoleInputBuffer(mConsoleIn))
                Dout() << "FlushConsoleInputBuffer failed with error " << GetLastError();
		}

		UpdateF((float)deltaTime / 1000.0f);
		Render();

		while (1)
		{
			deltaTime = timer.Now();
			if (deltaTime >= 17)
				break;
		}


		sum += deltaTime;
		counter++;
		if (sum >= 1000)
		{
			TCHAR  szbuff[255];
			StringCchPrintf(szbuff, 255, TEXT("FPS: %d"), counter);
			SetConsoleTitle(szbuff);

			counter = 0;
			sum = 0;
		}
	}
}
