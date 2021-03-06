#ifndef _UTILS_H_
#define _UTILS_H_

#include "defines.h"

#include <dirent.h>

#include <chrono>
#include <unordered_set>
#include <vector>
#include <string>
#ifndef __APPLE__
#include <sstream>
#endif


namespace gcw
{
  typedef unsigned long usec_t;
  
  enum class FMode
  {
    READING,
    WRITING,
    APPENDING
  };
  
  class Path
  {
  private:
    std::string path;
  
    void trimEndSlash() { if (path.back() == '/') path.pop_back(); }
    void trimStartSlash() { if (path.front() == '/') path.erase(1); }
    
  public:
  
    Path() { }
    Path(const char *path);
    Path(const std::string& path);
    Path(const Path& rhs) : path(rhs.path) { }
    Path& operator=(const Path& rhs) { path = rhs.path; return *this; }
    const std::string& value() const { return path; }
    const char* c_str() const { return path.c_str(); }
    void set(std::string& value) { path = value; }
    void set(const Path& opath) { path = opath.path; }
    Path append(std::string component) const;
    Path append(const Path& path) const;
    Path removeLast() const;
    bool isRoot() const;
  
    Path folder() const;
    std::string extension() const;
    std::string plainName() const;
    std::tuple<std::string, std::string> split() const;
  
    
    std::string fileInsidePath(const std::string& file) const;
    std::vector<Path> findFiles(const std::string& ext, bool recursive) const;
    std::vector<Path> findFiles(std::unordered_set<std::string>& exts, bool recursive) const;
    std::vector<Path> subfolders(bool sorted = true) const;
    
    bool operator==(const std::string& rhs) const { return path == rhs; }
    bool operator==(const Path& rhs) const { return path == rhs.path; }
    bool operator==(const char* rhs) const { return path == std::string(rhs); }
    
    time_t timeModified() const;
    FILE* open(FMode mode) const;
    bool exists() const;
    bool existsAsFolder() const;
    
    Path operator+(const Path& rhs) const { return this->append(rhs.path); }
    Path operator+(const std::string& rhs) const { return this->append(rhs); }
    Path operator+(const char* rhs) const { return this->append(std::string(rhs)); }
    
    bool operator<(const Path& rhs) const { return path < rhs.path; }
  };
  
  class Files
  {
  public:
    static std::vector<Path> findFiles(const std::string& path, const std::string& ext, bool recursive);
    static std::vector<Path> findFiles(const std::string& path, std::unordered_set<std::string>& exts, bool recursive);
    static std::vector<Path> findSubfolders(const std::string& path);
    static bool createFolder(const Path& path, bool recursive = true);
  };
  
  class Timer
  {
    const float DEFAULT_FPS = 60.0f;
    
    std::chrono::steady_clock clock;
    std::chrono::microseconds ticksForFrame;
    
    u32 totalFrames;
    std::chrono::time_point<std::chrono::steady_clock> base;
    
    public:
      Timer() : ticksForFrame(static_cast<u32>(1000000 / DEFAULT_FPS)), totalFrames(0) { }
    
      void setFrameRate(float rate);
      void frameRateDelay();
    
      static usec_t getusecs() { return SDL_GetTicks() * usec_t(1000); }
  };
  
  class Text
  {
    public:
      static std::string clipText(const std::string &text, s32 length, const char *filler = nullptr);
    
      static std::string fuzzyTimeInterval(time_t start, time_t end);
      static std::string dateToString(time_t timestamp);
    
      template <typename T> static std::string to_string(T value)
      {
        #ifdef __APPLE__
          return std::to_string(value);
        #else
          std::stringstream ss;
          ss << value;
          return ss.str();
        #endif
      }
  };
}

namespace std {
  template<typename T>
  struct hash<reference_wrapper<T>>
  {
    std::size_t operator()(const std::reference_wrapper<T>& k) const
    {
      return hash<T*>()(&k.get());
    }
  };
}

namespace std {
  template<typename T>
  bool operator==(const std::reference_wrapper<T>& r1, const std::reference_wrapper<T>& r2)
  {
    return &(r1.get()) == &(r2.get());
  }
}

#endif
