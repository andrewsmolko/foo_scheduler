#pragma once

class PlayerEvent;
class DateTimeEvent;
class MenuItemEvent;

class IEventVisitor
{
public:
    ~IEventVisitor() {}

    virtual void Visit(PlayerEvent& event) = 0;
    virtual void Visit(DateTimeEvent& event) = 0;
    virtual void Visit(MenuItemEvent& event) = 0;
};