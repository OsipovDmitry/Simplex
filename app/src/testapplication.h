#ifndef TESTAPPLICATION_H
#define TESTAPPLICATION_H

#include <vector>

#include <core/applicationbase.h>

#include <qt/forwarddecl.h>
#include <openal/forwarddecl.h>

class TestApplication : public simplex::core::ApplicationBase
{
public:
    TestApplication(simplex::qt::RenderWidget*,
                    const std::shared_ptr<simplex::openal::OpenALDevice>&);

    ~TestApplication() override;

    void setShadowMode(simplex::core::ShadingMode);
    void setShadowFilter(simplex::core::ShadingFilter);

protected:
    void doInitialize() override;
    void doUpdate(uint64_t time, uint32_t dt) override;

private:    
    void prepareStandardScene();
    void prepareSponzaScene();
    void prepareChessScene();
    void prepareEmptyRoomScene();
    void prepareRiggedFigureScene();

    simplex::qt::RenderWidget *m_renderWidget;
    std::shared_ptr<simplex::openal::OpenALDevice> m_audioDevice;

};

#endif // TESTAPPLICATION_H
