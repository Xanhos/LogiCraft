#include "SFML_ENGINE/WindowManager.h"
#include "SFML_ENGINE/RessourcesManager.h"

class State
{
protected:
    std::list<std::unique_ptr<State>>* m_listState;
    WindowManager& m_windowManager;
    bool m_isReady;
    bool m_initIsStarted;
    bool m_needToBeDeleted;
    std::string m_name;
public:
    State(WindowManager& _window, std::list<std::unique_ptr<State>>* listState);
    virtual ~State() {};

    template <typename T>
    static std::unique_ptr<T> Create(WindowManager& _window, std::list<std::unique_ptr<State>>* listState)
    {
		return std::make_unique<T>(_window, listState);
	}

    bool getIsReady() { return m_isReady; }
    bool& getNeedToBeDeleted() { return m_needToBeDeleted; }
    bool& getIsStarted() { return m_initIsStarted; }
    void virtual init() = 0;
    void virtual updateEvent(sf::Event& _event) = 0;
    void virtual update() = 0;
    void virtual render() = 0;
    void virtual pushState(char data) = 0;
    std::string virtual getName() { return m_name; }
};

typedef std::list<std::unique_ptr<State>> StateList;