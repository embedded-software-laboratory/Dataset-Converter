#include "CpmDatasetConverter/KoPer/KoPerParser.h"
#include "CpmDatasetConverter/KoPer/KoPerScenario.h"

namespace CpmDatasetConverter {

KoPerParser::KoPerParser() {
  this->scenarios_.push_back(new KoPerScenario("Sequence1a"));
}
}