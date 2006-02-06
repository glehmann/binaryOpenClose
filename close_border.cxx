#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"

#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"


int main(int, char * argv[])
{
  const int dim = 2;
  
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[3] );

  typedef itk::BinaryBallStructuringElement< PType, dim> KernelType;
  KernelType ball;
  KernelType::SizeType ballSize;
  ballSize.Fill( 40 );
  ball.SetRadius(ballSize);
  ball.CreateStructuringElement();
   
  typedef itk::BinaryDilateImageFilter< IType, IType, KernelType > DilateType;
  DilateType::Pointer dilate = DilateType::New();
  dilate->SetInput( reader->GetOutput() );
  dilate->SetKernel( ball );
  dilate->SetForegroundValue( 200 );
  dilate->SetBoundaryIsForeground( atoi(argv[1]) );
   
  typedef itk::BinaryErodeImageFilter< IType, IType, KernelType > ErodeType;
  ErodeType::Pointer erode = ErodeType::New();
  erode->SetInput( dilate->GetOutput() );
  erode->SetKernel( ball );
  erode->SetForegroundValue( 200 );
  erode->SetBoundaryIsForeground( atoi(argv[2]) );
   
  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( erode->GetOutput() );
  writer->SetFileName( argv[4] );
  writer->Update();
  writer->Update(); 
   
  return 0;
}

