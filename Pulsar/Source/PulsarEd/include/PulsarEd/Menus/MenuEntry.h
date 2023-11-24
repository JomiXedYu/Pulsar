#pragma once
#include <CoreLib/Core.h>
#include <CoreLib/Delegate.h>
#include "MenuContext.h"

namespace pulsared
{
    using MenuCanVisibility = FunctionDelegate<bool, sptr<MenuContexts>>;
    using MenuCanOperate = FunctionDelegate<bool, sptr<MenuContexts>>;
    using MenuAction = FunctionDelegate<void, sptr<MenuContexts>>;
    using MenuCheckAction = FunctionDelegate<void, sptr<MenuContexts>, bool>;

    class MenuEntry : public Object
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsared, pulsared::MenuEntry, Object);
    public:
        int Priority;
        string Name;
        string DisplayName;

        MenuEntry(const string& name) : Name(name), DisplayName(name), Priority(9999) {}
        MenuEntry(const string& name, const string& displayName) :
            Name(name), DisplayName(displayName), Priority(9999) {}

        virtual ~MenuEntry() override {}
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

        sptr<MenuCanVisibility> Visibility;
        sptr<MenuCanOperate> CanOperate;
        sptr<MenuAction> Action;
    };
    CORELIB_DECL_SHORTSPTR(MenuEntryButton);



    class MenuEntryCheck : public MenuEntry
    {
        CORELIB_DEF_TYPE(AssemblyObject_pulsared, pulsared::MenuEntryCheck, MenuEntry);
    public:
        MenuEntryCheck(const string& name, const string& displayName,
            const sptr<MenuCheckAction>& checkedAction, bool isChecked = false)
            : base(name, displayName), IsChecked(isChecked), CheckedAction(checkedAction)
        { }

        bool IsChecked;
        sptr<MenuCheckAction> CheckedAction;
    };
    CORELIB_DECL_SHORTSPTR(MenuEntryCheck);

}