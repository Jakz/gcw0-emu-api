#include "menu_view.h"

#include "../gfx/menu.h"
#include "../common/defines.h"
#include "../core/manager.h"

#include <vector>

using namespace std;
using namespace gcw;

u32 MenuView::MenuEntryList::current() const { return currentStatus.index; }
u32 MenuView::MenuEntryList::count() const { return static_cast<u32>(currentStatus.menu->count()); }
void MenuView::MenuEntryList::set(u32 i) { currentStatus.index = i; }
MenuEntry* MenuView::MenuEntryList::selected() { return currentStatus.menu->entryAt(currentStatus.index); }
MenuEntry* MenuView::MenuEntryList::get(u32 i) { return currentStatus.menu->entryAt(offset+i); }


MenuView::MenuView(Manager *manager, std::unique_ptr<Menu>& root) : View(manager), root(root)
{

}

void MenuView::handleEvents()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      
      // manage normal keys
      case SDL_KEYDOWN:
      {
        GCWKey key = static_cast<GCWKey>(event.key.keysym.sym);
        switch (key)
        {
          case GCW_KEY_DOWN: list.down(); break;
          case GCW_KEY_UP: list.up(); break;
            
          case GCW_KEY_L: list.prevPage(); break;
          case GCW_KEY_R: list.nextPage(); break;
            
          case MENU_BACK_BUTTON:
          {
            if (!menuStack.empty()) { list.setStatus(menuStack.top()); menuStack.pop(); }
            else backActionOnRoot();
            
            break;
          }
          case GCW_KEY_RIGHT:
          case GCW_KEY_LEFT:
          case MENU_ACTION_BUTTON:
            list.selected()->doAction(manager, key); break;
            
          default: break;
        }
      }
    }
    
    manager->getHelp()->set(list.selected()->help());
  }
}

void MenuView::enterSubmenu(SubMenuEntry *entry)
{
  menuStack.push(list.status());
  list.setStatus(MenuStatus(entry->subMenu()));
}


void MenuView::render()
{
  //gfx->blit(gfx->cache.get("data/syrstems/gba-small.png"), 100, 100);
  
  list.status().menu->render(gfx, list.minOffset(), list.getDisplayedAmount(), UI::TITLE_OFFSET.x, UI::TITLE_OFFSET.y, UI::MENU_OFFSET.x, UI::MENU_OFFSET.y, list.current());
  manager->getHelp()->render();
}