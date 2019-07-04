#pragma once

#include "util.h"
#include "view_id.h"

struct CollectiveName {
  optional<string> SERIAL(shortened);
  string SERIAL(full);
  string SERIAL(race);
  ViewId SERIAL(viewId);
  SERIALIZE_ALL(shortened, full, race, viewId)
};

