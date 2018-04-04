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

#include <mitkCommon.h>
#include <chrono>
#include <mitkPATissueGeneratorParameters.h>
#include <mitkPAPhantomTissueGenerator.h>
#include <mitkIOUtil.h>
#include <mitkCommandLineParser.h>
#include <mitkUIDGenerator.h>
#include <mitkException.h>

#include <itksys/SystemTools.hxx>

using namespace mitk::pa;

TissueGeneratorParameters::Pointer CreatePhantom_08_03_18_Parameters()
{
  auto returnParameters = TissueGeneratorParameters::New();
  returnParameters->SetAirThicknessInMillimeters(12);
  returnParameters->SetMinBackgroundAbsorption(0.1);
  returnParameters->SetMaxBackgroundAbsorption(0.1);
  returnParameters->SetBackgroundAnisotropy(0.9);
  returnParameters->SetBackgroundScattering(15);
  returnParameters->SetCalculateNewVesselPositionCallback(&VesselMeanderStrategy::CalculateNewPositionInStraightLine);
  returnParameters->SetDoPartialVolume(false);
  returnParameters->SetMinNumberOfVessels(5);
  returnParameters->SetMaxNumberOfVessels(5);
  returnParameters->SetMinVesselAbsorption(10);
  returnParameters->SetMaxVesselAbsorption(10);
  returnParameters->SetMinVesselAnisotropy(0.9);
  returnParameters->SetMaxVesselAnisotropy(0.9);
  returnParameters->SetMinVesselBending(0.1);
  returnParameters->SetMaxVesselBending(0.3);
  returnParameters->SetMinVesselRadiusInMillimeters(0.5);
  returnParameters->SetMaxVesselRadiusInMillimeters(4);
  returnParameters->SetMinVesselScattering(15);
  returnParameters->SetMaxVesselScattering(15);
  returnParameters->SetMinVesselZOrigin(2.2);
  returnParameters->SetMaxVesselZOrigin(4);
  returnParameters->SetVesselBifurcationFrequency(5000);
  returnParameters->SetRandomizePhysicalProperties(false);
  returnParameters->SetSkinThicknessInMillimeters(0);
  returnParameters->SetUseRngSeed(false);
  //returnParameters->SetVoxelSpacingInCentimeters(0.015);
  //returnParameters->SetXDim(280);
  //returnParameters->SetYDim(400);
  //returnParameters->SetZDim(360);
  returnParameters->SetVoxelSpacingInCentimeters(0.0075);
  returnParameters->SetXDim(560);
  returnParameters->SetYDim(400);
  returnParameters->SetZDim(720);
  return returnParameters;
}

struct InputParameters
{
  std::string saveFolderPath;
  std::string identifyer;
  std::string exePath;
  std::string probePath;
  bool verbose;
};

