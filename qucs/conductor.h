
#ifndef CONDUCTOR_H
#define CONDUCTOR_H

#include "element.h"
#include "wirelabel.h"


/** \class Conductor
  * \brief label for Node and Wire classes
  *
  */
class Conductor : public Element {
  std::unique_ptr<WireLabel> m_label;
public:
  bool hasLabel() const
  {
    return m_label != nullptr;
  }

  void dropLabel()
  {
    m_label.reset();
  }

  std::unique_ptr<WireLabel> releaseLabel()
  {
    if (hasLabel()) {
      m_label->setOwner(nullptr);
      return std::move(m_label);
    }
    return nullptr;
  }

  void acquireLabel(std::unique_ptr<WireLabel>&& new_label)
  {
    if (new_label != nullptr) {
      assert(new_label->owner() == nullptr);
      new_label->setOwner(this);
    }
    m_label = std::move(new_label);
  }

  void acquireLabel(WireLabel* new_label)
  {
    if (new_label != nullptr) {
      assert(new_label->owner() == nullptr);
      new_label->setOwner(this);
    }
    m_label.reset(new_label);
  }

  WireLabel* label() const
  {
    return m_label.get();
  }
};

#endif
