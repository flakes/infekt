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
#include "plugin_manager.h"
#include "app.h"

using namespace std;


long CPluginManager::PluginToCoreCallback(const char* szGuid, long lCall, long long lParam, void* pParam, void* pUser)
{
	switch(lCall)
	{
	case IPCI_GET_LOADED_NFO_TEXTWIDE:
	case IPCI_GET_LOADED_NFO_TEXTUTF8:
		return DoGetLoadedNfoText(static_cast<long>(lParam), pParam, (lCall != IPCI_GET_LOADED_NFO_TEXTWIDE));

	case IPCI_ENUM_LOADED_NFO_LINKS:
		return DoEnumLoadedNfoLinks(pParam, pUser);

	case IPCI_REGISTER_NFO_LOAD_EVENTS:
		return DoRegister(szGuid, false, REG_NFO_LOAD_EVENTS, pParam, pUser);
	case IPCI_UNREGISTER_NFO_LOAD_EVENTS:
		return DoRegister(szGuid, true, REG_NFO_LOAD_EVENTS, pParam, pUser);

	case IPCI_REGISTER_NFO_VIEW_EVENTS:
		return DoRegister(szGuid, false, REG_NFO_VIEW_EVENTS, pParam, pUser);
	case IPCI_UNREGISTER_NFO_VIEW_EVENTS:
		return DoRegister(szGuid, true, REG_NFO_VIEW_EVENTS, pParam, pUser);

	case IPCI_REGISTER_SETTINGS_EVENTS:
		return DoRegister(szGuid, false, REG_SETTINGS_EVENTS, pParam, pUser);
	case IPCI_UNREGISTER_SETTINGS_EVENTS:
		return DoRegister(szGuid, true, REG_SETTINGS_EVENTS, pParam, pUser);

	case IPCI_HTTP_REQUEST:
		return DoHttpRequest(szGuid, static_cast<infekt_http_request_t*>(pParam), pUser);
	}

	return IPE_NOT_IMPLEMENTED;
}


long CPluginManager::DoGetLoadedNfoText(long a_bufLen, void* a_buf, bool a_utf8)
{
	PNFOData l_nfoData = GetAppView()->GetNfoData();

	if(!l_nfoData || !l_nfoData->HasData())
	{
		return IPE_NO_FILE;
	}

	size_t l_bufSize = (a_utf8 ?
		l_nfoData->GetTextUtf8().size() + 1 :
		l_nfoData->GetTextWide().size() + 1);

	if(l_bufSize > (size_t)std::numeric_limits<long>::max())
	{
		return IPE_TOO_LARGE;
	}

	if(!a_buf || !a_bufLen)
	{
		// return required buffer size
		// (UTF-8: in bytes, otherwise: in characters)

		return static_cast<long>(l_bufSize);
	}
	else
	{
		// copy shit to buffer

		if(a_bufLen < static_cast<long>(l_bufSize))
		{
			return IPE_BUF_TOO_SMALL;
		}

		if(a_utf8)
		{
			strncpy_s(static_cast<char*>(a_buf), static_cast<size_t>(a_bufLen),
				l_nfoData->GetTextUtf8().c_str(), l_nfoData->GetTextUtf8().size());
		}
		else
		{
			wcsncpy_s(static_cast<wchar_t*>(a_buf), static_cast<size_t>(a_bufLen),
				l_nfoData->GetTextWide().c_str(), l_nfoData->GetTextWide().size());
		}

		return IPE_SUCCESS;
	}
}


long CPluginManager::DoEnumLoadedNfoLinks(void* a_pCallback, void* a_pUser)
{
	PNFOData l_nfoData = GetAppView()->GetNfoData();

	if(!l_nfoData || !l_nfoData->HasData())
	{
		return IPE_NO_FILE;
	}

	infektPluginMethod l_callback = (infektPluginMethod)a_pCallback;

	if(!l_callback)
	{
		return IPE_NULLCALLBACK;
	}

	long l_count = 0;

	l_callback(NULL, 0, IPV_ENUM_BEGIN, l_count, NULL, a_pUser);

	while(const CNFOHyperLink* l_link = l_nfoData->GetLinkByIndex(l_count))
	{
		infektDeclareStruct(infekt_nfo_link_t, l_linkInfo);

		l_linkInfo.colEnd = l_link->GetColEnd();
		l_linkInfo.colStart = l_link->GetColStart();
		l_linkInfo.href = l_link->GetHref().c_str();
		l_linkInfo.linkId = l_link->GetLinkID();
		l_linkInfo.row = l_link->GetRow();

		if(l_callback(NULL, 0, IPV_ENUM_ITEM, l_count, &l_linkInfo, a_pUser) == IPE_STOP)
		{
			break;
		}

		l_count++;
	}

	l_callback(NULL, 0, IPV_ENUM_END, l_count, NULL, a_pUser);

	return l_count;
}


long CPluginManager::DoRegister(const std::string& a_guid, bool a_unregister, EPluginReg a_regType, void* a_pParam, void* a_userData)
{
	TMGuidPlugins::iterator l_find = m_loadedPlugins.find(a_guid);
	if(l_find == m_loadedPlugins.end()) return IPE_NO_FILE;
	PLoadedPlugin l_plugin = l_find->second;

	infektPluginMethod l_callback = NULL;
	switch(a_regType)
	{
	default:
		l_callback = reinterpret_cast<infektPluginMethod>(a_pParam);
	}

	if(a_unregister)
	{
		return l_plugin->RemoveReg(a_regType, l_callback);
	}
	else
	{
		return l_plugin->AddReg(a_regType, l_callback, a_userData);
	}
}


static void _DoHttpRequest_Callback(PWinHttpRequest a_req, infektPluginMethod a_callback, void* a_pUser)
{
	_ASSERTE(a_callback);


	infektDeclareStruct(infekt_http_result_t, l_res);
	l_res.requestId = a_req->GetReqId();

	if(a_req->DidDownloadSucceed())
	{
		if(!a_req->GetDownloadFilePath().empty())
		{
			l_res.downloadFileName = a_req->GetDownloadFilePath().c_str();
		}
		else
		{
			l_res.textBuffer = a_req->GetBufferContents().c_str();
		}

		l_res.success = true;
	}

	a_callback("", 0, IPV_HTTP_RESULT, a_req->GetStatusCode(), &l_res, a_pUser);
}

long CPluginManager::DoHttpRequest(const std::string& a_guid, const infekt_http_request_t* a_pReq, void* a_pUser)
{
	TMGuidPlugins::iterator l_find = m_loadedPlugins.find(a_guid);
	if(l_find == m_loadedPlugins.end()) return IPE_NO_FILE;
	PLoadedPlugin l_plugin = l_find->second;

	if(!a_pReq || !a_pReq->url)
	{
		return IPE_INVALIDPARAM;
	}
	else if(!a_pReq->callback)
	{
		return IPE_NULLCALLBACK;
	}

	PWinHttpRequest l_req = l_plugin->GetHttpClient()->CreateRequest();

	if(a_pReq->downloadToFileName)
	{
		l_req->SetDownloadFilePath(a_pReq->downloadToFileName);
	}

	l_req->SetBypassCache(a_pReq->bypassCache);

	l_req->SetCallback(boost::bind(&_DoHttpRequest_Callback, _1, a_pReq->callback, a_pUser));

	if(l_plugin->GetHttpClient()->StartRequest(l_req))
	{
		return l_req->GetReqId();
	}
	else
	{
		return IPE_INTERNAL_PROBLEM;
	}
}
