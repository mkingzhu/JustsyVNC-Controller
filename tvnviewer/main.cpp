// Copyright (C) 2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the TightVNC software.  Please visit our Web site:
//
//                       http://www.tightvnc.com/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#include "client-config-lib/ViewerConfig.h"
#include "log-writer/LogWriter.h"
#include "TvnViewer.h"
#include "ConnectionData.h"
#include "ConnectionListener.h"
#include "util/ResourceLoader.h"
#include "util/StringStorage.h"
#include "win-system/WinProcessCommandLine.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE,
                       LPTSTR lpCmdLine, int nCmdShow)
{
  WinProcessCommandLine m_wpcl;
  if (4 != m_wpcl.getArgumentsCount())
    return 0;

  bool isOK = true;
  StringStorage strUser;
  isOK &= m_wpcl.getArgument(1, &strUser);
  StringStorage strDeviceId;
  isOK &= m_wpcl.getArgument(2, &strDeviceId);
  StringStorage strMagic;
  isOK &= m_wpcl.getArgument(3, &strMagic);

  if (!isOK)
    return 0;

  ViewerSettingsManager::initInstance(RegistryPaths::VIEWER_PATH);
  SettingsManager *sm = ViewerSettingsManager::getInstance();

  ViewerConfig config(RegistryPaths::VIEWER_PATH);
  config.loadFromStorage(sm);

  try {
    config.initLog(LogNames::LOG_DIR_NAME, LogNames::VIEWER_LOG_FILE_STUB_NAME);
  } catch (...) {
  }

  LogWriter logWriter(config.getLogger());

  // resource-loader initialization
  ResourceLoader resourceLoader(hInstance);

  logWriter.debug(_T("main()"));
  logWriter.debug(_T("loading settings from storage completed"));
  logWriter.debug(_T("Log initialization completed"));

  int result = 0;
  try {
    TvnViewer tvnViewer(hInstance,
                        ApplicationNames::WINDOW_CLASS_NAME,
                        WindowNames::TVN_WINDOW_CLASS_NAME);
    tvnViewer.setUser(strUser);
    tvnViewer.setDeviceId(strDeviceId);
    tvnViewer.setMagic(strMagic);
    tvnViewer.showLoginDialog();
    result = tvnViewer.run();
  } catch (const Exception &ex) {
    MessageBox(0,
               StringTable::getString(IDS_UNKNOWN_ERROR_IN_VIEWER),
               ProductNames::VIEWER_PRODUCT_NAME,
               MB_OK | MB_ICONERROR);
    logWriter.debug(ex.getMessage());
  }

  return result;
}
