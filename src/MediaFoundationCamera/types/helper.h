/*!
 * \file   helper.h
 * \author Attila Krüpl
 * \date   2019/06/03
 * \info   www.krupl.com
 */

#pragma once

int getValueOfScaleCurrent( int atMin, int atMax, double atCurrentPercent )
{
    return (int)( ( atMax - atMin ) * atCurrentPercent ) + atMin;
}
