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

#ifndef _NFO_VIEWCTRL_H
#define _NFO_VIEWCTRL_H

#include "nfo_renderer.h"


class CNFOViewControl : public CNFORenderer
{
protected:
	HINSTANCE m_instance;
	HWND m_parent;
	int m_left, m_top;
	int m_width, m_height;
	HWND m_hwnd;
	bool m_centerNfo;
	bool m_copyOnSelect;
	std::wstring m_lastFindTerm;
	size_t m_findPosGlobalRow, m_findPosGlobalCol;

	LPTSTR m_cursor;
	size_t m_selStartRow, m_selStartCol;
	size_t m_selEndRow, m_selEndCol;
	bool m_leftMouseDown, m_movedDownMouse;

	HMENU m_contextMenuHandle;
	HWND m_contextMenuCommandTarget;
	const CNFOHyperLink* m_linkUnderMenu;

	void UpdateScrollbars(bool a_resetPos);
	bool HandleScrollEvent(int a_dir, int a_event, int a_change);
	void GetScrollPositions(int& ar_x, int& ar_y);
	void CalcFromMouseCoords(int a_x, int a_y, ssize_t& ar_row, ssize_t& ar_col);
	bool ScrollIntoView(size_t a_row, size_t a_col);

	void OnPaint();
	void OnMouseMove(int a_x, int a_y);
	void OnMouseClickEvent(UINT a_event, int a_x, int a_y);

	static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT WindowProc(UINT, WPARAM, LPARAM);
public:
	CNFOViewControl(HINSTANCE a_hInstance, HWND a_parent, bool a_classic = false);
	virtual ~CNFOViewControl();

	virtual bool AssignNFO(const PNFOData& a_nfo);
	bool CreateControl(int a_left, int a_top, int a_width, int a_height);
	void SetContextMenu(HMENU a_menuHandle, HWND a_target);
	HWND GetHwnd() const { return m_hwnd; }
	HWND GetParent() const { return m_parent; }
	void SetParent(HWND a_new);

	const std::wstring GetSelectedText() const;
	void CopySelectedTextToClipboard() const;
	void SelectAll();
	void Show(bool a_show = true);
	bool ControlCreated() const { return (m_hwnd != 0); }
	void SetCenterNfo(bool nb);
	void SetCopyOnSelect(bool nb) { m_copyOnSelect = nb; }

	void SetZoom(unsigned int a_percent);
	void ZoomIn();
	void ZoomOut();
	void ZoomReset();

	bool FindTermDown(const std::wstring& a_term, size_t& a_startRow, size_t& a_startCol);
	bool FindTermUp(const std::wstring& a_term, size_t& a_startRow, size_t& a_startCol);
	bool FindAndSelectTerm(const std::wstring& a_term, bool a_up);

	void SetOnDemandRendering(bool nb) { m_onDemandRendering = nb; }
	bool GetOnDemandRendering() const { return m_onDemandRendering; }

	virtual void InjectSettings(const CNFORenderSettings& ns);
};


typedef std::shared_ptr<CNFOViewControl> PNFOViewControl;


#ifndef WM_MOUSEHWHEEL
// Windows Vista & higher only...
#define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef SPI_GETWHEELSCROLLCHARS
#define SPI_GETWHEELSCROLLCHARS 0x006C
#endif


#endif /* !_NFO_VIEWCTRL_H */
