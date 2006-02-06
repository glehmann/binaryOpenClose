#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

#include "itkBinaryMorphologicalOpeningImageFilter.h"
#include "itkBinaryBallStructuringElement.h"


int main(int, char * argv[])
{
  const int dim = 2;
  
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  typedef itk::BinaryBallStructuringElement< PType, dim> KernelType;
  KernelType ball;
  KernelType::SizeType ballSize;
  ballSize.Fill( 8 );
  ball.SetRadius(ballSize);
  ball.CreateStructuringElement();
   
  typedef itk::BinaryMorphologicalOpeningImageFilter< IType, IType, KernelType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
  filter->SetKernel( ball );
  filter->SetForegroundValue( 200 );
  filter->SetBackgroundValue( 150 );
   
  itk::SimpleFilterWatcher watcher(filter, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();
   
  FilterType::Pointer filter2 = FilterType::New();
  filter2->SetInput( filter->GetOutput() );
  filter2->SetKernel( ball );
  filter2->SetForegroundValue( 200 );
   
  WriterType::Pointer writer2 = WriterType::New();
  writer2->SetInput( filter->GetOutput() );
  writer2->SetFileName( argv[3] );
  writer2->Update();

  return 0;
}

