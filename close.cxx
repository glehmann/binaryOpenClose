#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

#include "itkBinaryMorphologicalClosingImageFilter.h"
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
  ballSize.Fill( 40 );
  ball.SetRadius(ballSize);
  ball.CreateStructuringElement();
   
  typedef itk::BinaryMorphologicalClosingImageFilter< IType, IType, KernelType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
  filter->SetKernel( ball );
  filter->SetSafeBorder( false );
  filter->SetForegroundValue( 200 );
   
  itk::SimpleFilterWatcher watcher(filter, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();

  FilterType::Pointer filter2 = FilterType::New();
  filter2->SetInput( filter->GetOutput() );
  filter2->SetKernel( ball );
  filter2->SetSafeBorder( false );
  filter2->SetForegroundValue( 200 );

  WriterType::Pointer writer2 = WriterType::New();
  writer2->SetInput( filter->GetOutput() );
  writer2->SetFileName( argv[3] );
  writer2->Update();

  // safe border test
  filter->SetSafeBorder( true );
  filter2->SetSafeBorder( true );
  writer->SetFileName( argv[4] );
  writer2->SetFileName( argv[5] );
  writer->Update(); 
  writer2->Update();
   
  return 0;
}

