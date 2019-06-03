/*!
 * \file   mediafoundationcamera.h
 * \author Attila Krüpl
 * \date   2019/06/03
 * \info   www.krupl.com
 */

#pragma once

/*!< External dependencies */

#include <comdef.h>
#include <atlcomcli.h>
#include <Strmif.h>
#include <Dshow.h>

#include <new>
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <Wmcodecdsp.h>
#include <assert.h>
#include <Dbt.h>
#include <shlwapi.h>
#include <mfplay.h>

#include <iostream>

/*!< Internal includes */

#include "types/helper.h"
#include "types/devicelist.h"

