#include "mitkContourMapper2D.h"
#include "mitkBaseRenderer.h"
#include "mitkPlaneGeometry.h"
#include "mitkColorProperty.h"
#include "mitkProperties.h"
#include <vtkTransform.h>

#include <GL/glut.h>

mitk::ContourMapper2D::ContourMapper2D()
{
}

mitk::ContourMapper2D::~ContourMapper2D()
{
}


void mitk::ContourMapper2D::Paint(mitk::BaseRenderer * renderer)
  {
  if(IsVisible(renderer)==false) return;

  ////	@FIXME: Logik fuer update
  bool updateNeccesary=true;

  if (updateNeccesary) 
    {
    mitk::Contour::Pointer input =  const_cast<mitk::Contour*>(this->GetInput());

    // ok, das ist aus GenerateData kopiert
    mitk::DisplayGeometry::Pointer displayGeometry = renderer->GetDisplayGeometry();
    assert(displayGeometry.IsNotNull());

    //apply color and opacity read from the PropertyList
    ApplyProperties(renderer);

    vtkTransform* transform = GetDataTreeNode()->GetVtkTransform();

    //    Contour::OutputType point;
    Contour::BoundingBoxType::PointType point;

    mitk::Point3D p, projected_p;
    float vtkp[3];

    if (input->GetClosed())
      {
      glBegin (GL_LINE_LOOP);
      }
    else 
      {
      glBegin (GL_LINE_STRIP);
      }


    //Contour::InputType end = input->GetContourPath()->EndOfInput();
    //if (end > 50000) end = 0;

    mitk::Contour::PointsContainerPointer points = input->GetPoints();
    mitk::Contour::PointsContainerIterator pointsIt = points->Begin();



    while ( pointsIt != points->End() )
      {
      //while ( idx != end )
      //{
      //      point = input->GetContourPath()->Evaluate(idx);
      point = pointsIt.Value();

      itk2vtk(point, vtkp);
      transform->TransformPoint(vtkp, vtkp);
      vtk2itk(vtkp,p);

      displayGeometry->Project(p, projected_p);
      Vector3D diff=p-projected_p;
      if(diff.GetSquaredNorm()<1.0)
      {
        Point2D pt2d, tmp;
        displayGeometry->Map(projected_p, pt2d);
        displayGeometry->MMToDisplay(pt2d, pt2d);
        glVertex2f(pt2d[0], pt2d[1]);
      }

      pointsIt++;
      //      idx += 1;
      }

    glEnd ();

    }
  }

const mitk::Contour* mitk::ContourMapper2D::GetInput(void)
{
	if (this->GetNumberOfInputs() < 1)
	{
		return 0;
	}

    return static_cast<const mitk::Contour * > ( GetData() );
}


void mitk::ContourMapper2D::Update()
{
}

void mitk::ContourMapper2D::GenerateOutputInformation()
{
}
