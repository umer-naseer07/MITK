/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "QmitkBinaryThresholdULToolGUI.h"
#include "QmitkConfirmSegmentationDialog.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qslider.h>

MITK_TOOL_GUI_MACRO(MITKSEGMENTATIONUI_EXPORT, QmitkBinaryThresholdULToolGUI, "")

QmitkBinaryThresholdULToolGUI::QmitkBinaryThresholdULToolGUI() : QmitkToolGUI()
{
  // create the visible widgets
  QBoxLayout *mainLayout = new QVBoxLayout(this);

  QLabel *label = new QLabel("Threshold :", this);
  QFont f = label->font();
  f.setBold(false);
  label->setFont(f);
  mainLayout->addWidget(label);

  QBoxLayout *layout = new QHBoxLayout();

  m_DoubleThresholdSlider = new ctkRangeWidget();
  connect(
    m_DoubleThresholdSlider, SIGNAL(valuesChanged(double, double)), this, SLOT(OnThresholdsChanged(double, double)));
  layout->addWidget(m_DoubleThresholdSlider);
  mainLayout->addLayout(layout);
  m_DoubleThresholdSlider->setSingleStep(0.01);

  QPushButton *okButton = new QPushButton("Confirm Segmentation", this);
  connect(okButton, SIGNAL(clicked()), this, SLOT(OnAcceptThresholdPreview()));
  okButton->setFont(f);
  mainLayout->addWidget(okButton);

  connect(this, SIGNAL(NewToolAssociated(mitk::Tool *)), this, SLOT(OnNewToolAssociated(mitk::Tool *)));
}

QmitkBinaryThresholdULToolGUI::~QmitkBinaryThresholdULToolGUI()
{
  // !!!
  if (m_BinaryThresholdULTool.IsNotNull())
  {
    m_BinaryThresholdULTool->IntervalBordersChanged -=
      mitk::MessageDelegate3<QmitkBinaryThresholdULToolGUI, double, double, bool>(
        this, &QmitkBinaryThresholdULToolGUI::OnThresholdingIntervalBordersChanged);
    m_BinaryThresholdULTool->ThresholdingValuesChanged -=
      mitk::MessageDelegate2<QmitkBinaryThresholdULToolGUI, mitk::ScalarType, mitk::ScalarType>(
        this, &QmitkBinaryThresholdULToolGUI::OnThresholdingValuesChanged);
  }
}

void QmitkBinaryThresholdULToolGUI::OnNewToolAssociated(mitk::Tool *tool)
{
  if (m_BinaryThresholdULTool.IsNotNull())
  {
    m_BinaryThresholdULTool->IntervalBordersChanged -=
      mitk::MessageDelegate3<QmitkBinaryThresholdULToolGUI, double, double, bool>(
        this, &QmitkBinaryThresholdULToolGUI::OnThresholdingIntervalBordersChanged);
    m_BinaryThresholdULTool->ThresholdingValuesChanged -=
      mitk::MessageDelegate2<QmitkBinaryThresholdULToolGUI, mitk::ScalarType, mitk::ScalarType>(
        this, &QmitkBinaryThresholdULToolGUI::OnThresholdingValuesChanged);
  }

  m_BinaryThresholdULTool = dynamic_cast<mitk::BinaryThresholdULTool *>(tool);

  if (m_BinaryThresholdULTool.IsNotNull())
  {
    m_BinaryThresholdULTool->IntervalBordersChanged +=
      mitk::MessageDelegate3<QmitkBinaryThresholdULToolGUI, double, double, bool>(
        this, &QmitkBinaryThresholdULToolGUI::OnThresholdingIntervalBordersChanged);
    m_BinaryThresholdULTool->ThresholdingValuesChanged +=
      mitk::MessageDelegate2<QmitkBinaryThresholdULToolGUI, mitk::ScalarType, mitk::ScalarType>(
        this, &QmitkBinaryThresholdULToolGUI::OnThresholdingValuesChanged);
  }
}

void QmitkBinaryThresholdULToolGUI::OnAcceptThresholdPreview()
{
  QmitkConfirmSegmentationDialog dialog;
  QString segName = QString::fromStdString(m_BinaryThresholdULTool->GetCurrentSegmentationName());

  dialog.SetSegmentationName(segName);
  int result = dialog.exec();

  switch (result)
  {
    case QmitkConfirmSegmentationDialog::CREATE_NEW_SEGMENTATION:
      m_BinaryThresholdULTool->SetOverwriteExistingSegmentation(false);
      break;
    case QmitkConfirmSegmentationDialog::OVERWRITE_SEGMENTATION:
      m_BinaryThresholdULTool->SetOverwriteExistingSegmentation(true);
      break;
    case QmitkConfirmSegmentationDialog::CANCEL_SEGMENTATION:
      return;
  }

  if (m_BinaryThresholdULTool.IsNotNull())
  {
    m_BinaryThresholdULTool->AcceptCurrentThresholdValue();
  }
}

void QmitkBinaryThresholdULToolGUI::OnThresholdingIntervalBordersChanged(double lower, double upper, bool isFloat)
{
  if (!isFloat)
  {
    m_DoubleThresholdSlider->setRange(int(lower), int(upper));
    m_DoubleThresholdSlider->setSingleStep(1);
    m_DoubleThresholdSlider->setDecimals(0);
  }
  else
  {
    m_DoubleThresholdSlider->setRange(lower, upper);
  }
}

void QmitkBinaryThresholdULToolGUI::OnThresholdingValuesChanged(mitk::ScalarType lower, mitk::ScalarType upper)
{
  m_DoubleThresholdSlider->setValues(lower, upper);
}

void QmitkBinaryThresholdULToolGUI::OnThresholdsChanged(double min, double max)
{
  m_BinaryThresholdULTool->SetThresholdValues(min, max);
}
