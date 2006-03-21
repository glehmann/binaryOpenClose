#Change PROJECT_NAME to the name of your project
PROJECT(binaryOpenClose)

#include some macros from another file...
INCLUDE(IJMacros.txt)

#The following lines are required to use Dart
ENABLE_TESTING()
INCLUDE(Dart)
FIND_PROGRAM(IMAGE_COMPARE ImageCompare)

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
OPTION(INSTALL_DEVEL_FILES "Install C++ headers" ON)
IF(INSTALL_DEVEL_FILES)
FOREACH(f itkBinaryMorphologicalClosingImageFilter.h    itkBinaryMorphologicalOpeningImageFilter.h itkBinaryMorphologicalClosingImageFilter.txx  itkBinaryMorphologicalOpeningImageFilter.txx )
  INSTALL_FILES(/include/InsightToolkit/BasicFilters FILES ${CMAKE_CURRENT_SOURCE_DIR}/${f})
ENDFOREACH(f)
ENDIF(INSTALL_DEVEL_FILES)


#the following block of code is an example of how to build an executable in
#cmake.  Unmodified, it will add an executable called "MyExe" to the project.
#MyExe will be built using the files MyClass.h and MyClass.cxx, and it will
#be linked to all the libraries you specified above. 
#You can build more than one executable per project

IF(BUILD_TESTING)

SET(CurrentExe "close")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "open")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "close_border")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "perf")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

ENDIF(BUILD_TESTING)

#the following line is an example of how to add a test to your project.
#Testname is the title for this particular test.  ExecutableToRun is the
#program which will be running this test.  It can either be a part of this
#project or an external executable.  After that list any args that are needed
#for this test.  Include as many tests as you like.  If your project doesn't have
#any tests you can comment out or delete the following line.
# ADD_TEST(Testname ExecutableToRun arg1 arg2 arg3)

ADD_TEST(RunClose close ${CMAKE_SOURCE_DIR}/images/2th_cthead1.png close-unsafe.png close-unsafe2.png close.png close2.png)
ADD_TEST(Close ${IMAGE_COMPARE} close.png ${CMAKE_SOURCE_DIR}/images/close.png)
ADD_TEST(Close2 ${IMAGE_COMPARE} close2.png ${CMAKE_SOURCE_DIR}/images/close.png)
ADD_TEST(CloseUnsafe ${IMAGE_COMPARE} close-unsafe.png ${CMAKE_SOURCE_DIR}/images/close-unsafe.png)
ADD_TEST(CloseUnsafe2 ${IMAGE_COMPARE} close-unsafe2.png ${CMAKE_SOURCE_DIR}/images/close-unsafe.png)

ADD_TEST(RunOpen open ${CMAKE_SOURCE_DIR}/images/2th_cthead1.png open.png open2.png)
ADD_TEST(Open ${IMAGE_COMPARE} open.png ${CMAKE_SOURCE_DIR}/images/open.png)
ADD_TEST(Open2 ${IMAGE_COMPARE} open2.png ${CMAKE_SOURCE_DIR}/images/open.png)

ADD_TEST(RunCloseBorder00 close_border 0 0 ${CMAKE_SOURCE_DIR}/images/2th_cthead1.png close_border00.png)
ADD_TEST(CloseBorder00 ${IMAGE_COMPARE} close_border00.png ${CMAKE_SOURCE_DIR}/images/close_border00.png)

ADD_TEST(RunCloseBorder01 close_border 0 1 ${CMAKE_SOURCE_DIR}/images/2th_cthead1.png close_border01.png)
ADD_TEST(CloseBorder01 ${IMAGE_COMPARE} close_border01.png ${CMAKE_SOURCE_DIR}/images/close_border01.png)

ADD_TEST(RunCloseBorder10 close_border 1 0 ${CMAKE_SOURCE_DIR}/images/2th_cthead1.png close_border10.png)
ADD_TEST(CloseBorder10 ${IMAGE_COMPARE} close_border10.png ${CMAKE_SOURCE_DIR}/images/close_border10.png)

ADD_TEST(RunCloseBorder11 close_border 1 1 ${CMAKE_SOURCE_DIR}/images/2th_cthead1.png close_border11.png)
ADD_TEST(CloseBorder11 ${IMAGE_COMPARE} close_border11.png ${CMAKE_SOURCE_DIR}/images/close_border11.png)

