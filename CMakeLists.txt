#Change PROJECT_NAME to the name of your project
PROJECT(binaryOpenClose)

#include some macros from another file...
INCLUDE(IJMacros.txt)

#The following lines are required to use Dart
ENABLE_TESTING()
INCLUDE(Dart)

#Declare any external dependencies that your project may have here.
#examples include: ITK, VTK, JPEG, PNG, OpenGL, ZLIB, Perl, Java
#If you're not sure what name to use, look in the Modules directory of your
#cmake install and check that a file named Find(Package).cmake exists
SET(Required_Packages
  ITK
)

#this foreach loads all of the packages that you specified as required.
#It shouldn't need to be modified.
FOREACH(Package ${Required_Packages})
  LOADPACKAGE(${Package})
ENDFOREACH(Package)

#Set any libraries that your project depends on.
#examples: ITKCommon, VTKRendering, etc
SET(Libraries
  ITKCommon
  ITKBasicFilters
  ITKIO
)

OPTION(BUILD_WRAPPERS "Wrap library" OFF)
IF(BUILD_WRAPPERS)
   SUBDIRS(Wrapping)
ENDIF(BUILD_WRAPPERS)


# install devel files
FOREACH(f itkBinaryMorphologicalClosingImageFilter.h    itkBinaryMorphologicalOpeningImageFilter.h itkBinaryMorphologicalClosingImageFilter.txx  itkBinaryMorphologicalOpeningImageFilter.txx )
  INSTALL_FILES(/include/InsightToolkit/BasicFilters FILES ${CMAKE_CURRENT_SOURCE_DIR}/${f})
ENDFOREACH(f)


#the following block of code is an example of how to build an executable in
#cmake.  Unmodified, it will add an executable called "MyExe" to the project.
#MyExe will be built using the files MyClass.h and MyClass.cxx, and it will
#be linked to all the libraries you specified above. 
#You can build more than one executable per project
ADD_EXECUTABLE(close close.cxx) 
TARGET_LINK_LIBRARIES(close ${Libraries})

ADD_EXECUTABLE(open open.cxx) 
TARGET_LINK_LIBRARIES(open ${Libraries})

ADD_EXECUTABLE(close_border close_border.cxx) 
TARGET_LINK_LIBRARIES(close_border ${Libraries})

#the following line is an example of how to add a test to your project.
#Testname is the title for this particular test.  ExecutableToRun is the
#program which will be running this test.  It can either be a part of this
#project or an external executable.  After that list any args that are needed
#for this test.  Include as many tests as you like.  If your project doesn't have
#any tests you can comment out or delete the following line.
# ADD_TEST(Testname ExecutableToRun arg1 arg2 arg3)

ADD_TEST(RunClose close ${CMAKE_SOURCE_DIR}/2th_cthead1.png close-unsafe.png close-unsafe2.png close.png close2.png)
ADD_TEST(Close ImageCompare close.png ${CMAKE_SOURCE_DIR}/close.png)
ADD_TEST(Close2 ImageCompare close2.png ${CMAKE_SOURCE_DIR}/close.png)
ADD_TEST(CloseUnsafe ImageCompare close-unsafe.png ${CMAKE_SOURCE_DIR}/close-unsafe.png)
ADD_TEST(CloseUnsafe2 ImageCompare close-unsafe2.png ${CMAKE_SOURCE_DIR}/close-unsafe.png)

ADD_TEST(RunOpen open ${CMAKE_SOURCE_DIR}/2th_cthead1.png open.png open2.png)
ADD_TEST(Open ImageCompare open.png ${CMAKE_SOURCE_DIR}/open.png)
ADD_TEST(Open2 ImageCompare open2.png ${CMAKE_SOURCE_DIR}/open.png)
