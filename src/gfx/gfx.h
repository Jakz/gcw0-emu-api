#ifndef _GFX_H_
#define _GFX_H_

#include "../common/defines.h"
#include "../common/utils.h"

#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>

#define WIDTH (320)
#define HEIGHT (240)



namespace gcw
{
  struct Font;
  
  class ImageCache
  {
    private:
      std::unordered_map<std::string, SDL_Surface*> cache;
      
      SDL_Surface* load(std::string name)
      {
        SDL_Surface *image = IMG_Load(name.c_str());
        return image;
      }
      
    public:
      ImageCache() { }
    
      SDL_Surface *getFallback(const std::string& name, std::string fallback)
      {
        SDL_Surface *icon = get(name);
        
        if (!icon)
          return get(fallback);
        else
          return icon;
      }
    
      SDL_Surface *get(std::string name) {
        std::unordered_map<std::string, SDL_Surface*>::iterator it = cache.find(name);
        if (it != cache.end())
          return it->second;
        else
        {
          SDL_Surface *image = load(name);
          cache[name] = image;
          return image;
        }
      }
      
      ~ImageCache()
      {
        for (auto &pair : cache)
          SDL_FreeSurface(pair.second);
      }
  };
  
  class Gfx
  {
    private:
      static SDL_PixelFormat *format;
    

    
    public:
      SDL_Surface *screen;

      void init();
      inline void flip() { SDL_Flip(screen); }
    

      static inline SDL_Rect rrr(s16 x, s16 y, u16 w, u16 h) { return {x,y,w,h}; }
    
      template<typename T>
      static inline T ccc(u8 r, u8 b, u8 g) { return static_cast<T>(SDL_MapRGB(format, r, g, b)); }
  
      template<typename T>
      void clear(T color) {
        SDL_Rect rect = {0,0,static_cast<Uint16>(screen->w),static_cast<u16>(screen->h)};
        SDL_FillRect(screen, &rect, color);
      }
    
      template<typename T>
      void rawBlit(SDL_Surface *dest, const GfxBuffer &buffer, const Offset &offset);
    
      template<typename T>
      void line(u16 x1, u16 y1, u16 x2, u16 y2, T color);
      template<typename T>
      void rect(u16 x, u16 y, u16 w, u16 h, T color);
      template<typename T>
      void rectFill(s16 x1, s16 y1, u16 w, u16 h, T color);
    
      template<typename T>
      u16 print(int x, int y, bool centered, const Font &font, const T color, const char *text) const;
    
    
      u16 print(int x, int y, bool centered, const Font &font, const char *text) const;
      u16 printf(int x, int y, bool centered, const Font &font, const char *text, ...) const;
    
      void blit(SDL_Surface *src, int x, int y)
      {
        SDL_Rect srcRect = rrr(0,0,src->w,src->h);
        SDL_Rect dstRect = rrr(x,y,0,0);
        
        SDL_BlitSurface(src, &srcRect, screen, &dstRect);
      }
    
      void blitCentered(SDL_Surface *src, int x, int y)
      {
        blit(src, x-src->w/2, y-src->h/2);
      }
    
      const SDL_PixelFormat* getFormat() const { return format; }
    
      template<typename T>
      static void clear(GfxBuffer &buffer, T color);
    
      static ImageCache cache;
  
  };
  
  struct Font
  {
    protected:
      SDL_Surface *image;
      u8 widths[256];
      u8 tileWidth;
      u8 tileHeight;
      u8 lineHeight;
    
    public:
      Font(std::string filename, u8 defaultWidth, u8 tileWidth, u8 tileHeight, u8 lineHeight, std::map<std::string,u8> customWidths);
    
      u16 stringWidth(const char *text) const
      {
        u16 length = strlen(text), width = 0;
        for (int i = 0; i < length; ++i) width += widths[static_cast<u8>(text[i])];
        return width;
      }
    
      friend class Gfx;
    
      static const Font bigFont;
  };
  
  
  class Blitter
  {
  protected:
    Gfx* gfx;
    
  public:
    Blitter(Gfx* gfx) : gfx(gfx) { }
    
    virtual void blit(const GfxBuffer& buffer, const Offset& offset, SDL_Surface* dest) = 0;
    virtual ~Blitter() { }
  };
  
  class Blitter565to565 : public Blitter
  {
  public:
    Blitter565to565(Gfx* gfx) : Blitter(gfx) { }
    
    void blit(const GfxBuffer& buffer, const Offset& offset, SDL_Surface* dest) override
    {
      gfx->rawBlit<u16>(gfx->screen, buffer, offset);
    }
  };
  
  class Blitter888to888 : public Blitter
  {
  public:
    Blitter888to888(Gfx* gfx) : Blitter(gfx) { }
    
    void blit(const GfxBuffer& buffer, const Offset& offset, SDL_Surface* dest) override
    {      
      gfx->rawBlit<u32>(gfx->screen, buffer, offset);
    }
  };
  
  class Blitter888to565 : public Blitter
  {
  public:
    Blitter888to565(Gfx* gfx) : Blitter(gfx) { }
    
    void blit(const GfxBuffer& buffer, const Offset& offset, SDL_Surface* dest) override
    {
      SDL_LockSurface(dest);
      u16* d = reinterpret_cast<u16*>(dest->pixels) + offset.x + offset.y*dest->w;
      u32* s = reinterpret_cast<u32*>(buffer.data);
      
      for (int y = 0; y < buffer.height; ++y)
      {
        u16* bd = d + y*dest->w;
        u32* bs = s + y*buffer.width;
        
        for (int x = 0; x < buffer.width; ++x, ++bd, ++bs)
        {
          u32 sc = *bs;
          
          u8 r = (sc >> 16) / 8;
          u8 g = ((sc >> 8) & 0xFF) / 4;
          u8 b = ((sc) & 0xFF) / 8;
          
          *bd = (r << 11) | (g << 5) | b;
        }
      }
      
      SDL_UnlockSurface(dest);

    }
  };
}



#endif
