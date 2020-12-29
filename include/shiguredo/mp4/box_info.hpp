#pragma once

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include "shiguredo/mp4/box_type.hpp"

namespace shiguredo::mp4 {

class Box;
class BoxInfo;

using BoxPath = std::vector<BoxType>;

struct BoxInfoParameters {
  BoxInfo* parent = nullptr;
  Box* box;
};

class BoxInfo {
 public:
  explicit BoxInfo(const BoxInfoParameters&);
  ~BoxInfo();
  BoxPath getPath() const;
  Box* getBox() const;
  std::uint64_t getSize() const;
  BoxType getType() const;
  void addLeaf(BoxInfo*);
  std::string toString() const;
  std::uint64_t adjustOffsetAndSize(const std::uint64_t);
  void write(std::ostream&) const;

 private:
  BoxPath m_path;
  Box* m_box;
  std::vector<BoxInfo*> m_leafs;
};

}  // namespace shiguredo::mp4
