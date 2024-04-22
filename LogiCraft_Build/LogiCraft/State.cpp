#include "State.h"
#include "SFML_ENGINE/RessourcesManager.h"


State::State(WindowManager& _window, std::list<std::unique_ptr<State>>* stackState) : m_windowManager(_window), m_listState(stackState), m_isReady(false), m_initIsStarted(false), m_needToBeDeleted(false), m_name("")
{
    static bool firstTime = true;
    if (firstTime)
    {
        m_windowManager.setSFX_Volume(50);
        m_windowManager.setMusic_Volume(50);
        m_windowManager.setMovie_Volume(50);
        firstTime = false;
    }
}