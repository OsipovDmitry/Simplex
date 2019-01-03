#include <scenegraph/abstractsceneoptimizer.h>

#include "abstractsceneoptimizer_p.h"


namespace scenegraph
{

AbstractSceneOptimizer::AbstractSceneOptimizer() :
    m_(new AbstractSceneOptimizerPrivate())
{

}

AbstractSceneOptimizer::~AbstractSceneOptimizer()
{
    delete m_;
}


}
