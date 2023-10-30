/**********************************************************************
*
* FILENAME    :   version.h
* CLASS       :   %{Cpp:License:ClassName}
*
* DESCRIPTION :
*
* AUTHOR      :    Giovanni Ortu ( giovortu )
* EMAIL       :    giovanni.ortu@abinsula.com
* CREATED     :    3 / 11 / 2020
*
**********************************************************************/

#ifndef VERSION_H
#define VERSION_H

#define MAJOR 1
#define MINOR 0
#define REVISION 0
#define BUILD "-rc"

#define STRINGIFY(x) #x


#define VERSION_FULL(A,B,C,D) STRINGIFY(A) "." STRINGIFY(B) "." STRINGIFY(C) D
#define VERSIONFULL VERSION_FULL(MAJOR,MINOR,REVISION,BUILD)

#endif // VERSION_H
