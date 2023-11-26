#ifndef AKINATOR_DEFINITION_H_
#define AKINATOR_DEFINITION_H_

#include <wchar.h>
#include "akinator.h"

AkinatorError akinatorGetDefinition     (Akinator* akin);
AkinatorError akinatorCompareDefinitions(Akinator* akin);

#endif // AKINATOR_DEFINITION_H_