InputParameters parseInput(int argc, char* argv[])
{
  MITK_INFO << "Paring arguments...";
  mitkCommandLineParser parser;
  // set general information
  parser.setCategory("MITK-Photoacoustics");
  parser.setTitle("Mitk Tissue Batch Generator");
  parser.setDescription("Creates in silico tissue in batch processing and automatically calculates fluence values for the central slice of the volume.");
  parser.setContributor("Computer Assisted Medical Interventions, DKFZ");

  // how should arguments be prefixed
  parser.setArgumentPrefix("--", "-");
  // add each argument, unless specified otherwise each argument is optional
  // see mitkCommandLineParser::addArgument for more information
  parser.beginGroup("Required parameters");
  parser.addArgument(
    "savePath", "s", mitkCommandLineParser::InputDirectory,
    "Input save folder (directory)", "input save folder",
    us::Any(), false);
  parser.addArgument(
    "mitkMcxyz", "m", mitkCommandLineParser::OutputFile,
    "MitkMcxyz binary (file)", "path to the MitkMcxyz binary",
    us::Any(), false);
  parser.endGroup();
  parser.beginGroup("Optional parameters");
  parser.addArgument(
    "probe", "p", mitkCommandLineParser::OutputFile,
    "xml probe file (file)", "file to the definition of the used probe (*.xml)",
    us::Any());
  parser.addArgument(
    "verbose", "v", mitkCommandLineParser::Bool,
    "Verbose Output", "Whether to produce verbose, or rather debug output");
  parser.addArgument(
    "identifyer", "i", mitkCommandLineParser::String,
    "Generator identifyer (string)", "A unique identifyer for the calculation instance");

  InputParameters input;

  std::map<std::string, us::Any> parsedArgs = parser.parseArguments(argc, argv);
  if (parsedArgs.size() == 0)
    exit(-1);

  if (parsedArgs.count("verbose"))
  {
    MITK_INFO << "verbose";
    input.verbose = us::any_cast<bool>(parsedArgs["verbose"]);
  }
  else
  {
    input.verbose = false;
  }

  if (parsedArgs.count("savePath"))
  {
    MITK_INFO << "savePath";
    input.saveFolderPath = us::any_cast<std::string>(parsedArgs["savePath"]);
  }

  if (parsedArgs.count("mitkMcxyz"))
  {
    MITK_INFO << "mitkMcxyz";
    input.exePath = us::any_cast<std::string>(parsedArgs["mitkMcxyz"]);
  }

  if (parsedArgs.count("probe"))
  {
    MITK_INFO << "probe";
    input.probePath = us::any_cast<std::string>(parsedArgs["probe"]);
  }

  if (parsedArgs.count("identifyer"))
  {
    MITK_INFO << "identifyer";
    input.identifyer = us::any_cast<std::string>(parsedArgs["identifyer"]);
  }
  else
  {
    MITK_INFO << "generating identifyer";
    auto uid = mitk::UIDGenerator("", 8);
    input.identifyer = uid.GetUID();
  }
  MITK_INFO << "Paring arguments...[Done]";
  return input;
}

int main(int argc, char * argv[])
{
  auto input = parseInput(argc, argv);
  double absorptions[4] = { 2, 4, 7, 10 };
  unsigned int iterationNumber = 0;

  for (; iterationNumber < 4; iterationNumber++)
  {
    auto parameters = CreatePhantom_08_03_18_Parameters();
    parameters->SetMinVesselAbsorption(absorptions[iterationNumber]);
    parameters->SetMaxVesselAbsorption(absorptions[iterationNumber]);
    MITK_INFO(input.verbose) << "Generating tissue..";
    auto resultTissue = PhantomTissueGenerator::GeneratePhantomData(parameters);
    MITK_INFO(input.verbose) << "Generating tissue..[Done]";

    auto inputfolder = std::string(input.saveFolderPath + "input/");
    auto outputfolder = std::string(input.saveFolderPath + "output/");
    if (!itksys::SystemTools::FileIsDirectory(inputfolder))
    {
      itksys::SystemTools::MakeDirectory(inputfolder);
    }
    if (!itksys::SystemTools::FileIsDirectory(outputfolder))
    {
      itksys::SystemTools::MakeDirectory(outputfolder);
    }

    std::string savePath = input.saveFolderPath + "input/Phantom_" + input.identifyer +
      "_" + std::to_string(iterationNumber) + ".nrrd";
    mitk::IOUtil::Save(resultTissue->ConvertToMitkImage(), savePath);
    std::string outputPath = input.saveFolderPath + "output/Phantom_" + input.identifyer +
      "_" + std::to_string(iterationNumber) + "/";

    resultTissue = nullptr;

    if (!itksys::SystemTools::FileIsDirectory(outputPath))
    {
      itksys::SystemTools::MakeDirectory(outputPath);
    }

    outputPath = outputPath + "Fluence_Phantom_" + input.identifyer + "_" + std::to_string(iterationNumber);

    MITK_INFO(input.verbose) << "Simulating fluence..";

    int result = -4;

    result = std::system(std::string(input.exePath + " -i " + savePath + " -o " +
      (outputPath + "_low_res.nrrd") +
      " -yo " + "0" + " -p " + input.probePath +
      " -n 10000000").c_str());

    result = std::system(std::string(input.exePath + " -i " + savePath + " -o " +
      (outputPath + "_mid_res.nrrd") +
      " -yo " + "0" + " -p " + input.probePath +
      " -n 200000000").c_str());

    MITK_INFO << result;
    MITK_INFO(input.verbose) << "Simulating fluence..[Done]";
  }
}
