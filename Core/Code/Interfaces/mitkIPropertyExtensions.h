/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef mitkIPropertyExtensions_h
#define mitkIPropertyExtensions_h

#include <usServiceInterface.h>
#include <string>
#include <MitkExports.h>

namespace mitk
{
  class PropertyExtension;

  /** \brief Interface of property extensions service.
    *
    * This service allows you to manage extensions for properties.
    * An extension is a class that derives from mitk::PropertyExtension.
    * Use extensions to attach useful metadata to your properties, e.g. the allowed range of values.
    * Note that you have to extend the property view if you want it to respect your custom metadata.
    */
  class MITK_CORE_EXPORT IPropertyExtensions
  {
  public:
    virtual ~IPropertyExtensions();

    /** \brief Add an extension to a specific property.
      *
      * \param[in] propertyName Name of the property.
      * \param[in] extension Property extension.
      * \param[in] overwrite Overwrite already existing property extension.
      * \return True if extension was added successfully.
      */
    virtual bool AddExtension(const std::string& propertyName, PropertyExtension* extension, bool overwrite = false) = 0;

    /** \brief Get the extension of a specific property.
      *
      * \param[in] propertyName Name of the property.
      * \return Property extension or null pointer if no extension was found.
      */
    virtual PropertyExtension* GetExtension(const std::string& propertyName) const = 0;

    /** \brief Check if a specific property has an extension.
      *
      * \param[in] propertyName Name of the property.
      * \return True if the property has an extension, false otherwise.
      */
    virtual bool HasExtension(const std::string& propertyName) const = 0;

    /** \brief Remove all property extensions.
      */
    virtual void RemoveAllExtensions() = 0;

    /** \brief Remove extension of a specific property.
      *
      * \param[in] propertyName Name of the property.
      */
    virtual void RemoveExtension(const std::string& propertyName) = 0;
  };
}

US_DECLARE_SERVICE_INTERFACE(mitk::IPropertyExtensions, "org.mitk.IPropertyExtensions")

#endif