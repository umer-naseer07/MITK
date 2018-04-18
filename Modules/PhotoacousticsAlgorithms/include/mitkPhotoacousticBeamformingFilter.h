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

#ifndef MITK_PHOTOACOUSTICS_BEAMFORMING_FILTER
#define MITK_PHOTOACOUSTICS_BEAMFORMING_FILTER

#include "mitkImageToImageFilter.h"
#include <functional>
#include "./OpenCLFilter/mitkPhotoacousticOCLBeamformingFilter.h"
#include "mitkPhotoacousticBeamformingSettings.h"
#include "mitkPhotoacousticBeamformingUtils.h"

namespace mitk {
  /*!
  * \brief Class implementing an mitk::ImageToImageFilter for beamforming on both CPU and GPU
  *
  *  The class must be given a configuration class instance of mitk::BeamformingSettings for beamforming parameters through mitk::BeamformingFilter::Configure(BeamformingSettings settings)
  *  Whether the GPU is used can be set in the configuration.
  *  For significant problems or important messages a string is written, which can be accessed via GetMessageString().
  */

  class BeamformingFilter : public ImageToImageFilter
  {
  public:
    mitkClassMacro(BeamformingFilter, ImageToImageFilter);

    itkFactorylessNewMacro(Self)
      itkCloneMacro(Self)

      /** \brief Sets a new configuration to use
      *
      * @param settings The configuration set to use for beamforming
      */
      void Configure(BeamformingSettings::Pointer settings)
    {
      MITK_INFO << "Configuring Beamforming Settings";
      m_Conf = settings;

      switch (m_Conf->GetApod())
      {
      case BeamformingSettings::Apodization::Hann:
        m_Conf->SetApodizationFunction(mitk::PhotoacousticBeamformingUtils::VonHannFunction(m_Conf->GetApodizationArraySize()));
        break;
      case BeamformingSettings::Apodization::Hamm:
        m_Conf->SetApodizationFunction(mitk::PhotoacousticBeamformingUtils::HammFunction(m_Conf->GetApodizationArraySize()));
        break;
      case BeamformingSettings::Apodization::Box:
        m_Conf->SetApodizationFunction(mitk::PhotoacousticBeamformingUtils::BoxFunction(m_Conf->GetApodizationArraySize()));
        break;
      default:
        m_Conf->SetApodizationFunction(mitk::PhotoacousticBeamformingUtils::BoxFunction(m_Conf->GetApodizationArraySize()));
        break;
      }
      MITK_INFO << "set Apodization Function!";
    }

    /** \brief Sets a new configuration to use
    *
    *  The Filter writes important messages that can be retrieved through this method; if nothing is to be reported, it returns "noMessage".
    *  @return The message
    */
    std::string GetMessageString()
    {
      return m_Message;
    }

    /** \brief Sets a callback for progress checking
    *
    *  An std::function<void(int, std::string)> can be set, through which progress of the currently updating filter is reported.
    *  The integer argument is a number between 0 an 100 to indicate how far completion has been achieved, the std::string argument indicates what the filter is currently doing.
    */
    void SetProgressHandle(std::function<void(int, std::string)> progressHandle);

  protected:
    BeamformingFilter();

    ~BeamformingFilter() override;

    void GenerateInputRequestedRegion() override;

    void GenerateOutputInformation() override;

    void GenerateData() override;

    //##Description
    //## @brief Time when Header was last initialized
    itk::TimeStamp m_TimeOfHeaderInitialization;

    /** \brief The std::function<void(int, std::string)>, through which progress of the currently updating filter is reported.
    */
    std::function<void(int, std::string)> m_ProgressHandle;

    float* m_OutputData;
    float* m_InputData;
    float* m_InputDataPuffer;

    /** \brief Current configuration set
    */
    BeamformingSettings::Pointer m_Conf;

    /**
    * The size of the apodization array when it last changed.
    */
    int m_LastApodizationArraySize;

    /** \brief Pointer to the GPU beamforming filter class; for performance reasons the filter is initialized within the constructor and kept for all later computations.
    */
    mitk::PhotoacousticOCLBeamformingFilter::Pointer m_BeamformingOclFilter;

    /** \brief The message returned by mitk::BeamformingFilter::GetMessageString()
    */
    std::string m_Message;
  };
} // namespace mitk

#endif //MITK_PHOTOACOUSTICS_BEAMFORMING_FILTER
