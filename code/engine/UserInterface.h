#pragma once

#include "system.h"
#include <string>

#define UI CEngine::the()->GetUI()

class CUserInterface
{
public:
    CUserInterface();
    BOOL Release(VOID);
    VOID Update(FLOAT dt);
    VOID Render(VOID);
    LRESULT ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    VOID ClearErrorWindow();
    VOID PushErrorMessage(LPCSTR err);
private:
    VOID DebugPanel(VOID);

#if _DEBUG
    // Error handling
    BOOL mShowError;
    std::string mErrorMessage;
#endif
};
