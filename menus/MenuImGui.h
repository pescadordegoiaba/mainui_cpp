#ifndef MENU_IMGUI_H
#define MENU_IMGUI_H

class CMenuItemsHolder;

void MenuImGui_Mouse( int x, int y );
void MenuImGui_Button( int down );
bool MenuImGui_Draw( CMenuItemsHolder *holder );
void MenuImGui_EndPass( void );

#endif
