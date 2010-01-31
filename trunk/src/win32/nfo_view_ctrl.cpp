/**
 * Copyright (C) 2010 cxxjoe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 **/

#include "stdafx.h"
#include "nfo_view_ctrl.h"

#define NFOVWR_CTRL_CLASS_NAME _T("NfoViewCtrl")


CNFOViewControl::CNFOViewControl(HINSTANCE a_hInstance, HWND a_parent) : CNFORenderer()
{
	m_instance = a_hInstance;
	m_parent = a_parent;
	m_left = m_top = m_width = m_height = 0;
	m_hwnd = 0;
	m_scrollH = m_scrollV = 0;
}


bool CNFOViewControl::CreateControl(int a_left, int a_top, int a_width, int a_height)
{
	WNDCLASSEX l_class = {0};
	l_class.cbSize = sizeof(WNDCLASSEX);

	l_class.hInstance = m_instance;
	l_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	l_class.lpszClassName = NFOVWR_CTRL_CLASS_NAME;
	l_class.lpfnWndProc = &_WindowProc;
	l_class.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));

	if(RegisterClassEx(&l_class) == 0)
	{
		return false;
	}

	m_top = a_top;
	m_left = a_left;
	m_width = a_width;
	m_height = a_height;

	m_hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
		NFOVWR_CTRL_CLASS_NAME, NULL,
		WS_CHILD | WS_BORDER | WS_VISIBLE,
		m_left, m_top, m_width, m_height,
		m_parent, NULL,
		m_instance, NULL);

	if(!m_hwnd)
	{
		return false;
	}

	SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (INT_PTR)this);

	m_scrollH = CreateWindowEx(0,
		_T("SCROLLBAR"), NULL,
		WS_CHILD | SBS_HORZ | SBS_BOTTOMALIGN,
		0, 0, m_width, CW_USEDEFAULT,
		m_hwnd, NULL,
		m_instance, NULL);

	SCROLLINFO l_si = {0};
	l_si.cbSize = sizeof(SCROLLINFO);

	l_si.fMask  = SIF_RANGE;
	l_si.nMin = 0;
	l_si.nMax = GetWidth();
	SetScrollInfo(m_hwnd, SB_HORZ, &l_si, FALSE); 

	m_scrollV = CreateWindowEx(0,
		_T("SCROLLBAR"), NULL,
		WS_CHILD | SBS_VERT | SBS_RIGHTALIGN,
		0, 0, CW_USEDEFAULT, m_height,
		m_hwnd, NULL,
		m_instance, NULL);

	l_si.fMask  = SIF_RANGE;
	l_si.nMin = 0;
	l_si.nMax = GetHeight();
	SetScrollInfo(m_hwnd, SB_VERT, &l_si, FALSE); 

	return true;
}


LRESULT CNFOViewControl::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC dc;

			dc = BeginPaint(m_hwnd, &ps);
			
			cairo_surface_t *surface = cairo_win32_surface_create (dc);
			DrawToSurface(surface, 0, 0, 0, 0, m_width, m_height);
			cairo_surface_destroy (surface);

			EndPaint(m_hwnd, &ps);
		}
	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
}


LRESULT CALLBACK CNFOViewControl::_WindowProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CNFOViewControl *l_ctrl = (CNFOViewControl*)(void*)(INT_PTR)GetWindowLongPtr(hWindow, GWLP_USERDATA);

	if(l_ctrl)
	{
		return l_ctrl->WindowProc(uMsg, wParam, lParam);
	}

	return DefWindowProc(hWindow, uMsg, wParam, lParam);
}


CNFOViewControl::~CNFOViewControl()
{
	UnregisterClass(NFOVWR_CTRL_CLASS_NAME, m_instance);

	if(m_hwnd)
	{
		DestroyWindow(m_hwnd);
		// MSDN: "If the specified window is a parent window, DestroyWindow
		// automatically destroys the associated child windows."
	}
}
