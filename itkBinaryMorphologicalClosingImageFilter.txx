/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBinaryMorphologicalClosingImageFilter.txx,v $
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkBinaryMorphologicalClosingImageFilter_txx
#define __itkBinaryMorphologicalClosingImageFilter_txx

#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkProgressAccumulator.h"
#include "itkCropImageFilter.h"
#include "itkConstantPadImageFilter.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodIterator.h"

namespace itk {

template<class TInputImage, class TOutputImage, class TKernel>
BinaryMorphologicalClosingImageFilter<TInputImage, TOutputImage, TKernel>
::BinaryMorphologicalClosingImageFilter()
  : m_Kernel()
{
  m_ClosingValue = NumericTraits<InputPixelType>::max();
  m_SafeBorder = false;
}

template <class TInputImage, class TOutputImage, class TKernel>
void 
BinaryMorphologicalClosingImageFilter<TInputImage, TOutputImage, TKernel>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  
  // We need all the input.
  InputImagePointer input = const_cast<InputImageType *>(this->GetInput());
  
  input->SetRequestedRegion( input->GetLargestPossibleRegion() );
}


template <class TInputImage, class TOutputImage, class TKernel>
void 
BinaryMorphologicalClosingImageFilter<TInputImage, TOutputImage, TKernel>
::EnlargeOutputRequestedRegion(DataObject *)
{
  this->GetOutput()
    ->SetRequestedRegion( this->GetOutput()->GetLargestPossibleRegion() );
}

template<class TInputImage, class TOutputImage, class TKernel>
void
BinaryMorphologicalClosingImageFilter<TInputImage, TOutputImage, TKernel>
::GenerateData()
{
  // Allocate the outputs
  this->AllocateOutputs();
  
  // let choose a background value. Background value should not be given by user
  // because closing is extensive so no background pixels will be added
  // it is just needed for internal erosion filter and constant padder
  InputPixelType backgroundValue = NumericTraits<InputPixelType>::Zero;
  if ( m_ClosingValue == backgroundValue )
    {
    // current background value is already used for foreground value
    // choose another one
    backgroundValue = NumericTraits<InputPixelType>::max();
    }

  /** set up erosion and dilation methods */
  typename BinaryDilateImageFilter<TInputImage, TInputImage, TKernel>::Pointer
    dilate = BinaryDilateImageFilter<TInputImage, TInputImage, TKernel>::New();

  typename BinaryErodeImageFilter<TInputImage, TOutputImage, TKernel>::Pointer
    erode = BinaryErodeImageFilter<TInputImage, TOutputImage, TKernel>::New();

  // create the pipeline without input and output image
  dilate->ReleaseDataFlagOn();
  dilate->SetKernel( this->GetKernel() );
  dilate->SetDilateValue( m_ClosingValue );

  erode->SetKernel( this->GetKernel() );
  erode->ReleaseDataFlagOn();
  erode->SetErodeValue( m_ClosingValue );
  erode->SetBackgroundValue( backgroundValue );
  erode->SetInput( dilate->GetOutput() );

  // now we have 2 cases:
  // + SafeBorder is true so we need to create a bigger image use it as input
  //   and crop the image to the normal output image size
  // + SafeBorder is false; we just have to connect filters
  if ( m_SafeBorder )
    {
    typedef typename itk::ConstantPadImageFilter<InputImageType, InputImageType> PadType;
    typename PadType::Pointer pad = PadType::New();
    pad->SetPadLowerBound( m_Kernel.GetRadius().m_Size );
    pad->SetPadUpperBound( m_Kernel.GetRadius().m_Size );
    pad->SetConstant( backgroundValue );
    pad->SetInput( this->GetInput() );

    dilate->SetInput( pad->GetOutput() );
    
    typedef typename itk::CropImageFilter<TOutputImage, TOutputImage> CropType;
    typename CropType::Pointer crop = CropType::New();
    crop->SetInput( erode->GetOutput() );
    crop->SetUpperBoundaryCropSize( m_Kernel.GetRadius() );
    crop->SetLowerBoundaryCropSize( m_Kernel.GetRadius() );
    
    ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
    progress->SetMiniPipelineFilter(this);
    progress->RegisterInternalFilter(pad, .1f);
    progress->RegisterInternalFilter(erode, .35f);
    progress->RegisterInternalFilter(dilate, .35f);
    progress->RegisterInternalFilter(crop, .1f);
    
    crop->GraftOutput( this->GetOutput() );
    /** execute the minipipeline */
    crop->Update();
  
    /** graft the minipipeline output back into this filter's output */
    this->GraftOutput( crop->GetOutput() );
    }
  else
    {
    /** set up the minipipeline */
    ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
    progress->SetMiniPipelineFilter(this);
    progress->RegisterInternalFilter(erode, .45f);
    progress->RegisterInternalFilter(dilate, .45f);
    
    dilate->SetInput( this->GetInput() );
    erode->GraftOutput( this->GetOutput() );
  
    /** execute the minipipeline */
    erode->Update();
  
    /** graft the minipipeline output back into this filter's output */
    this->GraftOutput( erode->GetOutput() );
    }

  // finally copy background which should have been eroded
  //
  // iterator on input image
  ImageRegionConstIterator<InputImageType> inIt 
            = ImageRegionConstIterator<InputImageType>( this->GetInput(),
                    this->GetOutput()->GetRequestedRegion() );
  // iterator on output image
  ImageRegionIterator<OutputImageType> outIt
            = ImageRegionIterator<OutputImageType>( this->GetOutput(),
                    this->GetOutput()->GetRequestedRegion() );
  outIt.GoToBegin(); 
  inIt.GoToBegin(); 

  while( !outIt.IsAtEnd() )
    {
    if( outIt.Get() != m_ClosingValue )
      {
      outIt.Set( static_cast<OutputPixelType>( inIt.Get() ) );
      }
    ++outIt;
    ++inIt;
    }
  
  // the end !
  this->SetProgress( 1.0f );
}

template<class TInputImage, class TOutputImage, class TKernel>
void
BinaryMorphologicalClosingImageFilter<TInputImage, TOutputImage, TKernel>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Kernel: " << m_Kernel << std::endl;
  os << indent << "ClosingValue: " << static_cast<typename NumericTraits<InputPixelType>::PrintType>(m_ClosingValue) << std::endl;
  os << indent << "SafeBorder: " << m_SafeBorder << std::endl;
}

}// end namespace itk
#endif
