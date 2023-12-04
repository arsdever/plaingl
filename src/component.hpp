#pragma once

class game_object;

class component
{
public:
    virtual ~component() = 0;
    void set_game_object(game_object* parent);
    game_object* get_game_object() const;

private:
    game_object* _game_object;
};
