#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"

#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

template < class TFilter >
class ProgressCallback : public itk::Command
{
public:
  typedef ProgressCallback   Self;
  typedef itk::Command  Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  itkTypeMacro( IterationCallback, Superclass );
  itkNewMacro( Self );
  
  /** Type defining the optimizer. */
  typedef    TFilter     FilterType;

  /** Method to specify the optimizer. */
  void SetFilter( FilterType * filter )
    {
    m_Filter = filter;
    m_Filter->AddObserver( itk::ProgressEvent(), this );
    }

  /** Execute method will print data at each iteration */
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }
    
  void Execute(const itk::Object *, const itk::EventObject & event)
    {
    std::cout << m_Filter->GetNameOfClass() << ": " << m_Filter->GetProgress() << std::endl;
    }

protected:
  ProgressCallback() {};
  itk::WeakPointer<FilterType>   m_Filter;
};


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
   
  typedef ProgressCallback< FilterType > ProgressType;
  ProgressType::Pointer progress = ProgressType::New();
  progress->SetFilter( filter );

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();

  FilterType::Pointer filter2 = FilterType::New();
  filter2->SetInput( filter->GetOutput() );
  filter2->SetKernel( ball );

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

