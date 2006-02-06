#include "itkImageFileReader.h"

#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkBinaryMorphologicalOpeningImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

#include "itkTimeProbe.h"
#include <vector>
#include "itkMultiThreader.h"

int main(int, char * argv[])
{
  itk::MultiThreader::SetGlobalMaximumNumberOfThreads(1);

  const int dim = 3;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim >    IType;
  
  // read the input image
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::BinaryBallStructuringElement< PType, dim> KernelType;
  KernelType kernel;
  KernelType::SizeType kernelSize;
  kernelSize.Fill( 20 );
  kernelSize[2] = 7;
  kernel.SetRadius(kernelSize);
  kernel.CreateStructuringElement();
   
  typedef itk::BinaryDilateImageFilter< IType, IType, KernelType > DilateType;
  DilateType::Pointer dilate = DilateType::New();
  dilate->SetInput( reader->GetOutput() );
  dilate->SetKernel( kernel );
  
  typedef itk::BinaryErodeImageFilter< IType, IType, KernelType > ErodeType;
  ErodeType::Pointer erode = ErodeType::New();
  erode->SetInput( dilate->GetOutput() );
  erode->SetKernel( kernel );
  
  typedef itk::BinaryMorphologicalClosingImageFilter< IType, IType, KernelType > ClosingType;
  ClosingType::Pointer safe = ClosingType::New();
  safe->SetInput( reader->GetOutput() );
  safe->SetSafeBorder( true );
  safe->SetKernel( kernel );

  ClosingType::Pointer unsafe = ClosingType::New();
  unsafe->SetInput( reader->GetOutput() );
  unsafe->SetSafeBorder( false );
  unsafe->SetKernel( kernel );
  
  typedef itk::BinaryMorphologicalOpeningImageFilter< IType, IType, KernelType > OpeningType;
  OpeningType::Pointer open = OpeningType::New();
  open->SetInput( reader->GetOutput() );
  open->SetKernel( kernel );

  ErodeType::Pointer erode2 = ErodeType::New();
  erode2->SetInput( reader->GetOutput() );
  erode2->SetKernel( kernel );

  DilateType::Pointer dilate2 = DilateType::New();
  dilate2->SetInput( erode2->GetOutput() );
  dilate2->SetKernel( kernel );

  reader->Update();
  
  
  std::cout << "#v" << "\t" 
            << "de" << "\t" 
            << "ed" << "\t" 
            << "unsafe" << "\t" 
            << "safe" << "\t" 
            << "open" << "\t" 
            << std::endl;

    for( int v=100; v<=200; v+=100 )
      {
      itk::TimeProbe detime;
      itk::TimeProbe edtime;
      itk::TimeProbe utime;
      itk::TimeProbe stime;
      itk::TimeProbe otime;
  
      dilate->SetForegroundValue( v );
      erode->SetForegroundValue( v );
      safe->SetForegroundValue( v );
      unsafe->SetForegroundValue( v );
      open->SetForegroundValue( v );
      erode2->SetForegroundValue( v );
      dilate2->SetForegroundValue( v );

      for( int i=0; i<5; i++ )
        {
        detime.Start();
        erode->Update();
        detime.Stop();
        erode->Modified();
        dilate->Modified();
  
        edtime.Start();
        dilate2->Update();
        edtime.Stop();
        erode2->Modified();
        dilate2->Modified();
  
        utime.Start();
        unsafe->Update();
        utime.Stop();
        unsafe->Modified();
  
        stime.Start();
        safe->Update();
        stime.Stop();
        safe->Modified();
  
        otime.Start();
        open->Update();
        otime.Stop();
        open->Modified();
  
        }
        
      std::cout << v << "\t" 
                << detime.GetMeanTime() << "\t" 
                << edtime.GetMeanTime() << "\t" 
                << utime.GetMeanTime() << "\t" 
                << stime.GetMeanTime() << "\t" 
                << otime.GetMeanTime() << "\t" 
                << std::endl;
    }
  return 0;
}

