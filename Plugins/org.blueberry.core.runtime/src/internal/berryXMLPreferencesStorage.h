/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/
#ifndef BERRYXMLPREFERENCES_H_
#define BERRYXMLPREFERENCES_H_

#include <org_blueberry_core_runtime_Export.h>
#include "berryAbstractPreferencesStorage.h"

#include "Poco/AutoPtr.h"

namespace Poco {
namespace XML {
class Node;
class Document;
}
}

namespace berry
{
  class Preferences;
  ///
  /// Implementation of the OSGI Preferences Interface.
  /// Wraps a DOMNode.
  ///
  class org_blueberry_core_runtime_EXPORT XMLPreferencesStorage: public AbstractPreferencesStorage
  {
  public:
    ///
    /// For use with berry::SmartPtr
    ///
    berryObjectMacro(berry::XMLPreferencesStorage);
    ///
    /// Construct a new XML-based PreferencesStorage
    ///
    XMLPreferencesStorage(const QString& _File);

    ///
    /// To be implemented in the subclasses.
    ///
    void Flush(IPreferences* _Preferences) override;

    ///
    /// Nothing to do here
    ///
    ~XMLPreferencesStorage() override;

  protected:
    ///
    /// Construct Preferences tree from DOM tree (recursive)
    /// Construct a PreferencesNode from the DOMNode by using prefParentNode
    ///
    void ToPreferencesTree(Poco::XML::Node* DOMNode, Preferences* prefParentNode);
    ///
    /// Construct DOM tree from Preferences tree (recursive)
    /// Construct a DOMNode from the prefNode by using parentDOMNode
    ///
    void ToDOMTree(Preferences* prefNode, Poco::XML::Node* parentDOMNode);

  protected:

    Poco::AutoPtr<Poco::XML::Document> m_Document;

  };
}

#endif /* BERRYXMLPREFERENCES_H_ */
