#pragma once
#include <CoreLib/Core.h>
#include <CoreLib/Delegate.h>

#include <utility>
#include "MenuContext.h"

namespace pulsared
{
    using MenuCanVisibility = FunctionDelegate<bool, SPtr<MenuContexts>>;
    using MenuCanOperate = FunctionDelegate<bool, SPtr<MenuContexts>>;
    using MenuAction = FunctionDelegate<void, SPtr<MenuContexts>>;
    using MenuCheckAction = FunctionDelegate<void, SPtr<MenuContexts>, bool>;
    using MenuGetCheckedAction = FunctionDelegate<bool, SPtr<MenuContexts>>;

    class MenuEntry : public Object
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsared, pulsared::MenuEntry, Object);
    public:
        string Name;
        string DisplayName;
        int Priority;

        SPtr<MenuCanOperate> CanOperate;

        explicit MenuEntry(const string& name) : Name(name), DisplayName(name), Priority(9999) {}
        MenuEntry(string name, string displayName) :
            Name(std::move(name)), DisplayName(std::move(displayName)), Priority(9999) {}

        ~MenuEntry() override = default;
    };
    CORELIB_DECL_SHORTSPTR(MenuEntry);

    class MenuEntrySeparate : public MenuEntry
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsared, pulsared::MenuEntrySeparate, MenuEntry);
    public:
        using base::base;
    };
    CORELIB_DECL_SHORTSPTR(MenuEntrySeparate);


    class MenuEntryButton : public MenuEntry
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsared, pulsared::MenuEntryButton, MenuEntry);
    public:
        using base::base;

        SPtr<MenuCanVisibility> Visibility;
        SPtr<MenuAction> Action;
    };
    CORELIB_DECL_SHORTSPTR(MenuEntryButton);



    class MenuEntryCheck : public MenuEntry
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsared, pulsared::MenuEntryCheck, MenuEntry);
    public:
        MenuEntryCheck(const string& name, const string& displayName,
            const SPtr<MenuCheckAction>& checkedAction, bool isChecked = false)
            : base(name, displayName), IsChecked(isChecked), CheckedAction(checkedAction)
        { }
        MenuEntryCheck(const string& name, const string& displayName,
            const SPtr<MenuCheckAction>& checkedAction, const SPtr<MenuGetCheckedAction>& getCheckedAction)
                : base(name, displayName), CheckedAction(checkedAction), GetCheckedAction(getCheckedAction)
        {

        }

        bool IsChecked{};
        SPtr<MenuGetCheckedAction> GetCheckedAction;
        SPtr<MenuCheckAction> CheckedAction;
    };
    CORELIB_DECL_SHORTSPTR(MenuEntryCheck);

}