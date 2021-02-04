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

BaseApp::BaseApp(int sizeX, int sizeY)
: SIZE_X(sizeX)
, SIZE_Y(sizeY)
{
    SetWindowSize(SIZE_X, SIZE_Y);
    DisableCursor();

	mChiBuffer = (CHAR_INFO*)malloc((SIZE_X+1)*(SIZE_Y+1)*sizeof(CHAR_INFO));

	mDwBufferSize.X = SIZE_X + 1;
	mDwBufferSize.Y = SIZE_Y + 1;		// размер буфера данных

	mDwBufferCoord.X = 0;
	mDwBufferCoord.Y = 0;				// координаты €чейки

	mLpWriteRegion.Left = 0;
	mLpWriteRegion.Top = 0;
	mLpWriteRegion.Right = SIZE_X + 1;
	mLpWriteRegion.Bottom = SIZE_Y + 1;	// пр€моугольник дл€ чтени€


	for (short x=0; x<SIZE_X+1; x++)
	{
		for (short y=0; y<SIZE_Y+1; y++)
		{
            SetChar({ x, y }, L' ');
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

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int fontSize = int(screenWidth/1920.0 * 25);
    wchar_t faceName[] = L"Terminal";
    memcpy(mLpConsoleCurrentFontEx->FaceName, faceName, sizeof(faceName));
    isSquared =
        mLpConsoleCurrentFontEx->dwFontSize.X == mLpConsoleCurrentFontEx->dwFontSize.Y and
        mLpConsoleCurrentFontEx->dwFontSize.X == fontSize;

    mLpConsoleCurrentFontEx->dwFontSize.X = fontSize;
    mLpConsoleCurrentFontEx->dwFontSize.Y = fontSize;
    if (!SetCurrentConsoleFontEx(mConsoleOut, FALSE, mLpConsoleCurrentFontEx))
    {
        Dout() << "SetCurrentConsoleFontEx failed with error " << GetLastError() << std::endl;
    }
    free(mLpConsoleCurrentFontEx);
}

void BaseApp::SetChar(COORD coord, CellStyle cellStyle)
{
	mChiBuffer[coord.Y*(SIZE_X + 1) + coord.X].Char.UnicodeChar = cellStyle.mSymbol;
    mChiBuffer[coord.Y*(SIZE_X + 1) + coord.X].Attributes = cellStyle.mAttributes;
}

wchar_t BaseApp::GetChar(COORD coord)
{
	return mChiBuffer[coord.Y*(SIZE_X + 1) + coord.X].Char.UnicodeChar;
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

        deltaTime = 0;
        while (deltaTime < 0.0001) {
            deltaTime += timer.Now();
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
