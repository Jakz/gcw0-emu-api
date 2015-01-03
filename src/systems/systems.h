#ifndef __SYSTEMS_H__
#define __SYSTEMS_H__

#include <string>
#include <vector>

#include <cstdint>


namespace gcw
{
  class System
  {
    
  public:
    enum class Type : uint16_t
    {
      UNCATEGORISED,
      
      GAME_BOY,
      GAME_BOY_ADVANCE,
      SUPER_NINTENDO,
      
      GAME_GEAR,
      SEGA_MASTER_SYSTEM
    };
    
    struct Spec
    {
      Type system;
      std::string ident;
      std::string name;
      std::vector<std::string> extensions;
      
      Spec(Type system, std::string&& ident, std::string &&name, std::vector<std::string> &&extensions) : system(system), ident(ident), name(name), extensions(extensions) { }
    };
    
    
    
  private:
    static std::vector<Spec> specs;
    
  public:
    static const std::vector<Spec>& getSystems() { return specs; }
    static std::vector<Spec>::const_iterator getIterator() { return specs.begin(); }
    static bool hasNext(std::vector<Spec>::const_iterator& it) { return it != specs.end(); }
  };
}

#endif