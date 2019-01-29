#pragma once


#include <iostream>

int getValueOfScaleCurrent( int atMin, int atMax, double atCurrentPercent )
{
    return (int)( ( atMax - atMin ) * atCurrentPercent ) + atMin;
}

//int main()
//{
//    int aMin = -100;
//    int aMax = 100;
//
//    int bMin = 2000;
//    int bMax = 7000;
//
//    int cMin = -13;
//    int cMax = -2;
//
//
//    int scaleMin = 0;
//    int scaleMax = 1;
//
//    double scaleCurrent = 0.98654;
//
//    double currentPercent = scaleCurrent / ( scaleMax - scaleMin );
//
//    std::cout << getValueOfScaleCurrent( aMin, aMax, currentPercent ) << std::endl;
//    std::cout << getValueOfScaleCurrent( bMin, bMax, currentPercent ) << std::endl;
//    std::cout << getValueOfScaleCurrent( cMin, cMax, currentPercent ) << std::endl;
//
//
//
//    return 0;
//}