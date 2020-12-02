#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(VARIANPANEL_LIB)
#  define VARIANPANEL_EXPORT Q_DECL_EXPORT
# else
#  define VARIANPANEL_EXPORT Q_DECL_IMPORT
# endif
#else
# define VARIANPANEL_EXPORT
#endif
